#include "tiny_rx.h"

#include <gtest/gtest.h>

namespace {
auto upper = [](std::string s) {
    for (auto& c : s) {
        c = static_cast<std::remove_reference_t<decltype(c)>>(toupper(c));
    }
    return s;
};
}

TEST(Observable_Stream_Functions, Check_Map) {
    tiny_rx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon{ 1, 4, 9, 16, 25, 36, 49, 64 };
    std::vector<int> results;

    auto subscription = observable.map([](int v) { return v * v; }).subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon, results);
}

TEST(Observable_Stream_Functions, Check_Map_Different_Types) {
    tiny_rx::Observable<int, std::string> observable;

    const std::vector<int> int_values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<std::string> string_values{ "a" , "b", "c", "d", "e", "f", "g", "h" };

    const std::vector<int> int_etalon{ 1, 4, 9, 16, 25, 36, 49, 64 };
    const std::vector<std::string> string_etalon{ "A" , "B", "C", "D", "E", "F", "G", "H" };

    std::vector<int> int_results;
    std::vector<std::string> string_results;

    auto subscription = observable.map([](int v, std::string s) {
        return std::make_tuple(v * v, upper(s));
    }).subscribe([&int_results, &string_results](int v, std::string s) {
        int_results.push_back(v);
        string_results.push_back(s);
    });

    for (size_t i = 0; i < int_values.size(); ++i) {
        observable.next(int_values[i], string_values[i]);
    }

    EXPECT_EQ(int_etalon, int_results);
    EXPECT_EQ(string_etalon, string_results);
}

TEST(Observable_Stream_Functions, Check_Filter) {
    tiny_rx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<int> etalon{ 1, 3, 5, 7 };
    std::vector<int> results;

    auto subscription = observable.filter([](int v) {
        return v % 2 != 0;
    }).subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(etalon, results);
}

TEST(Observable_Stream_Functions, Check_Filter_Different_types) {
    tiny_rx::Observable<int, std::string> observable;

    const std::vector<int> int_values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    const std::vector<std::string> string_values{ "A" , "b", "C", "d", "E", "f", "G", "H" };

    const std::vector<int> int_etalon{ 3, 5 };
    const std::vector<std::string> string_etalon{ "C" , "E" };

    std::vector<int> int_results;
    std::vector<std::string> string_results;

    auto subscription = observable.filter([](int v, std::string s) {
        return (v > 1 && v < 7) && upper(s) == s;
    }).subscribe([&int_results, &string_results](int v, std::string s) {
        int_results.push_back(v);
        string_results.push_back(s);
    });

    for (size_t i = 0; i < int_values.size(); ++i) {
        observable.next(int_values[i], string_values[i]);
    }

    EXPECT_EQ(int_etalon, int_results);
    EXPECT_EQ(string_etalon, string_results);
}

TEST(Observable_Stream_Functions, Check_Reduce) {
    tiny_rx::Observable<int> int_observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    constexpr int etalon = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8;
    int result = 0;

    auto subscription = int_observable.reduce([](int v, int buf) {
        return buf + v;
    }, 0).subscribe([&result](int v) {
        result = v;
    });

    for (const auto v : values) {
        int_observable.next(v);
    }
    int_observable.end();

    EXPECT_EQ(etalon, result);
}

TEST(Observable_Stream_Functions, Check_Reduce_Many_Values) {
    tiny_rx::Observable<int, int> observable;

    const std::vector<int> first_values{ 1, 2, 3, 4};
    const std::vector<int> second_values{ 10, 20, 30, 40 };

    constexpr int etalon = 1 * 1*10 * 2*20 * 3*30 * 4*40;
    int result = 0;

    auto subscription = observable.reduce([&result](int v, int buf) {
        return buf * v;
    }, 1).subscribe([&result](int v) {
        result = v;
    });

    for (size_t i = 0; i < first_values.size(); ++i) {
        observable.next(first_values[i], second_values[i]);
    }
    observable.end();

    EXPECT_EQ(etalon, result);
}
