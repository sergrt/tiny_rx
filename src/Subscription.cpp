#include "Subscription.h"
#include "Guid.h"
#include "Log.h"

namespace tirx {

using namespace utils;

Subscription::Subscription() {
    uuid_ = utils::get_uuid();
    log(LogSeverity::Trace, "Default constructing subscription " + uuid_);
};

Subscription::Subscription(IObservable* observable, const std::string& subscriber_id) {
    uuid_ = utils::get_uuid();
    log(LogSeverity::Trace, "Constructing subscription " + uuid_);
    observable_ = observable;
    subscriber_uuid_ = subscriber_id;
    valid_ = std::make_shared<bool>(true);
}

Subscription::Subscription(const Subscription& other) {
    log(LogSeverity::Trace, "Copying subscription " + uuid_);
    observable_ = other.observable_;
    subscriber_uuid_ = other.subscriber_uuid_;
    uuid_ = other.uuid_;
    valid_ = other.valid_;
}

Subscription::Subscription(Subscription&& other) noexcept
    : Subscription() {
    log(LogSeverity::Trace, "Move construct subscription " + uuid_);
    other.swap(*this);
}

Subscription& Subscription::operator=(Subscription other) noexcept {
    log(LogSeverity::Trace, "Operator= subscription " + uuid_);
    other.swap(*this);
    return *this;
}

void Subscription::swap(Subscription& other) {
    std::swap(valid_, other.valid_);
    std::swap(observable_, other.observable_);
    std::swap(subscriber_uuid_, other.subscriber_uuid_);
    std::swap(uuid_, other.uuid_);
}

Subscription::~Subscription() {
    log(LogSeverity::Trace, "Destructing subscription " + uuid_);
}

void Subscription::reset() {
    *valid_ = false;
}

void Subscription::unsubscribe() {
    if (*valid_) {
        auto linked_subscription = observable_->get_linked_subscription();
        if (!linked_subscription || observable_->subscribers_count() != 1) {
            observable_->unsubscribe(subscriber_uuid_);
        } else {
            linked_subscription->unsubscribe();
        }
    }
    reset();
}

std::string Subscription::get_uuid() const {
    return subscriber_uuid_;
}

}
