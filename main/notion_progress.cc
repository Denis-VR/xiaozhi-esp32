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
    vTaskDelay(pdMS_TO_TICKS(15000));

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

    http->SetContent(std::string("{}"));

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
    int result_count = cJSON_IsArray(results) ? cJSON_GetArraySize(results) : 0;
    ESP_LOGI(TAG, "Results count: %d", result_count);
    for (int i = 0; i < result_count; i++) {
        cJSON* item = cJSON_GetArrayItem(results, i);
        cJSON* properties = cJSON_GetObjectItem(item, "properties");
        if (!properties) continue;

        cJSON* name_prop = cJSON_GetObjectItemCaseSensitive(properties, "Name");
        cJSON* title_arr = name_prop ? cJSON_GetObjectItem(name_prop, "title") : nullptr;
        cJSON* title0 = (title_arr && cJSON_IsArray(title_arr)) ? cJSON_GetArrayItem(title_arr, 0) : nullptr;
        cJSON* plain_text = title0 ? cJSON_GetObjectItem(title0, "plain_text") : nullptr;
        if (!plain_text || !cJSON_IsString(plain_text)) continue;

        ESP_LOGI(TAG, "Entry [%d]: %s", i, plain_text->valuestring);
        if (strcmp(plain_text->valuestring, "Must Have") != 0) continue;

        cJSON* progress_prop = cJSON_GetObjectItemCaseSensitive(properties, "Progress");
        cJSON* rollup = progress_prop ? cJSON_GetObjectItemCaseSensitive(progress_prop, "rollup") : nullptr;
        cJSON* number = rollup ? cJSON_GetObjectItemCaseSensitive(rollup, "number") : nullptr;
        if (cJSON_IsNumber(number)) {
            progress_ = (float)number->valuedouble;
            success = true;
            ESP_LOGI(TAG, "Progress: %f (pct: %d%%)", number->valuedouble, (int)(number->valuedouble * 100.0));
        } else {
            ESP_LOGW(TAG, "Progress.rollup.number not found");
        }
        break;
    }
    if (!success) {
        ESP_LOGW(TAG, "'Must Have' not found in %d results", result_count);
    }

    cJSON_Delete(root);
    return success;
}
