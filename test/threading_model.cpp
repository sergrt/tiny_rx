#include <tiny_rx.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

TEST(TinyRxThreads, Single_Thread_Rvalue)
{
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::thread::id id_one{};

    std::atomic<int> latch = 0;

    auto source = tiny_rx::Observable<int>();
    auto subscription = source
        .subscribe_on(std::make_shared<tiny_rx::SingleThreadExecutor>())
        .subscribe([&id_one, &latch](int value) {
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            if (id_one == std::thread::id()) {
                id_one = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_one, std::this_thread::get_id());
            }
            if (value == 4)
                ++latch;
        });

    const auto values = std::vector<int>{ 1, 2, 3, 4 };

    std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
    for (auto v : values) {
        source.next(v);
    }
    source.end();

    // Wait for threads
    while (latch != 1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    EXPECT_NE(id_one, main_thread_id);
}

TEST(TinyRxThreads, Thread_Pool) {
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::vector<std::thread::id> ids;

    std::atomic<int> latch = 0;
    std::mutex ids_mutex;

    auto source = tiny_rx::Observable<int>();
    auto subscription = source
        .subscribe_on(std::make_shared<tiny_rx::ThreadPoolExecutor>(4))
        .subscribe([&ids, &ids_mutex, &latch](int value) {
            {
                std::lock_guard lock(ids_mutex);
                std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
                ids.push_back(std::this_thread::get_id());
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
            ++latch;
        });

    const auto values = std::vector<int>{ 1, 2, 3, 4 };

    std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
    for (auto v : values) {
        source.next(v);
    }
    source.end();

    // Wait for threads
    while (latch != 4) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ASSERT_EQ(4, ids.size());
    std::sort(ids.begin(), ids.end());
    for (size_t i = 1; i < ids.size(); ++i) {
        EXPECT_NE(ids[i], ids[i - 1]);
        EXPECT_NE(main_thread_id, ids[i]);
    }
}

TEST(TinyRxThreads, Single_Thread_OnNext_Two_Subscriptions) {
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::thread::id id_one{};
    std::thread::id id_two{};

    auto out_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
    std::mutex out_mutex;
    std::atomic<int> latch = 0;

    auto source = tiny_rx::Observable<int>();
    auto subscription_one = source
        .subscribe_on(out_executor)
        .subscribe([&out_mutex, &id_one, &latch](int value) {
            std::lock_guard lock(out_mutex);
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            if (id_one == std::thread::id()) {
                id_one = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_one, std::this_thread::get_id());
            }
            if (value == 4)
                ++latch;
        });

    auto subscription_two = source
        .subscribe_on(out_executor)
        .subscribe([&out_mutex, &id_two, &latch](int value) {
            std::lock_guard lock(out_mutex);
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            if (id_two == std::thread::id()) {
                id_two = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_two, std::this_thread::get_id());
            }
            if (value == 4)
                ++latch;
        });

    const auto values = std::vector<int>{ 1, 2, 3, 4 };

    std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
    for (auto v : values) {
        source.next(v);
    }
    source.end();

    // Wait for threads
    while (latch != 2) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }    

    EXPECT_EQ(id_one, id_two);
    EXPECT_NE(id_one, main_thread_id);
}

TEST(TinyRxThreads, Single_Thread_Map_And_OnNext) {
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::thread::id id_one{};
    std::thread::id id_two{};

    auto out_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
    std::atomic<int> latch = 0;

    auto source = tiny_rx::Observable<int>();
    auto subscription = source
        .subscribe_on(out_executor)
        .map([&id_one](int value) {
            if (id_one == std::thread::id()) {
                id_one = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_one, std::this_thread::get_id());
            }
            return value * 2;
        })
        .subscribe([&id_two, &latch](int value) {
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            if (id_two == std::thread::id()) {
                id_two = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_two, std::this_thread::get_id());
            }
            if (value == 8)
                ++latch;
        });


    const auto values = std::vector<int>{ 1, 2, 3, 4 };

    std::cout << "[" << std::this_thread::get_id() << "] Observable works here" << "\n";
    for (auto v : values) {
        source.next(v);
    }
    source.end();

    // Wait for threads
    while (latch != 1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    EXPECT_EQ(id_one, id_two);
    EXPECT_NE(id_one, main_thread_id);
}

TEST(TinyRxThreads, Single_Thread_Map_And_OnNext_Another_Subscription_In_Main) {
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::thread::id id_one{};
    std::thread::id id_two{};
    std::mutex out_mutex;

    auto out_executor = std::make_shared<tiny_rx::SingleThreadExecutor>();
    std::atomic<int> latch = 0;

    auto source = tiny_rx::Observable<int>();
    auto subscription_one = source
        .subscribe_on(out_executor)
        .map([&id_one](int value) {
            if (id_one == std::thread::id()) {
                id_one = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_one, std::this_thread::get_id());
            }
            return value * 2;
        })
        .subscribe([&out_mutex, &id_two, &latch](int value) {
            std::lock_guard lock(out_mutex);
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            if (id_two == std::thread::id()) {
                id_two = std::this_thread::get_id();
            } else {
                EXPECT_EQ(id_two, std::this_thread::get_id());
            }
            if (value == 8)
                ++latch;
        });

    auto subscription_two = source
        .subscribe([&out_mutex, &latch, main_thread_id](int value) {
            std::lock_guard lock(out_mutex);
            std::cout << "[" << std::this_thread::get_id() << "] " << value << "\n";
            EXPECT_EQ(main_thread_id, std::this_thread::get_id());

            if (value == 4)
                ++latch;
        });

    const auto values = std::vector<int>{ 1, 2, 3, 4 };

    std::cout << "[" << main_thread_id << "] Observable works here" << "\n";
    for (auto v : values) {
        source.next(v);
    }
    source.end();

    // Wait for threads
    while (latch != 2) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    EXPECT_EQ(id_one, id_two);
    EXPECT_NE(id_one, main_thread_id);
}
