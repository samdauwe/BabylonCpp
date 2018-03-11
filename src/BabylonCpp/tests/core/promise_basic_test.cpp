#include <cstdint>
#include <gtest/gtest.h>

#include <babylon/core/promise.h>

namespace BABYLON {

struct Destructor {
  static std::int64_t construct_count;
  static std::int64_t default_construct_count;
  static std::int64_t move_construct_count;
  static std::int64_t copy_construct_count;
  static std::int64_t destruct_count;

  Destructor(void)
  {
    ++construct_count;
    ++default_construct_count;
  }

  Destructor(const Destructor&)
  {
    ++construct_count;
    ++copy_construct_count;
  }
  Destructor(Destructor&&)
  {
    ++construct_count;
    ++move_construct_count;
  }

  ~Destructor(void)
  {
    ++destruct_count;
  }
};
std::int64_t Destructor::construct_count         = 0;
std::int64_t Destructor::default_construct_count = 0;
std::int64_t Destructor::move_construct_count    = 0;
std::int64_t Destructor::copy_construct_count    = 0;
std::int64_t Destructor::destruct_count          = 0;

struct PromiseBasicTests : public testing::Test {
};

TEST(TestPromise, ReuseWithReset)
{
  Promise<> promise;
  int firstCallCount  = 0;
  int secondCallCount = 0;

  promise.future().then([&]() {
    EXPECT_EQ(0, firstCallCount);
    EXPECT_EQ(0, secondCallCount);
    ++firstCallCount;
  });

  EXPECT_EQ(0, firstCallCount);
  EXPECT_EQ(0, secondCallCount);
  promise.resolve();
  EXPECT_EQ(1, firstCallCount);
  EXPECT_EQ(0, secondCallCount);

  promise.reset();
  promise.future().then([&]() {
    EXPECT_EQ(1, firstCallCount);
    EXPECT_EQ(0, secondCallCount);
    ++secondCallCount;
  });

  EXPECT_EQ(1, firstCallCount);
  EXPECT_EQ(0, secondCallCount);
  promise.resolve();
  EXPECT_EQ(1, firstCallCount);
  EXPECT_EQ(1, secondCallCount);
}

TEST(TestPromise, Destruction)
{
  auto promise = std::make_shared<Promise<>>();

  {
    Destructor monitor;
    ASSERT_EQ(1, Destructor::construct_count);
    promise->future().then([monitor]() {
      EXPECT_LE(2, Destructor::construct_count);
      EXPECT_LE(1, Destructor::copy_construct_count);
      EXPECT_LE(1, Destructor::destruct_count);
    });
  }
  EXPECT_LE(2, Destructor::construct_count);
  EXPECT_LE(1, Destructor::destruct_count);

  promise->resolve();
  promise.reset();
  EXPECT_EQ(Destructor::construct_count, Destructor::destruct_count);
}

} // end of namespace BABYLON
