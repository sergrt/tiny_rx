#pragma once

#include "IExecutor.h"

#include <deque>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace tirx {

class ThreadPoolExecutor : public IExecutor {
public:
    ThreadPoolExecutor(size_t pool_size = std::thread::hardware_concurrency());
    ~ThreadPoolExecutor() override;
    ThreadPoolExecutor(ThreadPoolExecutor&&) = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor& operator=(ThreadPoolExecutor&&) = delete;

    void add_task(std::function<void()> f) override;

private:
    std::vector<std::thread> threads_;
    std::deque<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::atomic<bool> stop_thread_{ false };
    size_t pool_size_ = std::thread::hardware_concurrency();
};

}