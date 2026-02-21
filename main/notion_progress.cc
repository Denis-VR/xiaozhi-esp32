#include "notion_progress.h"
#include "notion_secrets.h"
#include "board.h"
#include <cJSON.h>
#include <esp_log.h>

#define TAG "NotionProgress"

NotionProgress::NotionProgress() {}

NotionProgress::~NotionProgress() {
    Stop();
}

void NotionProgress::Start(ProgressCallback callback) {
    callback_ = callback;
    running_ = true;
    xTaskCreatePinnedToCore(
        FetchTaskFunction,
        "notion_fetch",
        8 * 1024,
        this,
        2,
        &task_handle_,
        0
    );
}

void NotionProgress::Stop() {
    running_ = false;
    if (task_handle_ != nullptr) {
        task_handle_ = nullptr;
    }
}

void NotionProgress::FetchTaskFunction(void* pvParameters) {
    NotionProgress* self = static_cast<NotionProgress*>(pvParameters);

    // Wait for network to connect
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (self->running_) {
        if (self->FetchFromNotion()) {
            ESP_LOGI(TAG, "Progress: %.1f%%, Category: %s",
                     self->progress_.load() * 100.0f,
                     self->category_name_.c_str());
            if (self->callback_) {
                self->callback_(self->progress_.load(), self->category_name_);
            }
        } else {
            ESP_LOGW(TAG, "Failed to fetch from Notion API");
        }
        vTaskDelay(pdMS_TO_TICKS(self->FETCH_INTERVAL_MS));
    }
    vTaskDelete(NULL);
}

bool NotionProgress::FetchFromNotion() {
    auto& board = Board::GetInstance();
    auto network = board.GetNetwork();
    if (network == nullptr) {
        ESP_LOGW(TAG, "Network not available");
        return false;
    }
    auto http = network->CreateHttp(0);

    // Set timeout to 15 seconds
    http->SetTimeout(15000);

    std::string auth = "Bearer ";
    auth += NOTION_API_KEY;
    http->SetHeader("Authorization", auth);
    http->SetHeader("Notion-Version", "2022-06-28");
    http->SetHeader("Content-Type", "application/json");

    std::string body = "{}";
    http->SetContent(std::move(body));

    std::string url = "https://api.notion.com/v1/databases/";
    url += NOTION_DATABASE_ID;
    url += "/query";

    ESP_LOGI(TAG, "Opening connection...");
    if (!http->Open("POST", url)) {
        ESP_LOGE(TAG, "Failed to open HTTP connection");
        return false;
    }

    int status_code = http->GetStatusCode();
    ESP_LOGI(TAG, "HTTP status code: %d", status_code);
    if (status_code != 200) {
        ESP_LOGE(TAG, "Unexpected HTTP status: %d", status_code);
        http->Close();
        return false;
    }

    // Read body using Read() loop instead of ReadAll() which hangs
    ESP_LOGI(TAG, "Reading response body (Content-Length: %d)...", (int)http->GetBodyLength());
    std::string response;
    char buf[1024];
    int total_read = 0;
    while (true) {
        int n = http->Read(buf, sizeof(buf));
        if (n <= 0) {
            ESP_LOGI(TAG, "Read() returned %d, done reading", n);
            break;
        }
        response.append(buf, n);
        total_read += n;
        ESP_LOGI(TAG, "Read %d bytes (total: %d)", n, total_read);
    }
    http->Close();

    ESP_LOGI(TAG, "Total response: %d bytes", (int)response.size());
    if (response.empty()) {
        ESP_LOGE(TAG, "Empty response body");
        return false;
    }

    // Log first 300 chars
    ESP_LOGI(TAG, "Response: %.300s", response.c_str());

    cJSON* root = cJSON_Parse(response.c_str());
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return false;
    }

    bool success = false;
    cJSON* results = cJSON_GetObjectItem(root, "results");
    if (cJSON_IsArray(results)) {
        int count = cJSON_GetArraySize(results);
        ESP_LOGI(TAG, "Results array size: %d", count);

        if (count > 0) {
            cJSON* first_result = cJSON_GetArrayItem(results, 0);
            cJSON* properties = cJSON_GetObjectItem(first_result, "properties");

            if (!properties) {
                ESP_LOGW(TAG, "No 'properties' in first result");
            } else {
                // Extract Progress
                cJSON* progress_prop = cJSON_GetObjectItem(properties, "Progress");
                if (progress_prop) {
                    cJSON* rollup = cJSON_GetObjectItem(progress_prop, "rollup");
                    if (rollup) {
                        cJSON* number = cJSON_GetObjectItem(rollup, "number");
                        if (cJSON_IsNumber(number)) {
                            progress_ = (float)number->valuedouble;
                            success = true;
                            ESP_LOGI(TAG, "Parsed progress: %f", number->valuedouble);
                        } else {
                            ESP_LOGW(TAG, "Progress.rollup.number is not a number");
                        }
                    } else {
                        ESP_LOGW(TAG, "Progress.rollup not found");
                    }
                } else {
                    ESP_LOGW(TAG, "Progress property not found");
                }

                // Extract Name
                cJSON* name_prop = cJSON_GetObjectItem(properties, "Name");
                if (name_prop) {
                    cJSON* title = cJSON_GetObjectItem(name_prop, "title");
                    if (cJSON_IsArray(title) && cJSON_GetArraySize(title) > 0) {
                        cJSON* first_title = cJSON_GetArrayItem(title, 0);
                        cJSON* plain_text = cJSON_GetObjectItem(first_title, "plain_text");
                        if (cJSON_IsString(plain_text)) {
                            category_name_ = plain_text->valuestring;
                            ESP_LOGI(TAG, "Parsed category: %s", category_name_.c_str());
                        }
                    }
                } else {
                    ESP_LOGW(TAG, "Name property not found");
                }
            }
        }
    } else {
        ESP_LOGW(TAG, "No 'results' array in response");
    }

    cJSON_Delete(root);
    return success;
}
