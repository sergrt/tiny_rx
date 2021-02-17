#include "ThreadPoolExecutor.h"

namespace tirx {

ThreadPoolExecutor::ThreadPoolExecutor(size_t pool_size) : pool_size_(pool_size) {
    for (size_t x = 0; x < pool_size_; ++x) {
        threads_.emplace_back(std::thread([this]() {
            while (!stop_thread_) {

                auto lock = std::unique_lock<std::mutex>(mutex_);
                cond_var_.wait(lock, [this]() {return !jobs_.empty() || stop_thread_; });
                if (stop_thread_)
                    break;

                const auto job = jobs_.front();
                jobs_.pop_front();
                lock.unlock();

                cond_var_.notify_all();

                job();
            }
        }));
    }
}

ThreadPoolExecutor::~ThreadPoolExecutor() {
    stop_thread_ = true;
    cond_var_.notify_all();
    for (auto& t : threads_)
        t.join();
}

void ThreadPoolExecutor::add_task(std::function<void()> f) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        jobs_.emplace_back(std::move(f));
    }
    cond_var_.notify_all();
}

}