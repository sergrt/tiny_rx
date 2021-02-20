#include "gtest/gtest.h"

#include "tirx.h"

TEST(Observable_Complex_Subscription, Check_Map_Filter) {
    tirx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon{ 18, 21, 24 };
    std::vector<int> results;

    auto subscription = observable.map([](int v) {
        return v * 3;
    }).filter([](int v) {
        return v > 15;
    }).subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon, results);
}

TEST(Observable_Complex_Subscription, Check_Filter_Map) {
    tirx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon{ 3, 9, 15, 21 };
    std::vector<int> results;

    auto subscription = observable.filter([](int v) {
        return v % 2 != 0;
    }).map([](int v) {
        return v * 3;
    }).subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon, results);
}

TEST(Observable_Complex_Subscription, Check_Map_Subscribe_Unsubscribe_Map_Removed) {
    tirx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon_map{ 3, 6, 9, 12, 15, 18, 21, 24 };
    std::vector<int> results;

    auto subscription_with_map = observable.map([](int v) {
        return v * 3;
    }).subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon_map, results);

    subscription_with_map.unsubscribe();

    results.clear();
    auto subscription_without_map = observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(values, results);

}

TEST(Observable_Complex_Subscription, Check_Map_Subscribe_Unsubscribe_Map_Not_Removed) {
    tirx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon_map{ 3, 6, 9, 12, 15, 18, 21, 24 };
    std::vector<int> results;

    auto& map_observable = observable.map([](int v) {
        return v * 3;
    });

    auto subscription_with_map = map_observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    // Add dummy subscription to prevent map linked subscription to be deleted
    map_observable.subscribe([](int) {});

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon_map, results);

    subscription_with_map.unsubscribe();

    results.clear();
    // Access violation if map linked subscription deleted
    auto subscription_with_map_again = map_observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon_map, results);
}