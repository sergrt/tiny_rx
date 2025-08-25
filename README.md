**`tiny_rx`** is a compact reactive programming C++ library
- [`tiny_rx` description](#tiny_rx-description)
- [Overview](#overview)
  - [Features](#features)
- [Usage](#usage)
  - [Quick start example](#quick-start-example)
  - [Use stream manipulating functions](#use-stream-manipulating-functions)
    - [Using `map()`](#using-map)
    - [Using `filter()`](#using-filter)
    - [Using `reduce()`](#using-reduce)
    - [Combining functions](#combining-functions)
    - [Subscribing to intermediate result](#subscribing-to-intermediate-result)
- [Multithreading](#multithreading)
  - [Overview](#overview-1)
  - [Same thread as the value source](#same-thread-as-the-value-source)
  - [Work in another thread](#work-in-another-thread)
  - [Work in a thread pool](#work-in-a-thread-pool)
  - [Run loop executor](#run-loop-executor)
  - [`map()`, `filter()` and `reduce()` on different threads](#map-filter-and-reduce-on-different-threads)

# `tiny_rx` description

**`tiny_rx`** is a compact open-source reactive programming C++ library with a simple and comprehensible threading model.

# Overview
Reactive programming is a paradigm centered around asynchronous data streams and the propagation of changes. It provides a declarative approach to handle events and data that change over time, making code more readable, maintainable, and scalable, especially in applications that require responsiveness and real-time data processing.

Some frameworks have the luxury of built-in notification mechanisms, e.g. `Qt`’s signal/slot system. It is a handy tool for passing notifications, reacting to new values, and processing stream data.

The problems are:
- Not every framework provides such tools
- Some frameworks use additional tools. `Qt`, for example, relies on the `moc` compiler
- Frameworks that do provide such features are usually UI-oriented, making them unsuitable for backend or middleware code

Another issue is that such notifications typically target single events and are not well-suited for handling continuous data streams.

**`tiny_rx`** addresses all these issues:
- Easy-to-use notification/response mechanics
- No dependencies or extra compilation steps
- Suitable for any layer: UI (especially Immediate-mode UI libraries), backend, or middleware
- Provides tools for handling not only single events but also continuous streams
- Threading model is explicit, clear, and easy to control

## Features
- Functional-style stream manipulation: `map()`, `filter()`, `reduce()` etc.
- Clear multithreading support with different executors
- Simple and lightweight API
- No external dependencies

**`tiny_rx`** implements the following reactive programming entities:
- **observable source** of any type (and number) of values. This is what is commonly called *observable* or *subject* in different sources
- **subscriber** is an object with callables (a lambda function or an object with a simple interface) that execute code when a new value arrives, an error occurs, or the data stream ends
- **subscription** is an abstraction that manages stream data subscriptions, e.g., unsubscribe or resubscribe to a stream
- **executors** allow processing subscriptions in a multi-threaded environment
- **functional-style stream manipulation**: `map()`, `filter()`, `reduce()` functions

The interface is based on well-known reactive programming libraries, making it easy to migrate with a very gentle learning curve. 

# Usage

## Quick start example
Let's say we have a source of integer values and want to print them to the console
First, create an **observable** source of `int` values:
```c++
auto source = tiny_rx::Observable<int>();
```
Next, we can subscribe to this observable `source` and get a subscription:
```c++
auto subscription = source.subscribe([](int value) {
    std::cout << value << "\n";
});
```
The lambda function (the 1st parameter of the `subscribe()` function) is the `on_next()` callable. It will be executed each time a new value arrives from the source.

Now let's push some values (from a vector) to the source. Each of these values will be printed to the console:
```c++
const auto values = std::vector<int>{ 1, 2, 3, 4 };
for (auto v : values) {
    source.next(v);
}
source.end();
```
If we need to unsubscribe from the `source`, we can call the `unsubscribe()` method of the `subscription`:
```c++
subscription.unsubscribe();
```
Note that the call to `source.end()` currently does nothing. When we subscribed to the `source`, we specified only one callable - `on_next()`. You can specify up to three callables:
- `on_next()` - executed on new value arrival
- `on_end()` - executed when the source stream ends
- `on_error()` - executed if the observable reports an error

It is also possible to use an object instead of lambdas to produce code with lower coupling. Just create a class that implements these functions:
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
Pass an object of this class as an argument to the `subscribe()` function.

You can implement `on_next()` with the argument passed by reference or by value.

## Use stream manipulating functions
The reactive programming paradigm usually provides three core functions for stream manipulation:
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

Some examples are shown below.

All examples use an observable source of `int` values, as before:
```c++
auto source = tiny_rx::Observable<int>();
const auto values = std::vector<int>{ 1, 2, 3, 4 };
// ... subscription
for (auto v : values) {
    source.next(v);
}
source.end();
```

### Using `map()`
Let's double each value.
Map values by multiplying each by 2, and subscribe:
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

### Using `filter()`
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

### Using `reduce()`
Calculate the sum of all stream values.

`reduce()` takes 2 parameters:
- a callable (a lambda in our case) that takes the current value and the accumulated value
- the initial value (0 in our case)

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
### Combining functions
It is possible to combine these functions.

For example, let's filter even numbers, multiply them by 4, and then filter again to keep only numbers greater than 10:

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

### Subscribing to intermediate result
Each of the functions `map()`, `filter()`, and `reduce()` returns an observable, so you can have multiple subscriptions at different points in the chain.

For example, let's take the previous code and add another subscription before the second `filter()`, to print every x4 value as well as the final result `16`:

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
**Lifetime warning**: An `Observable<>` returned from `map()`, `filter()`, or `reduce()` is not intended to be stored. By design, it enters an unspecified state as soon as the last subscriber unsubscribes.

For example, this code will lead to errors:
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

Make sure to set up all subscriptions before unsubscribing the only subscriber. This code works fine:
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
One of the most powerful and practical aspects of **`tiny_rx`** is its support for multithreaded environments.

## Overview
The library provides 4 types of execution contexts:
- Same thread as the value source
- A dedicated thread
- A thread pool with configurable capacity
- Run loop (placeholder for integration into external frameworks)

The concept of an `Executor` is introduced in **`tiny_rx`**. To support different threading models, the caller specifies an object implementing the `IExecutor` interface. Implementation classes include:
- `SingleThreadExecutor` - runs tasks in a dedicated single thread, separate from the caller
- `ThreadPoolExecutor` - runs tasks in a thread pool with configurable capacity
- `RunLoopExecutor` - a placeholder designed for integration into a framework's run loop

## Same thread as the value source
All the examples above run in the same thread as the `Observable`. No additional setup is required.

## Work in another thread
To process values in a different thread, create a `SingleThreadExecutor` and pass it as a shared pointer using `subscribe_on()`:
```c++
auto single_thread_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
auto subscription = source
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
A `SingleThreadExecutor` can be reused for multiple subscriptions; all tasks will be executed in the same dedicated thread.

## Work in a thread pool
To use a thread pool, use the `ThreadPoolExecutor`. Let's rewrite the last example with a thread pool of capacity 3:
```c++
auto thread_pool_executor = std::make_shared<tiny_rx::ThreadPoolExecutor>(3);
std::mutex out_mutex;
auto subscription = source
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
Note: a mutex is required to avoid corrupted console output.

Output:
```
[27896] Observable works here
[23300] 1
[34932] 2
[13584] 3
[23300] 4
```
## Run loop executor
Sometimes there's a need to inject processing into a framework's event loop, such as:
- `Qt` event loop
- `NoesisGUI` rendering routine
- etc.

Since there is no universal solution, **`tiny_rx`** provides a class with two functions:
- `add_task()` - adds a task
- `dispatch()` - executes queued tasks

It is up to the framework to decide where and when to call `dispatch()`. This should be integrated at the appropriate place in the loop.

## `map()`, `filter()` and `reduce()` on different threads
Sometimes these functions are resource-heavy, and it's better to run them in separate threads using a `ThreadPoolExecutor`. Alternatively, all calculations can be executed on a single dedicated thread with a `SingleThreadExecutor`. For example, the following code:
- executes all `map()` functions in a separate thread
- executes all `filter()` functions in a thread pool
- executes `on_next()` in another thread

```c++
auto map_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
auto filter_executor = std::make_shared<tiny_rx::ThreadPoolExecutor>(3);
auto cout_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
std::mutex out_mutex;

auto subscription_doubling = source
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

auto subscription_tripling = source
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

Note: all `[map thread]` outputs (from both subscriptions) come from a single thread, all value outputs come from another thread, and `[filter thread]` outputs are distributed across different threads (up to 3).

Note: calls to `subscribe_on()` have transitive effect: all other subscriptions of the expression (till `;`) will use last supplied executor, e.g.:

```c++
auto subscription = source
    .subscribe_on(executor)  // Executor supplied
    .map([](int value) {
        // executed with `executor`
    })
    .subscribe([](int value) {
        // also executed with `executor`
    });
```

Additional examples can be found in the tests, which are written more like usage snippets than traditional unit tests.