#include <gtest/gtest.h>

#include <babylon/babylon_stl.h>
#include <babylon/core/delegate.h>

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
  int square_const(int x) const
  {
    return x * x;
  }
  int addOrMultiply(int x, int y, char a)
  {
    return a == 'a' ? x + y : x * y;
  }

  bool simpleFunctionWasCalled;
};

static bool myFreeFunctionGotCalled = false;
static void myFreeFunction()
{
  myFreeFunctionGotCalled = true;
}
static bool myFreeFunction2GotCalled = false;
static int myFreeFunction2parameter  = 0;
static void myFreeFunction2(int x)
{
  myFreeFunction2GotCalled = true;
  myFreeFunction2parameter = x;
}

typedef BABYLON::delegate<void()> ServiceDelegate;
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
typedef BABYLON::delegate<void(int)> ServiceDelegate2;
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
  using namespace BABYLON;
  A a;
  // Call void function with 0 parameter
  {
    auto d = delegate<void()>::from<A, &A::simple>(&a);
    d();
    static_assert(std::is_same<delegate<void()>, decltype(d)>::value, "!");
    EXPECT_TRUE(a.simpleFunctionWasCalled);
  }
  // Call void function with 0 parameter - alternative syntax
  {
    auto d = delegate<void()>(&a, &A::simple);
    d();
    static_assert(std::is_same<delegate<void()>, decltype(d)>::value, "!");
    EXPECT_TRUE(a.simpleFunctionWasCalled);
  }
  // Call function with 0 parameter, returning int
  {
    auto d = delegate<int()>::from<A, &A::simpleInt>(&a);
    static_assert(std::is_same<delegate<int()>, decltype(d)>::value, "!");
    EXPECT_EQ(d(), 42);
  }
  // Call with 1 parameter, returning int
  {
    auto d = delegate<int(int)>::from<A, &A::square>(&a);
    static_assert(std::is_same<delegate<int(int)>, decltype(d)>::value, "!");
    EXPECT_EQ(25, d(5));
  }
  // Call with 1 parameter (pointer), returning int
  {
    auto d = delegate<int(int*)>::from<A, &A::squareStar>(&a);
    static_assert(std::is_same<delegate<int(int*)>, decltype(d)>::value, "!");
    int in = 5;
    EXPECT_EQ(25, d(&in));
  }
  // Call const function with 1 parameter, returning int
  {
    auto d = delegate<int(int)>::from<A, &A::square_const>(&a);
    static_assert(std::is_same<delegate<int(int)>, decltype(d)>::value, "!");
    EXPECT_EQ(25, d(5));
  }
  // Call with 2 parameter, returning int
  {
    auto d = delegate<int(int, int)>::from<A, &A::add>(&a);
    static_assert(std::is_same<delegate<int(int, int)>, decltype(d)>::value,
                  "!");
    EXPECT_EQ(9, d(4, 5));
  }
  // Call with multiple parameters
  {
    auto d = delegate<int(int, int, char)>::from<A, &A::addOrMultiply>(&a);
    static_assert(
      std::is_same<delegate<int(int, int, char)>, decltype(d)>::value, "!");
    EXPECT_EQ(9, d(5, 4, 'a'));
    EXPECT_EQ(20, d(5, 4, 'm'));
  }
}

TEST(TestDelegate, FreeFunctionCalls)
{
  using namespace BABYLON;
  // Calling free function
  {
    auto d = delegate<void()>{myFreeFunction};
    static_assert(std::is_same<delegate<void()>, decltype(d)>::value, "!");
    EXPECT_FALSE(myFreeFunctionGotCalled);
    Service s;
    s.registerDelegate(d);
    s.notifyDelegate();
    EXPECT_TRUE(myFreeFunctionGotCalled);
  }
  // Calling free function with argument
  {
    auto d = delegate<void(int)>{myFreeFunction2};
    static_assert(std::is_same<delegate<void(int)>, decltype(d)>::value, "!");
    EXPECT_FALSE(myFreeFunction2GotCalled);
    Service2 s2;
    s2.registerDelegate(d);
    s2.notifyDelegate(345);
    EXPECT_TRUE(myFreeFunction2GotCalled);
    EXPECT_EQ(345, myFreeFunction2parameter);
  }
}

TEST(TestDelegate, LambdaCalls)
{
  using namespace BABYLON;
  // Call lambda with 0 parameter, returning int
  {
    auto d = delegate<int()>([]() { return 42; });
    static_assert(std::is_same<delegate<int()>, decltype(d)>::value, "!");
    EXPECT_EQ(d(), 42);
  }
  // Call with 2 parameter, returning int
  {
    auto d = delegate<int(int, int)>([](int a, int b) { return a + b; });
    static_assert(std::is_same<delegate<int(int, int)>, decltype(d)>::value,
                  "!");
    EXPECT_EQ(9, d(4, 5));
  }
}

TEST(TestDelegate, CompareCopiedDelegates)
{
  using namespace BABYLON;
  A a;
  // Compare: void function with 0 parameter
  {
    auto d      = delegate<void()>::from<A, &A::simple>(&a);
    auto dother = delegate<void()>::from<A, &A::simpleOther>(&a);
    auto d2     = d;
    EXPECT_EQ(d, d2);
    EXPECT_NE(d, dother);
  }
  // Compare: with 1 parameter, returning int
  {
    auto d  = delegate<int(int)>::from<A, &A::square>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Compare: with 1 parameter (pointer), returning int
  {
    auto d  = delegate<int(int*)>::from<A, &A::squareStar>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Compare: const function with 1 parameter, returning int
  {
    auto d  = delegate<int(int)>::from<A, &A::square_const>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Call with 2 parameter, returning int
  {
    auto d  = delegate<int(int, int)>::from<A, &A::add>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
  // Call with multiple parameters
  {
    auto d  = delegate<int(int, int, char)>::from<A, &A::addOrMultiply>(&a);
    auto d2 = d;
    EXPECT_EQ(d, d2);
  }
}
