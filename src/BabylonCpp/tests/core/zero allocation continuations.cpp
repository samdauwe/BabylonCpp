#include <gtest/gtest.h>

#include <babylon/core/continuations/zero allocation continuations.h>

static std::atomic<int> ctr = 0;
struct sctr {
  sctr()
  {
    ++ctr;
  }
  ~sctr()
  {
    --ctr;
  }
};

TEST(ZeroAllocationContinuations, DISABLED_Test1)
{
  using namespace BABYLON;

  std::mutex mtx;
  std::random_device rd;
  std::default_random_engine re(rd());

  const auto rndint = [&](int min, int max) {
    std::scoped_lock l{mtx};
    return std::uniform_int_distribution<int>{min, max - 1}(re);
  };

  const auto rndsleep = [&] {
    std::this_thread::sleep_for(std::chrono::nanoseconds{rndint(0, 100)});
  };

  for (int i = 0; i < 1000; ++i) {
    auto f = initiate(
               [&] {
                 rndsleep();
                 return 1;
               },
               [&] {
                 rndsleep();
                 return 2;
               },
               [&] {
                 rndsleep();
                 return 3;
               })
               .then(
                 [&](auto t) {
                   rndsleep();
                   auto [a, b, c] = t;
                   EXPECT_EQ(a + b + c, 1 + 2 + 3);
                   return 0;
                 },
                 [&](auto t) {
                   rndsleep();
                   auto [a, b, c] = t;
                   EXPECT_EQ(a + b + c, 1 + 2 + 3);
                   return 1;
                 })
               .then(
                 [](auto t) {
                   auto [a, b] = t;
                   EXPECT_EQ(a + b, 1);
                   return std::string{"hello"};
                 },
                 [](auto t) {
                   auto [a, b] = t;
                   EXPECT_EQ(a + b, 1);
                   return std::string{"world"};
                 })
               .then([](auto y) {
                 auto [s0, s1] = y;
                 EXPECT_EQ(s0 + s1, "helloworld");
               });

    std::move(f).wait_and_get(zero_alloc_thread_pool{});
  }

  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(ctr, 0);
    auto f = initiate([] { return std::make_shared<sctr>(); })
               .then([](auto&) { EXPECT_EQ(ctr, 1); },
                     [](auto&) { EXPECT_EQ(ctr, 1); })
               .then([](auto) { EXPECT_EQ(ctr, 0); });

    std::move(f).wait_and_get(zero_alloc_thread_pool{});
    EXPECT_EQ(ctr, 0);
  }
}

TEST(ZeroAllocationContinuations, DISABLED_Test2)
{
  using namespace BABYLON;

  {
    auto f = initiate([] { return 1; });
    EXPECT_EQ(std::move(f).wait_and_get(zero_alloc_thread_pool{}), 1);
  }

  {
    auto f = initiate([] { return 1; }).then([](int x) { return x + 1; });
    EXPECT_EQ(std::move(f).wait_and_get(zero_alloc_thread_pool{}), 2);
  }

  {
    auto f = initiate([] { return 1; })
               .then([](int x) { return x + 1; })
               .then([](int x) { return x + 1; });
    EXPECT_EQ(std::move(f).wait_and_get(zero_alloc_thread_pool{}), 3);
  }

  {
    auto f = initiate([] { return 1; })
               .then([](int x) { return x + 1; })
               .then([](int) { /* std::cout << "void!\n"; */ });
    std::move(f).wait_and_get(zero_alloc_thread_pool{});
  }

  {
    auto f = initiate([] { return 1; }, [] { return 2; });
    EXPECT_EQ(std::move(f).wait_and_get(zero_alloc_thread_pool{}),
              (std::tuple{1, 2}));
  }

  {
    auto f = initiate([] { return 1; }, [] { return 1; }).then([](auto t) {
      auto [a, b] = t;
      return a + b;
    });
    EXPECT_EQ(std::move(f).wait_and_get(zero_alloc_thread_pool{}), 2);
  }

  auto f2
    = initiate(
        [] { /* std::cout << "A0\n"; */
             return 1;
        },
        [] { /* std::cout << "A1\n"; */
             return 2;
        })
        .then([](auto t) {
          auto [a, b] = t;
          EXPECT_EQ(a + b, 3);
          return a + b;
        })
        .then(
          [](auto x) {
            EXPECT_EQ(x, 3); /* std::cout << x << " C0\n"; */
            return std::string{"hello"};
          },
          [](auto x) {
            EXPECT_EQ(x, 3); /* std::cout << x << " C1\n"; */
            return std::string{"world"};
          })
        .then([](auto y) {
          auto [s0, s1] = y;
          EXPECT_EQ(s0, "hello");
          EXPECT_EQ(s1, "world"); /* std::cout << s0 << " " << s1 << "\n"; */
        });

  std::move(f2).wait_and_get(zero_alloc_thread_pool{});

  auto f3 = initiate([] {}).then([] {});
  std::move(f3).wait_and_get(zero_alloc_thread_pool{});

  auto f4 = initiate([] {}, [] {}).then([](auto) {});
  std::move(f4).wait_and_get(zero_alloc_thread_pool{});

  auto f5 = initiate(
              [] { /* std::cout << "A0\n"; */
                   return 1;
              },
              [] { /* std::cout << "A1\n"; */
                   return 2;
              })
              .then([](auto t) {
                auto [a, b] = t;
                EXPECT_EQ(a + b, 3);
                return a + b;
              })
              .then(
                [](auto x) {
                  EXPECT_EQ(x, 3); /* std::cout << x << " C0\n"; */
                  return 2;
                },
                [](auto x) {
                  EXPECT_EQ(x, 3); /* std::cout << x << " C1\n"; */
                  return 3;
                })
              .then(
                [](auto t) {
                  auto [a, b] = t;
                  auto x      = a + b;
                  EXPECT_EQ(x, 5); /* std::cout << x << " C2\n"; */
                  return 4;
                },
                [](auto t) {
                  auto [a, b] = t;
                  auto x      = a + b;
                  EXPECT_EQ(x, 5); /* std::cout << x << " C3\n"; */
                  return 5;
                })
              .then([](auto y) {
                auto [a, b] = y;
                EXPECT_EQ(a + b, 9);
              });

  std::move(f5).wait_and_get(zero_alloc_thread_pool{});
}
