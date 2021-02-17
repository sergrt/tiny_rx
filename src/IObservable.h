#pragma once

#include <optional>
#include <string>

namespace tirx {

class Subscription;

class IObservable {
public:
    virtual ~IObservable() = default;
    virtual void unsubscribe(const std::string& uuid) = 0;
    virtual std::optional<Subscription> get_linked_subscription() = 0;
    virtual size_t subscribers_count() const = 0;
};

}
