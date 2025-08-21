#pragma once

#include "guid.h"

#include <optional>

namespace tiny_rx {

class Subscription;

class IObservable {
public:
    virtual ~IObservable() = default;
    virtual void unsubscribe(const Guid& uuid) = 0;
    virtual std::optional<Subscription> get_linked_subscription() = 0;
    virtual size_t subscribers_count() const = 0;
};

} // namespace tiny_rx
