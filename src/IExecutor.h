#pragma once

#include <functional>

namespace tiny_rx {

class IExecutor {
public:
    virtual ~IExecutor() = default;
    virtual void add_task(std::function<void()> f) = 0;
};

} // namespace tiny_rx
