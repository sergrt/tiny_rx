#pragma once

#include "ExecutionPolicy.h"
#include "Subscriber.h"
#include "IExecutor.h"
#include "Guid.h"
#include "IObservable.h"
#include "Subscription.h"

#include <list>
#include <string>
#include <memory>
#include <optional>

namespace tirx {

template<typename ...T>
class Observable : public IObservable, public std::enable_shared_from_this<IObservable> {
public:
    Observable() {
        log(LogSeverity::Trace, "Constructing observable " + uuid_);
        set_default_params();
    }

    // This is usually not what is intended
    Observable(const Observable& other) = delete;
    Observable(Observable&& other) = default;
    Observable& operator=(const Observable& other) = delete;
    Observable& operator=(Observable&& other) = default;


    void set_default_params() {
        executor_.reset();
        execution_policy_ = ExecutionPolicy::NoExecutor;
    }

    ~Observable() override {
        log(LogSeverity::Trace, "Destructing observable " + uuid_);
        for (auto& s : subscriptions_) {
            s.reset();
        }
    }

    Observable& subscribe_on(std::shared_ptr<IExecutor> executor) {
        executor_ = std::move(executor);
        execution_policy_ = ExecutionPolicy::Executor;
        return *this;
    }

    template<typename F, std::enable_if_t<std::is_object_v<F>, bool> = true>
    Subscription subscribe(std::shared_ptr<F> object) {
        return subscribe(
            [object](T... args) { object->on_next(std::move(args...)); },
            [object]() { object->on_end(); },
            [object](std::string descr) {object->on_error(std::move(descr)); }
        );
    }

    template<typename ...F>
    Subscription subscribe(F... args) {
        const std::size_t size = sizeof...(F);

        subscribers_.emplace_back(Subscriber<T...>());
        auto& subscriber = subscribers_.back();

        auto params = std::tuple<F...>(args...);
        subscriber.set_function(std::move(std::get<0>(params)));
        if constexpr (size > 1)
            subscriber.set_on_end(std::move(std::get<1>(params)));
        if constexpr (size > 2)
            subscriber.set_on_error(std::move(std::get<2>(params)));

        subscriber.set_execution_policy(execution_policy_);
        subscriber.set_executor(std::move(executor_));
        set_default_params();

        subscriptions_.emplace_back(this, subscriber.get_uuid());
        return subscriptions_.back();
        //return Subscription(this, subscriber.get_uuid());
    }

    void unsubscribe(const std::string& uuid) override {
        for (auto i = subscribers_.begin(); i != subscribers_.end(); ++i) {
            if (uuid == i->get_uuid()) {
                subscribers_.erase(i);
                break;
            }
        }

        /*if (subscribers_.size() == 1 && linked_subscription_) {
            linked_subscription_->unsubscribe();
        }*/
    }

    std::optional<Subscription> get_linked_subscription() override {
        return linked_subscription_;
    }
    size_t subscribers_count() const override {
        return subscribers_.size();
    }

    void detach() {
        if (linked_subscription_)
            linked_subscription_.value().unsubscribe();
    }

    void next(T... value) {
        for (auto& subscriber : subscribers_) {
            subscriber.on_next(value...);
        }
    }

    void end() {
        for (auto& subscriber : subscribers_) {
            subscriber.on_end();
        }
    }

    void error(std::string descr) {
        for (auto& subscriber : subscribers_) {
            subscriber.on_error(descr);
        }
    }

    Observable& map(std::function<std::tuple<T...>(T...)> map_func) {
        auto proxy_observable = std::make_shared<Observable<T...>>();
        auto subscription = this->subscribe(
            [map_func, proxy_observable](T... args) {
            auto res = map_func(args...);

            std::apply(&Observable::next, std::tuple_cat(make_tuple(proxy_observable.get()), res));
        }
        );
        proxy_observable->set_linked_info(subscription);
        return *proxy_observable;
    }

    Observable& filter(std::function<bool(T...)> filter_func) {
        auto proxy_observable = std::make_shared<Observable<T...>>();
        auto subscription = this->subscribe(
            [filter_func, proxy_observable](T... args) {
            auto filter_res = filter_func(args...);
            if (filter_res)
                proxy_observable->next(args...);
        }
        );
        proxy_observable->set_linked_info(subscription);
        return *proxy_observable;
    }
    
    using I = std::tuple_element_t<0, std::tuple<T...>>;
    Observable<I>& reduce(std::function<I(I, I)> reduce_func, I init_val) {
        auto proxy_observable = std::make_shared<Observable<I>>();
        auto result = std::make_shared<I>(init_val);

        auto subscription = this->subscribe(
            [reduce_func, result](T... args) {

            std::apply([reduce_func, result](auto&&... values) {((
                *result = reduce_func(*result, values)
                ), ...); }, std::make_tuple(args...));
        },
        [proxy_observable, result]() {
            proxy_observable->next(*result);
            proxy_observable->end();
        }
        );
        proxy_observable->set_linked_info(subscription);
        return *proxy_observable;
    }

    void set_linked_info(Subscription subscription) {
        linked_subscription_ = std::move(subscription);
    }
private:
    std::shared_ptr<IExecutor> executor_{ nullptr };
    ExecutionPolicy execution_policy_ = ExecutionPolicy::NoExecutor;

    std::list<Subscriber<T...>> subscribers_;
    std::list<Subscription> subscriptions_;
    const std::string uuid_ = utils::get_uuid(); // For debug

    // linked - means that this observable is a proxy observable
    // made to allow subscribers to subscribe on map, filter or other function
    std::optional<Subscription> linked_subscription_;
};

}