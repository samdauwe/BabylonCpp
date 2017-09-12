#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/time.h>

TEST(TestTime, measureFunctionCall)
{
  using namespace BABYLON;

  const auto sum = [](int a, int b) { return a + b; };
  ::std::chrono::duration<float, ::std::nano> duration;
  float totalDurationNanos = 0.f;
  int result               = 0;
  for (int i = 0; i < 5; ++i) {
    result = Time::measureFunctionCall(duration, sum, i, i + 1);
    if (result > 0) {
      totalDurationNanos += duration.count();
    }
  }
  EXPECT_GT(totalDurationNanos, 0.f);
}
