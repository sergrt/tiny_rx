#include "RunLoopExecutor.h"

namespace tirx {

void RunLoopExecutor::add_task(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(mutex_);
    jobs_.emplace_back(std::move(f));
}

void RunLoopExecutor::dispatch() {
    std::unique_lock<std::mutex> lock(mutex_);
    const auto job = jobs_.front();
    jobs_.pop_front();
    lock.unlock();

    job();
}

size_t RunLoopExecutor::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return jobs_.size();
}

}