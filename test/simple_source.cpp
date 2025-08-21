#include "tiny_rx.h"

#include <gtest/gtest.h>

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

TEST(Observable_Source_Int, Check_Object_Subscriber) {

    class SubscriberObject {
    public:
        void on_next(int v) {
            result_.push_back(v);
        }
        void on_end() {
            
        }
        void on_error(std::string description) {
            
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

TEST(Observable_Source_Int, Check_Object_Subscriber_Two_Values) {

    class SubscriberObject {
    public:
        void on_next(int v, std::string s) {
            result_.push_back(s + std::to_string(v));
        }
        void on_end() {

        }
        void on_error(std::string description) {

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
