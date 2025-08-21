#include "run_loop_executor.h"

namespace tiny_rx {

void RunLoopExecutor::add_task(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.emplace_back(std::move(f));
}

void RunLoopExecutor::dispatch() {
    std::unique_lock<std::mutex> lock(mutex_);
    const auto task = tasks_.front();
    tasks_.pop_front();
    lock.unlock();

    task();
}

size_t RunLoopExecutor::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}

} // namespace tiny_rx
