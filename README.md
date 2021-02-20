**tirx (tiny-rx)** is a compact open-source reactive programming library with simple and comprehensible threading model.
It implements following reactive programming entities:
- **observable source** of any type (and of any number) of values
- **subscriber** - lambda function or object with simple interface
- **subscription** allows to unsubscribe from sources
- **executors** to process subscriptions in multi-threaded environment
- **stream manipulating functions** like ```map```, ```filter```, ```reduce```. And it's very easy to add user-defined functions.

Interface is quite similar to other reactive programming libraries. Some examples:

### 1. Simple subscription example
Create observable source of ```int``` values:
```
auto source = tirx::Observable<int>();
```
Subscribe on it:
```
auto subscription = source.subscribe([](int value) {
    std::cout << value << "\n";
});
```
Push values to subscribers when (and where) it's needed:
```
const auto values = std::vector<int>{ 1, 2, 3, 4 };
for (auto v : values) {
    source.next(v);
}
```
Unsubscribe:
```
subscription.unsubscribe();
```

### 2. Use stream manipulating functions
Create observable source of ```int``` values:
```
auto source = tirx::Observable<int>();
```
Map values by doubling each value, and subscribe:
```
auto subscription = source
    .map([](int v) { return v * 2; })
    .subscribe([](int value) { std::cout << value << "\n"; });
```

### 3. Subscribe on another thread
Create ```SingleThreadExecutor```:
```
auto executor = std::make_shared<tirx::SingleThreadExecutor>();
```
Create observable source of ```int``` values:
```
auto source = tirx::Observable<int>();
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

### 4. Use ```filter``` in one thread and subscribe on thread pool
You can even use different executors for stream functions and subscribers:
Create executors:
```
auto executor = std::make_shared<tirx::SingleThreadExecutor>();
auto pool_executor = std::make_shared<tirx::ThreadPoolExecutor>(6);
```
Create observable, set ```SingleThreadExecutor``` for ```filter```, and ```ThreadPoolExecutor``` for subscriber:
```
auto source = tirx::Observable<int>();
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
// ...
```

Some other examples can be found in tests, which are more like snippets than usual unit-tests.