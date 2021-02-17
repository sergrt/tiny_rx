#include "SingleThreadExecutor.h"

namespace tirx {

SingleThreadExecutor::SingleThreadExecutor() {
    thread_ = std::thread([this]() {
        while (!stop_thread_) {
            auto lock = std::unique_lock<std::mutex>(mutex_);
            cond_var_.wait(lock, [this]() {return !jobs_.empty() || stop_thread_; });
            if (stop_thread_)
                break;

            const auto job = jobs_.front();
            jobs_.pop_front();
            lock.unlock();

            job();
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
        jobs_.emplace_back(std::move(f));
    }
    cond_var_.notify_all();
}

}
