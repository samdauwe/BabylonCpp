#include <gtest/gtest.h>

#include <babylon/core/delegates/delegate.h>

struct A {

  void simple()
  {
    simpleFunctionWasCalled = true;
  }
  void simpleOther()
  {
  }
  int simpleInt()
  {
    return 42;
  }
  int add(int a, int b)
  {
    return a + b;
  }
  int square(int x)
  {
    return x * x;
  }
  int squareStar(int* x)
  {
    return *x * *x;
  }
  [[nodiscard]] int squareConst(int x) const
  {
    return x * x;
  }
  static int staticSquareConst(int x)
  {
    return x * x;
  }
  int addOrMultiply(int x, int y, char a)
  {
    return a == 'a' ? x + y : x * y;
  }

  bool simpleFunctionWasCalled;

}; // end of struct

using ServiceDelegate = SA::delegate<void()>;
class Service {
public:
  void registerDelegate(ServiceDelegate& d)
  {
    mD = &d;
  }
  void notifyDelegate()
  {
    (*mD)();
  }

private:
  ServiceDelegate* mD;
};
using ServiceDelegate2 = SA::delegate<void(int)>;
class Service2 {
public:
  void registerDelegate(ServiceDelegate2& d)
  {
    mD = &d;
  }
  void notifyDelegate(int x)
  {
    (*mD)(x);
  }

private:
  ServiceDelegate2* mD;
};

TEST(TestDelegate, MemberFunctionCalls)
{
  A a;

  // Call void function with 0 parameter
  {
    auto d = SA::delegate<void()>::create<A, &A::simple>(&a);
    d();
    static_assert(std::is_same<SA::delegate<void()>, decltype(d)>::value, "!");
    EXPECT_TRUE(a.simpleFunctionWasCalled);
  }
  // Call void function with 0 parameter using decltype
  {
    SA::delegate<void()> d;
    auto dFunc = decltype(d)::create<A, &A::simple>(&a);
    dFunc();
    static_assert(std::is_same<SA::delegate<void()>, decltype(d)>::value, "!");
    EXPECT_TRUE(a.simpleFunctionWasCalled);
  }
  // Call function with 0 parameter, returning int
  {
    auto d = SA::delegate<int()>::create<A, &A::simpleInt>(&a);
    static_assert(std::is_same<SA::delegate<int()>, decltype(d)>::value, "!");
    EXPECT_EQ(d(), 42);
  }
  // Call with 1 parameter, returning int
  {
    auto d = SA::delegate<int(int)>::create<A, &A::square>(&a);
    static_assert(std::is_same<SA::delegate<int(int)>, decltype(d)>::value, "!");
    EXPECT_EQ(25, d(5));
  }
  // Call with 1 parameter (pointer), returning int
  {
    auto d = SA::delegate<int(int*)>::create<A, &A::squareStar>(&a);
    static_assert(std::is_same<SA::delegate<int(int*)>, decltype(d)>::value, "!");
    int in = 5;
    EXPECT_EQ(25, d(&in));
  }
  // Call const function with 1 parameter, returning int
  {
    auto d = SA::delegate<int(int)>::create<A, &A::squareConst>(&a);
    static_assert(std::is_same<SA::delegate<int(int)>, decltype(d)>::value, "!");
    EXPECT_EQ(25, d(5));
  }
  // Call static function with 1 parameter, returning int
  {
    auto d = SA::delegate<int(int)>::create<&A::staticSquareConst>();
    static_assert(std::is_same<SA::delegate<int(int)>, decltype(d)>::value, "!");
    EXPECT_EQ(25, d(5));
  }
  // Call with 2 parameter, returning int
  {
    auto d = SA::delegate<int(int, int)>::create<A, &A::add>(&a);
    static_assert(std::is_same<SA::delegate<int(int, int)>, decltype(d)>::value, "!");
    EXPECT_EQ(9, d(4, 5));
  }
  // Call with multiple parameters
  {
    auto d = SA::delegate<int(int, int, char)>::create<A, &A::addOrMultiply>(&a);
    static_assert(std::is_same<SA::delegate<int(int, int, char)>, decltype(d)>::value, "!");
    EXPECT_EQ(9, d(5, 4, 'a'));
    EXPECT_EQ(20, d(5, 4, 'm'));
  }
}

TEST(TestDelegate, LambdaCalls)
{
  // Call lambda with 0 parameter, returning int
  {
    auto d = SA::delegate<int()>([]() { return 42; });
    static_assert(std::is_same<SA::delegate<int()>, decltype(d)>::value, "!");
    EXPECT_EQ(d(), 42);
  }
  // Call with 2 parameter, returning int
  {
    auto d = SA::delegate<int(int, int)>([](int a, int b) { return a + b; });
    static_assert(std::is_same<SA::delegate<int(int, int)>, decltype(d)>::value, "!");
    EXPECT_EQ(9, d(4, 5));
  }
  // Call lamba using decltype
  {
    SA::delegate<int(int, int)> d;
    const auto lambda   = [](int a, int b) { return a + b; };
    d                   = lambda;
    decltype(d) dLambda = lambda;
    EXPECT_TRUE(d != nullptr);
    EXPECT_EQ(11, d(5, 6));
    EXPECT_TRUE(d == dLambda);
    EXPECT_EQ(d, dLambda);
    EXPECT_EQ(11, dLambda(5, 6));
  }
}

TEST(TestDelegate, CompareCopiedDelegates)
{
  // using namespace BABYLON;
  A a;
  // Compare: void function with 0 parameter
  {
    auto d      = SA::delegate<void()>::create<A, &A::simple>(&a);
    auto dother = SA::delegate<void()>::create<A, &A::simpleOther>(&a);
    auto d2     = d;
    EXPECT_EQ(d, d2);
    EXPECT_NE(d, dother);
  }
  // Compare: with 1 parameter, returning int
  {
    auto d  = SA::delegate<int(int)>::create<A, &A::square>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Compare: with 1 parameter (pointer), returning int
  {
    auto d  = SA::delegate<int(int*)>::create<A, &A::squareStar>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Compare: const function with 1 parameter, returning int
  {
    auto d  = SA::delegate<int(int)>::create<A, &A::squareConst>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Call with 2 parameter, returning int
  {
    auto d  = SA::delegate<int(int, int)>::create<A, &A::add>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Call with multiple parameters
  {
    auto d  = SA::delegate<int(int, int, char)>::create<A, &A::addOrMultiply>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
}
