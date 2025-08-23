# tiny_rx reactive programming c++ library

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
- `on_error()` - will be executed if the observable reports that some error had happened
- `on_end()` - will be executed on source data end
- 
It is also possible to use object instead of lambdas to produce code with low coupling.  Just make a class implementing functions:
```c++
void on_next(ObservableType value) {
    // ...
}
void on_end() {
    // ...
}
void on_error(std::string description) {
    // ...
}
```
Pass an object of this class as an argument to `subscribe()` function.

## Use stream manipulating functions
Create observable source of ```int``` values:
```
auto source = tiny_rx::Observable<int>();
```
Map values by doubling each value, and subscribe:
```
auto subscription = source
    .map([](int v) { return v * 2; })
    .subscribe([](int value) { std::cout << value << "\n"; });
```

## Subscribe on another thread
Create ```SingleThreadExecutor```:
```
auto executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
```
Create observable source of ```int``` values:
```
auto source = tiny_rx::Observable<int>();
```
Subscribe with executor - subscriber functions will be executed on separate thread:
```
std::cout << "Current thread id = " << std::this_thread::get_id() << "\n";
auto subscription = source
    .subscribe_on(executor)
    .subscribe([](int value) {
        std::cout << value << " on thread " << std::this_thread::get_id() << "\n";
    });
```
Subscribe another subscriber on the observable source using the same thread:
```
std::vector<int> vec;
auto another_subscription = source
    .subscribe_on(executor)
    .subscribe([&vec](int value) {
        std::cout << "vector operation on thread " << std::this_thread::get_id() << "\n";
        vec.push_back(value);
    });
```
Different subscribers using the same executor will be sharing it's resources, in this particular case they will be executed sequentially in one thread.

## Use ```filter``` in one thread and subscribe on thread pool
You can even use different executors for stream functions and subscribers:
Create executors:
```
auto executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
auto pool_executor = std::make_shared<tiny_rx::ThreadPoolExecutor>(6);
```
Create observable, set ```SingleThreadExecutor``` for ```filter```, and ```ThreadPoolExecutor``` for subscriber:
```
auto source = tiny_rx::Observable<int>();
std::cout << "Current thread id = " << std::this_thread::get_id() << "\n";
auto subscription = source
    .subscribe_on(executor)
    .filter([](int v) {
        std::cout << "filter operation on thread " << std::this_thread::get_id() << "\n";
        return v % 2 != 0;
    } )
    .subscribe_on(pool_executor)
    .subscribe([](int value) {
        std::cout << value << " on thread " << std::this_thread::get_id() << "\n";
    });
```
Observe on vector:
```
const auto values = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
for (auto v : values) {
    source.next(v);
}
source.end();
// ...
```

Some other examples can be found in tests, which are more like snippets than usual unit-tests.
