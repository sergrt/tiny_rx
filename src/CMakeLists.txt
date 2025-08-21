cmake_minimum_required(VERSION 3.10)

project(tiny_rx)

set(SOURCE
    Guid.cpp
    RunLoopExecutor.cpp
    SingleThreadExecutor.cpp
    Subscription.cpp
    ThreadPoolExecutor.cpp
)

set(HEADER
    ExecutionPolicy.h
    Guid.h
    IExecutor.h
    IObservable.h
    Log.h
    Observable.h
    RunLoopExecutor.h
    SingleThreadExecutor.h
    Subscriber.h
    Subscription.h
    tiny_rx.h
    ThreadPoolExecutor.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${HEADER})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
