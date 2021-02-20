#include "gtest/gtest.h"

#include "tirx.h"

#include <thread>

TEST(Observable_Sources, Vector_Source) {
    tirx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> results;

    auto subscription = observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    std::thread thread([&observable, &values] {
        for (const auto v : values) {
            observable.next(v);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        observable.end();
    });
    thread.join();

    EXPECT_EQ(values, results);
}