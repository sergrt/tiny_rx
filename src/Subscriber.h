#pragma once

#include "ExecutionPolicy.h"
#include "IExecutor.h"
#include "Guid.h"
#include "Log.h"

#include <functional>
#include <iostream>

namespace tirx {

template<typename ...T>
class Subscriber {
public:
    Subscriber() : uuid_{ utils::get_uuid() } {
        utils::log(utils::LogSeverity::Trace, "Constructing subscriber " + uuid_);
    }

    ~Subscriber() {
        utils::log(utils::LogSeverity::Trace, "Destructing subscriber " + uuid_);
    }

    Subscriber(const Subscriber& other) = delete;

    Subscriber& operator=(const Subscriber& other) = delete;

    Subscriber(Subscriber&& other) noexcept
        : Subscriber() {
        utils::log(utils::LogSeverity::Trace, "Move constructing subscriber " + other.uuid_);
        other.swap(*this);
    }

    // We want no copy assignment, so use explicit && here
    Subscriber& operator=(Subscriber&& other) noexcept {
        utils::log(utils::LogSeverity::Trace, "Move operator= subscriber " + other.uuid_);
        other.swap(this);
        return *this;
    }

    /*
    Subscriber(const Subscriber& other) {
        utils::log(utils::LogSeverity::Trace, "Copying subscriber " + other.uuid_);
        uuid_ = other.uuid_;
        func_ = other.func_;
        end_func_ = other.end_func_;
        error_func_ = other.error_func_;
        execution_policy_ = other.execution_policy_;
        executor_ = other.executor_;
    }
    */
    [[nodiscard]] std::string get_uuid() const {
        return uuid_;
    }

    void set_function(std::function<void(T...)> func) {
        func_ = std::move(func);
    }

    void set_on_end(std::function<void()> func) {
        end_func_ = std::move(func);
    }

    void set_on_error(std::function<void(std::string)> func) {
        error_func_ = std::move(func);
    }

    void set_execution_policy(ExecutionPolicy execution_policy) {
        execution_policy_ = execution_policy;
    }

    void set_executor(std::shared_ptr<IExecutor> executor) {
        executor_ = std::move(executor);
    }

    void on_next(T ...values) {
        if (execution_policy_ == ExecutionPolicy::NoExecutor) {
            func_(values...);
        } else {
            executor_->add_task(std::bind(func_, values...));
        }
    }

    void on_end() {
        if (execution_policy_ == ExecutionPolicy::NoExecutor) {
            end_func_();
        } else {
            executor_->add_task(end_func_);
        }
    }

    void on_error(std::string descr) {
        if (execution_policy_ == ExecutionPolicy::NoExecutor) {
            error_func_(descr);
        } else {
            executor_->add_task(std::bind(error_func_, descr));
        }
    }

private:

    void swap(Subscriber& other) noexcept {
        std::swap(uuid_, other.uuid_);
        std::swap(func_, other.func_);
        std::swap(end_func_, other.end_func_);
        std::swap(error_func_, other.error_func_);
        std::swap(execution_policy_, other.execution_policy_);
        std::swap(executor_, other.executor_);
    }

    std::string uuid_;
    std::function<void(T...)> func_;
    std::function<void()> end_func_;
    std::function<void(std::string)> error_func_;
    ExecutionPolicy execution_policy_ = ExecutionPolicy::NoExecutor;
    std::shared_ptr<IExecutor> executor_;
};

}