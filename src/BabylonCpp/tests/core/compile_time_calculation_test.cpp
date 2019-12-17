#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Compile-time Factorial calculation

template <size_t N>
struct Factorial {
  enum { value = N * Factorial<N - 1>::value };
};

template <>
struct Factorial<1> {
  enum { value = 1 };
};

// Compile-time Factorial calculation using C++11 constexpr

constexpr size_t factorial(size_t n)
{
  return n ? (n * factorial(n - 1)) : 1;
}

// Compile-time Fibonacci calculation

template <size_t N>
struct Fibonacci {
  enum { value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value };
};

template <>
struct Fibonacci<0> {
  enum { value = 0 };
};

template <>
struct Fibonacci<1> {
  enum { value = 1 };
};

// Compile-time is number

constexpr bool isNumber(const char c)
{
  return c >= '0' && c <= '9';
}

// Compile-time exponent calculation.
// Pow<2,3>::value == 2^3

template <size_t B, size_t E>
struct Pow {
  enum { value = B * Pow<B, E - 1>::value };
};

template <size_t B>
struct Pow<B, 0> {
  enum { value = 1 };
};

// Compile-time Primality test

template <size_t p, size_t i>
struct PrimalityTest {
  enum { value = p % i != 0 && PrimalityTest<p, i - 1>::value };
};

template <size_t p>
struct PrimalityTest<p, 1> {
  enum { value = 1 };
};

template <size_t p>
struct IsPrime {
  enum { value = PrimalityTest<p, p - 1>::value };
};

TEST(TestCompileTimeCalculation, Factorial)
{
  // Without using C++11 constexpr.
  {
    static_assert(Factorial<7>::value == 5040, "f(7) == 5040");
    static_assert(Factorial<12>::value == 479001600, "f(12) == 479001600");

    EXPECT_EQ(Factorial<7>::value, 5040);
    EXPECT_EQ(Factorial<12>::value, 479001600);
  }

  // Using C++11 constexpr.
  {
    static_assert(factorial(8) == 40320, "f(8) == 40320");
#if 0
    // This test only works on 64 bits build
    //static_assert(factorial(13) == 6227020800, "f(13) == 6227020800");
#endif
    EXPECT_EQ(factorial(8), 40320ull);
    EXPECT_EQ(factorial(13), 6227020800ull);
  }
}

TEST(TestCompileTimeCalculation, Fibonacci)
{
  static_assert(Fibonacci<12>::value == 144, "f(12) == 144");
  static_assert(Fibonacci<20>::value == 6765, "f(10) == 6765");

  EXPECT_EQ(Fibonacci<12>::value, 144);
  EXPECT_EQ(Fibonacci<20>::value, 6765);
}

TEST(TestCompileTimeCalculation, isNumber)
{
  static_assert(isNumber('5') == 1, "5 is a number");
  static_assert(isNumber('d') == 0, "d is not a number");

  EXPECT_TRUE(isNumber('5'));
  EXPECT_FALSE(isNumber('d'));
}

TEST(TestCompileTimeCalculation, IsPrime)
{
  static_assert(IsPrime<13>::value == 1, "13 is prime");
  static_assert(IsPrime<24>::value == 0, "24 is not prime");

  EXPECT_TRUE(IsPrime<13>::value);
  EXPECT_FALSE(IsPrime<24>::value);
}

TEST(TestCompileTimeCalculation, Pow)
{
  static_assert(Pow<2, 3>::value == 8, "f(2,3) == 8");
  static_assert(Pow<16, 2>::value == 256, "f(16,2) == 256");

  constexpr size_t pow1 = Pow<2, 3>::value;
  constexpr size_t pow2 = Pow<16, 2>::value;
  EXPECT_EQ(pow1, 8);
  EXPECT_EQ(pow2, 256);
}
