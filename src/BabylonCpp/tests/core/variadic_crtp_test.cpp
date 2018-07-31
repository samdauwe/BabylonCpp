#include <gmock/gmock.h>
#include <gtest/gtest.h>

/**
 * @brief Variadic Curiously-Recurring Template Pattern (CRTP) sample.
 * @see http://stevedewhurst.com/once_weakly.html
 * @see
 * https://www.fluentcpp.com/2018/06/22/variadic-crtp-opt-in-for-class-features-at-compile-time/
 * @see
 * https://www.fluentcpp.com/2018/07/03/how-to-reduce-the-code-bloat-of-a-variadic-crtp/
 */

template <typename T>
class Sum;

template <typename T>
class Multiplication;

template <typename T>
class SumMul : public Sum<T>, public Multiplication<T> {
};

template <template <typename> typename... CRTPs>
class Base : public CRTPs<Base<CRTPs...>>... {
public:
  // using Calculator = Base<SumMul>;
  friend class Sum<Base<SumMul>>;
  friend class Multiplication<Base<SumMul>>;
  // using Calculator = Base<Sum, Multiplication>;
  friend class Sum<Base<Sum, Multiplication>>;
  friend class Multiplication<Base<Sum, Multiplication>>;

  Base(int a) : _value(a)
  {
  }

  int value()
  {
    return _value;
  }

protected:
  int _value;
};

template <typename T>
class Sum {
public:
  auto sum(int value)
  {
    auto& derived = static_cast<T&>(*this);
    derived._value += value;
    return *this;
  }
};

template <typename T>
class Multiplication {
public:
  auto multiply(int value)
  {
    auto& derived = static_cast<T&>(*this);
    derived._value *= value;
    return *this;
  }
};

TEST(TestVariadicCRTP, ClassExtension)
{
  {
    using Calculator = Base<SumMul>;

    Calculator calc{10};
    EXPECT_EQ(calc.value(), 10);
    calc.sum(5);
    EXPECT_EQ(calc.value(), 15);
    calc.multiply(2);
    EXPECT_EQ(calc.value(), 30);
  }

  {
    using Calculator = Base<Sum, Multiplication>;

    Calculator calc{5};
    EXPECT_EQ(calc.value(), 5);
    calc.multiply(2);
    EXPECT_EQ(calc.value(), 10);
    calc.sum(30);
    EXPECT_EQ(calc.value(), 40);
  }
}
