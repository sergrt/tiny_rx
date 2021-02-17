#include "Subscription.h"

#include "Guid.h"

#include <iostream>

namespace tirx {

Subscription::Subscription() {
    uuid_ = utils::get_uuid();
};

Subscription::Subscription(IObservable* observable, const std::string& subscriber_id) {
    uuid_ = utils::get_uuid();
    std::cout << "Constructing subscription " << uuid_ << "\n";
    observable_ = observable;
    subscriber_id_ = subscriber_id;
    valid_ = std::make_shared<bool>(true);
}

Subscription::Subscription(const Subscription& other) {
    std::cout << "Copying subscription " << other.uuid_ << "\n";
    observable_ = other.observable_;
    subscriber_id_ = other.subscriber_id_;
    uuid_ = other.uuid_;
    valid_ = other.valid_;
}

Subscription::Subscription(Subscription&& other) {
    std::cout << "Move construct subscription " << other.uuid_ << "\n";
    observable_ = other.observable_;
    subscriber_id_ = other.subscriber_id_;
    uuid_ = other.uuid_;
    valid_ = other.valid_;
}

Subscription& Subscription::operator=(const Subscription& other) {
    std::cout << "Operator= subscription " << other.uuid_ << "\n";
    observable_ = other.observable_;
    subscriber_id_ = other.subscriber_id_;
    uuid_ = other.uuid_;
    valid_ = other.valid_;
    return *this;
}

Subscription& Subscription::operator=(Subscription&& other) {
    std::cout << "Move operator= subscription " << other.uuid_ << "\n";
    observable_ = other.observable_;
    subscriber_id_ = other.subscriber_id_;
    uuid_ = other.uuid_;
    valid_ = other.valid_;
    return *this;
}

Subscription::~Subscription() {
    try {
        std::cout << "Destructing subscription " << uuid_ << "\n";
    } catch(...) {
        
    }
}

void Subscription::reset() {
    *valid_ = false;
}

void Subscription::unsubscribe() {
    if (*valid_) {
        auto linked_subscription = observable_->get_linked_subscription();
        if (!linked_subscription || observable_->subscribers_count() != 1) {
            observable_->unsubscribe(subscriber_id_);
        } else {
            linked_subscription->unsubscribe();
        }
    }
    reset();
}

std::string Subscription::get_uuid() const {
    return subscriber_id_;
}

}
