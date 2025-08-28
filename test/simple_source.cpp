#include "tiny_rx.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

TEST(Observable_Source_Int, Check_Next) {
    tiny_rx::Observable<int> observable;

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> results;
    auto subscription = observable.subscribe([&results](int v) {
        results.push_back(v);
    });

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(values, results);
}

TEST(Observable_Source_Struct, Check_Next) {
    class SimpleClass {
    public:
        SimpleClass(int x, int y) : x_{ x }, y_{ std::make_shared<int>(y) } {
        }
        bool operator==(const SimpleClass& other) const {
            return x_ == other.x_ && *y_ == *other.y_;
        }
    private:
        int x_ = 10;
        std::shared_ptr<int> y_ = std::make_shared<int>(20);
    };

    tiny_rx::Observable<SimpleClass> observable;

    const std::vector<SimpleClass> values{ {1, 2}, {3, 4}, {5, 6} };
    std::vector<SimpleClass> results;
    auto subscription = observable.subscribe([&results](SimpleClass c) {
        results.push_back(c);
    });

    for (const auto& c : values) {
        observable.next(c);
    }

    EXPECT_EQ(values, results);
}

TEST(Observable_Source_Int_Str, Check_Multiple_Values_Observable) {
    auto source = tiny_rx::Observable<int, std::string>();

    auto subscription = source
        .subscribe([](const int& value, const std::string& str) {
            std::cout << "{ " << value << ", " << str << " }\n";
        });

    const auto values = std::map<int, std::string>{ 
        { 1, "A" },
        { 2, "B" }, 
        { 3, "C" },
        { 4, "D" }
    };

    for (const auto& [i, s] : values) {
        source.next(i, s);
    }
    source.end();
}

TEST(Observable_Source_Int, Check_Object_Subscriber) {

    class SubscriberObject {
    public:
        void on_next(int v) {
            result_.push_back(v);
        }
        void on_end() {
            
        }
        void on_error(const std::string& description) {
            
        }
        auto get_result() const {
            return result_;
        }
    private:
        std::vector<int> result_;
    };

    tiny_rx::Observable<int> observable;
    auto subscriber_object = std::make_shared<SubscriberObject>();

    const std::vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> results;
    auto subscription = observable.subscribe(subscriber_object);

    for (const auto v : values) {
        observable.next(v);
    }

    EXPECT_EQ(values, subscriber_object->get_result());
}

TEST(Observable_Source_Int, Check_Next_OnEnd_OnError) {
    auto source = tiny_rx::Observable<int>();

    const auto values = std::vector<int>{ 1, 2, 3, 4 };
    std::vector<int> collected_values;
    int on_end_call_times = 0;
    const auto errors = std::vector<std::string>{ std::string("err_2"), std::string("err_4") };
    std::vector<std::string> collected_errors;

    auto subscription = source.subscribe([&collected_values](int value) {
        collected_values.push_back(value);
    },
    [&on_end_call_times]() {
        ++on_end_call_times;
    },
    [&collected_errors](const std::string& descr) {
        collected_errors.push_back(descr);
    });
    
    for (auto v : values) {
        source.next(v);
        if (v % 2 == 0)
            source.error("err_" + std::to_string(v));
    }
    source.end();

    EXPECT_THAT(values, testing::ElementsAreArray(collected_values));
    EXPECT_EQ(on_end_call_times, 1);
    EXPECT_THAT(errors, testing::ElementsAreArray(collected_errors));

    subscription.unsubscribe();
}

TEST(Observable_Source_Int, Check_Object_Subscriber_Two_Values) {

    class SubscriberObject {
    public:
        void on_next(int v, std::string s) {
            result_.push_back(s + std::to_string(v));
        }
        void on_end() {

        }
        void on_error(const std::string& description) {

        }
        auto get_result() const {
            return result_;
        }
    private:
        std::vector<std::string> result_;
    };

    tiny_rx::Observable<int, std::string> observable;
    auto subscriber_object = std::make_shared<SubscriberObject>();

    const std::vector<int> int_values{ 1, 2, 3, 4, 5, 6 };
    const std::vector<std::string> str_values{ "A", "B", "C", "D", "E", "F" };
    const std::vector<std::string> etalon{ "A1", "B2", "C3", "D4", "E5", "F6" };

    std::vector<int> results;
    auto subscription = observable.subscribe(subscriber_object);

    for (size_t i = 0; i < int_values.size(); ++i) {
        observable.next(int_values[i], str_values[i]);
    }

    EXPECT_EQ(etalon, subscriber_object->get_result());
}

TEST(Observable_Source_Int, Check_Unsubscribe) {
    auto source = tiny_rx::Observable<int>();

    const auto values = std::vector<int>{ 1, 2, 3, 4 };
    const auto etalon_values = std::vector<int>{ 1, 2 };
    std::vector<int> collected_values;
    int on_end_call_times = 0;
    const auto errors = std::vector<std::string>{ std::string("err_2") };
    std::vector<std::string> collected_errors;

    auto subscription = source.subscribe([&collected_values](int value) {
        collected_values.push_back(value);
    },
    [&on_end_call_times]() {
        ++on_end_call_times;
    },
    [&collected_errors](const std::string& descr) {
        collected_errors.push_back(descr);
    });
    
    for (auto v : values) {
        if (v > 2) {
            source.end();
            subscription.unsubscribe();
        }
        source.next(v);
        if (v % 2 == 0)
            source.error("err_" + std::to_string(v));
        
    }
    source.end();

    EXPECT_THAT(etalon_values, testing::ElementsAreArray(collected_values));
    EXPECT_EQ(on_end_call_times, 1);
    EXPECT_THAT(errors, testing::ElementsAreArray(collected_errors));
}
