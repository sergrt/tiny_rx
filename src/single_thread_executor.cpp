#include "single_thread_executor.h"

#include "log.h"

namespace tiny_rx {

SingleThreadExecutor::SingleThreadExecutor() {
    thread_ = std::thread([this]() {
        while (!stop_thread_) {
            auto lock = std::unique_lock<std::mutex>(mutex_);
            cond_var_.wait(lock, [this]() {return !tasks_.empty() || stop_thread_; });
            if (stop_thread_)
                break;

            const auto task = tasks_.front();
            tasks_.pop_front();
            lock.unlock();

            try {
                task();
            } catch (const std::exception& e) {
                log(utils::LogSeverity::Error, "SingleThreadExecutor exception on task: ", e.what());
            }
        }
    });
}

SingleThreadExecutor::~SingleThreadExecutor() {
    stop_thread_ = true;
    cond_var_.notify_all();
    thread_.join();
}

void SingleThreadExecutor::add_task(std::function<void()> f) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.emplace_back(std::move(f));
    }
    cond_var_.notify_all();
}

} // namespace tiny_rx
