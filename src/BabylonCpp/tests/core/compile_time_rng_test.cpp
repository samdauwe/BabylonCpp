#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/compile_time_rng.h>
#include <unordered_set>

TEST(TestCompileTimeRNG, Get)
{
  using namespace BABYLON;

  auto TEST_IMPL = [](std::unordered_set<std::size_t>& Set, std::size_t N) {
    Set.insert(compile_time_rng<std::size_t>::seed<__LINE__>::get<0>::value);
    ++N;
  };

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);

  EXPECT_GE(s.size(), n * 95 / 100);

#undef TEST_IMPL
}

TEST(TestCompileTimeRNG, Iteration)
{
  using namespace BABYLON;

#define TEST_IMPL(Set, N, Offset)                                              \
  do {                                                                         \
    Set.insert(                                                                \
      compile_time_rng<std::size_t>::get<__LINE__ - Offset - 1>::value);       \
    ++N;                                                                       \
  } while (false)

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  auto const offset = __LINE__;
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);
  TEST_IMPL(s, n, offset);

  EXPECT_GE(s.size(), n * 95 / 100);

#undef TEST_IMPL
}

TEST(TestCompileTimeRNG, FatalCompileTimeRNGs)
{
  using namespace BABYLON;

  auto TEST_IMPL = [](std::unordered_set<std::size_t>& Set, std::size_t N) {
    using rng = BABYLON_COMPILE_TIME_RNG<std::size_t>;
    Set.insert(rng::get<>::value);
    ++N;
  };

  std::unordered_set<std::size_t> s;
  std::size_t n = 0;

  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);
  TEST_IMPL(s, n);

  EXPECT_GE(s.size(), n * 95 / 100);

#undef TEST_IMPL
}
