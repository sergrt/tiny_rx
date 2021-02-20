#pragma once

#include "IExecutor.h"

#include <deque>
#include <functional>
#include <mutex>

namespace tirx {

class RunLoopExecutor : public IExecutor {
public:
    RunLoopExecutor(const RunLoopExecutor&) = delete;
    RunLoopExecutor(RunLoopExecutor&&) = delete;
    RunLoopExecutor& operator=(const RunLoopExecutor&) = delete;
    RunLoopExecutor& operator=(RunLoopExecutor&&) = delete;

    void add_task(std::function<void()> f) override;
    void dispatch();
    size_t size() const;

private:
    std::deque<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
};

}

