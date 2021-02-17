#pragma once

#include "IExecutor.h"

#include <deque>
#include <functional>
#include <mutex>

namespace tirx {

class RunLoopExecutor : public IExecutor {
public:
    void add_task(std::function<void()> f) override;
    void dispatch();
    size_t size() const;

private:
    std::deque<std::function<void()>> jobs_;
    mutable std::mutex mutex_;
};

}

