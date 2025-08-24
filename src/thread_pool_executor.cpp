#include "thread_pool_executor.h"

#include "log.h"

namespace tiny_rx {

ThreadPoolExecutor::ThreadPoolExecutor(size_t pool_size) : pool_size_(pool_size) {
    for (size_t x = 0; x < pool_size_; ++x) {
        threads_.emplace_back([this]() {
            while (!stop_thread_) {

                auto lock = std::unique_lock<std::mutex>(mutex_);
                cond_var_.wait(lock, [this]() {return !tasks_.empty() || stop_thread_; });
                if (stop_thread_)
                    break;

                const auto task = tasks_.front();
                tasks_.pop_front();
                lock.unlock();

                cond_var_.notify_all();

                try{
                    task();
                } catch(std::exception& e) {
                    log(utils::LogSeverity::Error, "ThreadPoolExecutor exception on taslk: ", e.what());
                }
            }
        });
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
        tasks_.emplace_back(std::move(f));
    }
    cond_var_.notify_all();
}

} // namespace tiny_rx
