cmake_minimum_required(VERSION 3.10)

project(tirx)

set(SOURCE
    Guid.cpp
    RunLoopExecutor.cpp
    SingleThreadExecutor.cpp
    Subscription.cpp
    ThreadPoolExecutor.cpp)

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
    ThreadPoolExecutor.h)

add_library(tirx STATIC ${SOURCE} ${HEADER})

set_property(TARGET tirx PROPERTY CXX_STANDARD 17)
