#include <gtest/gtest.h>

#include <babylon/core/future.h>

struct MsgType {
  std::string _msg;
  MsgType(std::string m) : _msg{m}
  {
  }
  std::string msg()
  {
    return _msg;
  }
};

struct FibonacciCalculator {
  unsigned int n;
  FibonacciCalculator(unsigned int target) : n{target}
  {
  }
  unsigned int calculate()
  {
    if (n == 0)
      return 0;
    BABYLON::Uint32Array v(n, 1);
    std::adjacent_difference(v.begin(), v.end() - 1, v.begin() + 1,
                             std::plus<unsigned int>());
    // "array" now contains the Fibonacci sequence from 1 up
    return v[n - 1];
  }
};

TEST(TestFuture, BasicWorker)
{
  using namespace BABYLON;
  const ::std::string str("Hello from struct");
  MsgType type(str);
  ::std::unique_ptr<Active> bgWorker(Active::createActive());
  ::std::future<::std::string> fstring
    = spawn_task(::std::bind(&MsgType::msg, type), bgWorker.get());
  EXPECT_STREQ(str.c_str(), fstring.get().c_str());
}

TEST(TestFuture, FibonacciCalculationWorker)
{
  using namespace BABYLON;
  ::std::unique_ptr<Active> bgWorker(Active::createActive());
  ::std::future<unsigned int> fibNumber12 = spawn_task(
    ::std::bind(&FibonacciCalculator::calculate, FibonacciCalculator(12)),
    bgWorker.get());
  ::std::future<unsigned int> fibNumber20 = spawn_task(
    ::std::bind(&FibonacciCalculator::calculate, FibonacciCalculator(20)),
    bgWorker.get());
  EXPECT_EQ(144 + 6765, fibNumber12.get() + fibNumber20.get());
}

TEST(TestFuture, CopyableLambdaCall)
{
  using namespace BABYLON;
  ::std::unique_ptr<Active> bgWorker(Active::createActive());

  // lambda task
  const ::std::string str_standalone("Hello from standalone");
  auto msg_lambda = [=]() { return (str_standalone + str_standalone); };
  ::std::string expected(str_standalone + str_standalone);

  auto fstring_standalone = spawn_task(msg_lambda, bgWorker.get());
  EXPECT_STREQ(expected.c_str(), fstring_standalone.get().c_str());
}

TEST(TestFuture, CannotCallSpawnTaskOnNullptrWorker)
{
  using namespace BABYLON;
  auto FailedHelloWorld = [] { ::std::cout << "Hello World" << ::std::endl; };
  Active* active        = nullptr;
  auto failed           = spawn_task(FailedHelloWorld, active);
  EXPECT_ANY_THROW(failed.get());
}
