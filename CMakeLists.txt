cmake_minimum_required(VERSION 3.10)

project(tirx VERSION 0.8.1 DESCRIPTION "tirx - tiny reactive programming library")

set(SOURCE
	src/Guid.cpp
	src/RunLoopExecutor.cpp
	src/SingleThreadExecutor.cpp
	src/ThreadPoolExecutor.cpp)

set(HEADER
	src/ExecutionPolicy.h
	src/Guid.h
	src/IExecutor.h
	src/IObservable.h
	src/Observable.h
	src/RunLoopExecutor.h
	src/SingleThreadExecutor.h
	src/Subscriber.h
	src/Subscription.h
	src/ThreadPoolExecutor.h)

add_library(tirx STATIC ${SOURCE} ${HEADER})

set_property(TARGET tirx PROPERTY CXX_STANDARD 17)