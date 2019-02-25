#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/future_then.h>

#define USE_TIMINGS 0

inline std::string thread_id_string(const std::thread::id& threadId)
{
  std::stringstream ss;
  ss << threadId;
  return ss.str();
}

inline size_t string_counter(const std::string& s)
{
#if USE_TIMINGS
  std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
  return s.size();
}

std::size_t string_count_accumulator(const std::vector<std::string>& strings)
{
  std::vector<std::future<size_t>> calcs;
  for (auto&& s : strings) {
    calcs.push_back(std::async(std::launch::async,
                               [&s]() -> size_t { return string_counter(s); }));
  }
  std::size_t total = 0;
  for (auto&& fut : calcs) {
    total += fut.get();
  }
  return total;
}

TEST(TestFutureThen, AsyncContinuation)
{
  using namespace BABYLON;

  {
    auto task1
      = std::async([] { return thread_id_string(std::this_thread::get_id()); });

    auto task2 = then(task1, [](std::future<std::string>& f) {
      auto currentThreadId = thread_id_string(std::this_thread::get_id());
      EXPECT_NE(f.get(), currentThreadId);
      return currentThreadId;
    });

    auto task3 = then(task2, [](std::future<std::string>& f) {
      auto currentThreadId = thread_id_string(std::this_thread::get_id());
      EXPECT_NE(f.get(), currentThreadId);
      return currentThreadId;
    });

    task3.wait();
  }

  {
    auto task1 = std::async(std::launch::async, [] { return 1; });

    auto task2 = then(task1, std::launch::async,
                      [](std::future<int>& f) { return f.get() + 2; });

    auto task3 = then(task2, std::launch::async,
                      [](std::future<int>& f) { return f.get() + 3; });

    // Check result received from continuation in parent task
    EXPECT_EQ(task3.get(), 6);
  }

  {
#if USE_TIMINGS
    // Get Start Time
    std::chrono::system_clock::time_point start
      = std::chrono::system_clock::now();
#endif

    auto task1 = std::async(std::launch::async, [] {
      return string_count_accumulator({"this", "is", "task1"});
    });

    auto task2
      = then(task1, std::launch::async, [](std::future<std::size_t>& f) {
          return f.get()
                 + string_count_accumulator({"and", "this", "is", "task2"});
        });

#if USE_TIMINGS
    // Get End Time
    auto end = std::chrono::system_clock::now();

    auto diff
      = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
    std::cout << "Total Time Taken = " << diff << " milliseconds\n";
#endif

    // Check result received from continuation in parent task
    EXPECT_EQ(task2.get(), 25);
  }
}
