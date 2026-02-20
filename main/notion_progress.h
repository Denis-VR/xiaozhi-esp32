#ifndef NOTION_PROGRESS_H
#define NOTION_PROGRESS_H

#include <string>
#include <functional>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class NotionProgress {
public:
    using ProgressCallback = std::function<void(float, const std::string&)>;

    NotionProgress();
    ~NotionProgress();

    void Start(ProgressCallback callback);
    void Stop();

    float GetProgress() const { return progress_.load(); }

private:
    static void FetchTaskFunction(void* pvParameters);
    bool FetchFromNotion();

    ProgressCallback callback_;
    std::atomic<float> progress_{0.0f};
    std::string category_name_;
    TaskHandle_t task_handle_ = nullptr;
    std::atomic<bool> running_{false};

    static constexpr int FETCH_INTERVAL_MS = 5 * 60 * 1000;
};

#endif // NOTION_PROGRESS_H
