# tiny_rx reactive programming c++ library

- [tiny\_rx reactive programming c++ library](#tiny_rx-reactive-programming-c-library)
- [Overview](#overview)
  - [Features](#features)
- [Usage](#usage)
  - [Quick start example](#quick-start-example)
  - [Use stream manipulating functions](#use-stream-manipulating-functions)
    - [Using `map()` function](#using-map-function)
    - [Using `filter()` function](#using-filter-function)
    - [Using `reduce()` function](#using-reduce-function)
    - [Combination of functions](#combination-of-functions)
    - [Subscriptions to intermediate result](#subscriptions-to-intermediate-result)
- [Multithreading](#multithreading)
  - [Overview](#overview-1)
  - [Same thread as values source](#same-thread-as-values-source)
  - [Work in another thread](#work-in-another-thread)
  - [Work in thread pool](#work-in-thread-pool)
  - [Run loop executor](#run-loop-executor)
  - [`map()`, `filter()` and `reduce()` on different threads](#map-filter-and-reduce-on-different-threads)


**tiny-rx** is a compact open-source reactive programming C++ library with simple and comprehensible threading model.

# Overview
Reactive programming is a programming paradigm centered around asynchronous data streams and the propagation of changes. It provides a declarative approach to handle events and data that change over time, making code more readable, maintainable, and scalable, especially in applications that require responsiveness and real-time data processing.

## Features
**tiny_rx** implements following reactive programming entities:
- **observable source** of any type (and number) of values. This is what is called *observable* or *subject* in different sources
- **subscriber** is an object with callables (lambda function or object with simple interface), implementing code execution on new value arrival, on error and on data stream ending
- **subscription** abstraction to manipulate stream data subscription, e.g. unsubscribe or resubscribe to stream
- **executors** to process subscriptions in multi-threaded environment
- **stream manipulating functions** like ```map```, ```filter```, ```reduce```. And it's very easy to add user-defined functions.

Interface is based on some well-known reactive programming librarises, so it is easy to migrate and it has very gentle learning curve. 

# Usage

## Quick start example
Let's say we have some source of integer values, and want to print them out to console.
First, create ad **observable** source of ```int``` values:
```c++
auto source = tiny_rx::Observable<int>();
```
After that we can subscribe on this observable (`source`) and get the subscription:
```c++
auto subscription = source.subscribe([](int value) {
    std::cout << value << "\n";
});
```
The lambda function (1st parameter of the `subscribe()` function) is `on_next()` callable. It will be executed each time new value arrives from the `source`.
Let's push some values (from vector) to the `source`. Each of this value will be printed to the console:
```c++
const auto values = std::vector<int>{ 1, 2, 3, 4 };
for (auto v : values) {
    source.next(v);
}
source.end();
```
If we need to unsubscribe from `source` we can call `unsubscribe()` method of the `subscription`:
```c++
subscription.unsubscribe();
```
Note that call to `source.end();` does nothing for now. When we had subscribed to the `source` we specified ony one callable, the first one. This is `on_next` function. You can specify up to three callables:
- `on_next()` - will be executed on new value arrval
- `on_end()` - will be executed on source data end
- `on_error()` - will be executed if the observable reports that some error had happened


It is also possible to use object instead of lambdas to produce code with low coupling.  Just make a class implementing functions:
```c++
void on_next(const ObservableType& value) {
    // ...
}
void on_end() {
    // ...
}
void on_error(const std::string& description) {
    // ...
}
```
Pass an object of this class as an argument to `subscribe()` function.

## Use stream manipulating functions
Reactive pprogramming paradigm usually implies providing three functions for stream manipulation:
- `map()`

    _Purpose_: Transforms each element of a collection and returns a new collection containing the results of the transformation.

    _Mechanism_: It applies a given function to every element in the input collection, creating a new collection of the same length with the transformed elements.

    _Example_: Doubling each number in a list: `[1, 2, 3]` becomes `[2, 4, 6]`.

- `filter()`

    _Purpose_: Selects elements from a collection based on a specified condition and returns a new collection containing only the elements that satisfy the condition.

    _Mechanism_: It applies a given predicate (a function that returns a boolean) to each element. Only elements for which the predicate returns true are included in the new collection.

    _Example_: Filtering out even numbers from a list: `[1, 2, 3, 4]` becomes `[2, 4]`.

- `reduce()`

    _Purpose_: Aggregates all elements of a collection into a single value.

    _Mechanism_: It applies a "reducer" function cumulatively to each element, from left to right, to reduce the collection to a single output value. It often takes an initial accumulator value.

    _Example_: Summing all numbers in a list: `[1, 2, 3]` becomes `6`.

Some examples below.

All the examples uses observable source of `int` values, as before:
```c++
auto source = tiny_rx::Observable<int>();
const auto values = std::vector<int>{ 1, 2, 3, 4 };
// ... subscription
for (auto v : values) {
    source.next(v);
}
source.end();
```

### Using `map()` function
Let's double each value
Map values by doubling each value, and subscribe:
```c++
auto subscription = source
    .map([](int v) { return v * 2; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

Output:
```
2
4
6
8
```

### Using `filter()` function
Print only even numbers:
```c++
auto subscription = source
    .filter([](int x) { return x % 2 == 0; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

Output:
```
2
4
```

### Using `reduce()` function
Make a sum of every stream value. `reduce()` takes 2 parameters:
- callable (lambda in our case) taking one extra parameter (accumulation value)
- initial value. `0` in our case

```c++
auto subscription = source
    .reduce([](int x, int accum) { return x + accum; }, 0)
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

Output:
```
10
```
### Combination of functions
It is possible to make a combinaton of these functions.

Let's filter only even numbers, multiply them by `4`, and filter again only numbers greater than `10`:

```c++
auto subscription = source
    .filter([](int x) { return x % 2 == 0; })
    .map([](int x) { return x * 4; })
    .filter([](int x) { return x > 10; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

Output:
```
16
```

### Subscriptions to intermediate result
Each of the functions `map()`, `filter()`, `reduce()` return and observable, so it is possible to have severals subscriptions on the observable in different places of the chain. For example, let's take previous code and add another subscription before second `filter()`, e.g. print every x4 value as long as the result `16`:

```c++
auto& x4_observable = source
    .filter([](int x) { return x % 2 == 0; })
    .map([](int x) { return x * 4; });

auto x4_subscription = x4_observable
    .subscribe([](int value) {
        std::cout << "x4 value = " << value << "\n";
    });

auto subscription = x4_observable
    .filter([](int x) { return x > 10; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

Output:
```
x4 value = 8
x4 value = 16
16
```
**Lifetime warning**: `Observable<>` returned as a result to call of the `map()`, `filter()` and `reduce()` functions is not intended to be stored. By design it turns to unspecified state as soon as last subscriber is unsubscribed. For example, this code leads to errors:
```c++
auto& x4_observable = source
    .filter([](int x) { return x % 2 == 0; })
    .map([](int x) { return x * 4; });

auto x4_subscription = x4_observable
    .subscribe([](int value) {
        std::cout << "x4 value = " << value << "\n";
    });

// After this call `x4_observable` should not be used anymore
x4_subscription.unsubscribe();

// Error
auto subscription = x4_observable
    .filter([](int x) { return x > 10; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });
```

So be sure to make all subscriptions before unsubscribe the only subscriber. This code works fine:
```c++
auto& x4_observable = source
    .filter([](int x) { return x % 2 == 0; })
    .map([](int x) { return x * 4; });

auto x4_subscription = x4_observable
    .subscribe([](int value) {
        std::cout << "x4 value = " << value << "\n";
    });

auto subscription = x4_observable
    .filter([](int x) { return x > 10; })
    .subscribe([](int value) {
        std::cout << value << "\n";
    });

// Note: there are 2 subscribers to `x4_observable`.
// Unsubscribe `x4_subscription` won't affect `subscription`

x4_subscription.unsubscribe();

// Filtered values will be processed as intended
```

# Multithreading
More interesting and useful usages of the `tiny_rx` is about multithreaded environment.

## Overview
The library provides 4 types of connections:
- Same thread as values sources
- Another thread
- Thread pool with tweakable capacity
- Run loop (placeholder)

There's a concept of `Executor` is introduced in `tiny_rx`. To support different threading model caller should specify an object implemening `IExecutor` interface. Implementation classes for different connection types:
- `SingleThreadExecutor` for one thread, different from the caller
- `ThreadPoolExecutor` for thread pool with tweakable capacity
- `RunLoopExecutor` is a placeholder for injecting into framework's run loop

## Same thread as values source
All the above examples work in the same thread as the `Observable`. No need to do something for this behavior

## Work in another thread
Let's process our values in different thread. Create `SingleThreadExecutor` and pass it as a shared pointer by calling `subscribe_on()`:
```c++
auto single_thread_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
auto observable = source
    .subscribe_on(single_thread_executor)
    .subscribe([](int value) {
        std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
    });

const auto values = std::vector<int>{ 1, 2, 3, 4 };

std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
for (auto v : values) {
    source.next(v);
    
}
source.end();
```
Output:
```
[24560] Observable works here
[22308] 1
[22308] 2
[22308] 3
[22308] 4
```
`SingleThreadExecutor` can be reused for different subscriptions, all the tasks will be executed in the same thread.

## Work in thread pool
To use thread pool, use another executor named `ThreadPoolExecutor`. Let's rewrite last example with thread pool with the capacity of 3:
```c++
auto thread_pool_executor = std::make_shared<tiny_rx::ThreadPoolExecutor>(3);
std::mutex out_mutex;
auto observable = source
    .subscribe_on(thread_pool_executor)
    .subscribe([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
    });

const auto values = std::vector<int>{ 1, 2, 3, 4 };

std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
for (auto v : values) {
    source.next(v);
    
}
source.end();
```
Note mutex to avoid garbage in console output.

Output:
```
[27896] Observable works here
[23300] 1
[34932] 2
[13584] 3
[23300] 4
```
## Run loop executor
Sometimes there's a need to inject processing into framework code:
- Qt event loop
- NoesisGUI drawing routine
- ... etc.

There's no general approaches to this implementation, so `tiny_rx` provides a class with the implementation of functions:
- `add_task()` function to add a task
- `dispatch()` to execute stored task

It's up to the framework where and when to execute the `dispatch()`, it should be integrated at the right place

## `map()`, `filter()` and `reduce()` on different threads
Sometimes these functions are resource-heavy, so it is better to call them on different thread using `ThreadPoolExecutor`. Also it might be useful to execute all calculations on one separate thread with `SingleThreadExecutor`. Let's write the code, which:
- executes all `map()` functions on different thread
- executes all `filter()` functions in the thread pool
- executes `on_next()` on another thread

```c++
auto map_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
auto filter_executor = std::make_shared<tiny_rx::ThreadPoolExecutor>(3);
auto cout_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
std::mutex out_mutex;
auto observable_doubling = source
    .subscribe_on(map_executor)
    .map([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] [map thread] " << value << "\n";
        return value * 2;
    })
    .subscribe_on(filter_executor)
    .filter([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] [filter thread] " << value << "\n";
        return value % 2 == 0;
    })
    .subscribe_on(cout_executor)
    .subscribe([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] double " << value << "\n";
    });

auto observable_tripling = source
    .subscribe_on(map_executor)
    .map([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] [map thread] " << value << "\n";
        return value * 3;
    })
    .subscribe_on(filter_executor)
    .filter([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] [filter thread] " << value << "\n";
        return value > 10;
    })
    .subscribe_on(cout_executor)
    .subscribe([&out_mutex](int value) {
        std::lock_guard lock(out_mutex);
        std::cout << "[" << std::this_thread::get_id() << "] triple " << value << "\n";
    });

const auto values = std::vector<int>{ 1, 2, 3, 4 };

std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
for (auto v : values) {
    source.next(v);
    
}
source.end();
```


Output:
```
[44012] Observable works here
[14032] [map thread] 1
[26296] [filter thread] 2
[14032] [map thread] 1
[9780] double 2
[26296] [filter thread] 3
[14032] [map thread] 2
[26296] [filter thread] 4
[14032] [map thread] 2
[9780] double 4
[14032] [map thread] 3
[26296] [filter thread] 6
[14032] [map thread] 3
[33408] [filter thread] 6
[14032] [map thread] 4
[26296] [filter thread] 9
[9780] double 6
[14032] [map thread] 4
[33408] [filter thread] 8
[26296] [filter thread] 12
[9780] double 8
[9780] triple 12
```

Note that all [map thread] (from both subscriptions) are reported as one thread, all values output are reported as another thread, and all [filter thread] use different threads (up to 3)


Some other examples can be found in tests, which are more like snippets than usual unit-tests.
