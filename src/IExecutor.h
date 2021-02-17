#pragma once

#include <functional>

namespace tirx {

class IExecutor {
public:
    virtual ~IExecutor() = default;
    virtual void add_task(std::function<void()> f) = 0;
};

}
