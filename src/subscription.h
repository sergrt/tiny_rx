#pragma once

#include "iobservable.h"

#include <memory>

namespace tiny_rx {

class Subscription {
public:
    Subscription();
    Subscription(IObservable* observable, const Guid& subscriber_id);
    // TODO: remove these operations - they're here for debug purposes only
    Subscription(const Subscription& other);
    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(Subscription other) noexcept;
    ~Subscription();

    void reset();
    void unsubscribe();
    [[nodiscard]] Guid get_uuid() const;

private:
    void swap(Subscription& other) noexcept;

    std::shared_ptr<bool> valid_ = std::make_shared<bool>(false);
    IObservable* observable_{nullptr};
    Guid subscriber_uuid_;
    Guid uuid_;
};

} // namespace tiny_rx
