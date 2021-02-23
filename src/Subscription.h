#pragma once

#include "IObservable.h"

#include <memory>
#include <string>

namespace tirx {

class Subscription {
public:
    Subscription();
    Subscription(IObservable* observable, const std::string& subscriber_id);
    // TODO: remove these operations - they're here for debug purposes only
    Subscription(const Subscription& other);
    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(const Subscription other) noexcept;
    ~Subscription();

    void reset();
    void unsubscribe();
    [[nodiscard]] std::string get_uuid() const;

private:
    void swap(Subscription& other) noexcept;

    std::shared_ptr<bool> valid_ = std::make_shared<bool>(false);
    IObservable* observable_{nullptr};
    std::string subscriber_uuid_;
    std::string uuid_;
};

}
