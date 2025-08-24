#include "tiny_rx.h"

#include <gtest/gtest.h>

#include <thread>

TEST(Observable_Sources, Vector_Source) {
    tiny_rx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> results;

    auto subscription = observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    std::thread thread([&observable, &values] {
        for (const auto v : values) {
            observable.next(v);;
        }
        observable.end();
    });
    thread.join();

    EXPECT_EQ(values, results);
}
