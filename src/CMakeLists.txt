cmake_minimum_required(VERSION 3.10)

project(tiny_rx)

set(SOURCE
    guid.cpp
    run_loop_executor.cpp
    single_thread_executor.cpp
    subscription.cpp
    thread_pool_executor.cpp
)

set(HEADER
    execution_policy.h
    guid.h
    iexecutor.h
    iobservable.h
    log.h
    observable.h
    run_loop_executor.h
    single_thread_executor.h
    subscriber.h
    subscription.h
    tiny_rx.h
    thread_pool_executor.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${HEADER})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
