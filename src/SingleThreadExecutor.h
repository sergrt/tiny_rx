#pragma once

#include "IExecutor.h"

#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace tirx {

class SingleThreadExecutor : public IExecutor {
public:
    SingleThreadExecutor();
    ~SingleThreadExecutor() override;
    SingleThreadExecutor(const SingleThreadExecutor&) = delete;
    SingleThreadExecutor(SingleThreadExecutor&&) = delete;
    SingleThreadExecutor& operator=(const SingleThreadExecutor&) = delete;
    SingleThreadExecutor& operator=(SingleThreadExecutor&&) = delete;

    void add_task(std::function<void()> f) override;

private:
    std::thread thread_;
    std::deque<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::atomic<bool> stop_thread_{ false };
};

}
