#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/any.h>

struct E {
  E(int a) : _a{a}
  {
  }
  E(E&& e) = default;
  E(E const& e) = default;
  E& operator=(E&&) = delete;
  E& operator=(E const&) = delete;

  int _a;
};

TEST(TestAny, Initialization)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  EXPECT_EQ(a0._<bool>(), true);
  EXPECT_EQ(a1._<char>(), 'A');
  EXPECT_EQ(a2._<int>(), 6);
  EXPECT_EQ(a3._<double>(), 3.14);
  EXPECT_STREQ(a4._<const char*>(), "hello,");
  EXPECT_THAT(a5._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a6._<double>(), 2.71);
  EXPECT_EQ(a7._<std::string>(), "world!");
}

TEST(TestAny, CopyAssignment)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  any c0 = a0;
  any c1 = a1;
  any c2 = a2;
  any c3 = a3;
  any c4{a4};
  any c5{a5};
  any c6{a6};
  any c7{a7};

  a0.clear();
  a1.clear();
  a2.clear();
  a3.clear();
  a4.clear();
  a5.clear();
  a6.clear();
  a7.clear();

  EXPECT_EQ(c0._<bool>(), true);
  EXPECT_EQ(c1._<char>(), 'A');
  EXPECT_EQ(c2._<int>(), 6);
  EXPECT_EQ(c3._<double>(), 3.14);
  EXPECT_STREQ(c4._<const char*>(), "hello,");
  EXPECT_THAT(c5._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(c6._<double>(), 2.71);
  EXPECT_EQ(c7._<std::string>(), "world!");
}

TEST(TestAny, MoveAssignment)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any c0 = true;
  any c1 = 'A';
  any c2 = 6;
  any c3 = 3.14;
  any c4 = "hello,";
  any c5 = testArray;
  any c6 = 2.71;
  any c7 = std::string("world!");

  any m0 = std::move(c0);
  any m1 = std::move(c1);
  any m2 = std::move(c2);
  any m3 = std::move(c3);
  any m4{std::move(c4)};
  any m5{std::move(c5)};
  any m6{std::move(c6)};
  any m7{std::move(c7)};

  EXPECT_EQ(m0._<bool>(), true);
  EXPECT_EQ(m1._<char>(), 'A');
  EXPECT_EQ(m2._<int>(), 6);
  EXPECT_EQ(m3._<double>(), 3.14);
  EXPECT_STREQ(m4._<const char*>(), "hello,");
  EXPECT_THAT(m5._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(m6._<double>(), 2.71);
  EXPECT_EQ(m7._<std::string>(), "world!");
}

TEST(TestAny, InitializeVector)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  std::vector<any> a(8);
  a[0] = true;
  a[1] = 'A';
  a[2] = 6;
  a[3] = 3.14;
  a[4] = "hello,";
  a[5] = testArray;
  a[6] = 2.71;
  a[7] = std::string("world!");

  EXPECT_EQ(a[0]._<bool>(), true);
  EXPECT_EQ(a[1]._<char>(), 'A');
  EXPECT_EQ(a[2]._<int>(), 6);
  EXPECT_EQ(a[3]._<double>(), 3.14);
  EXPECT_STREQ(a[4]._<const char*>(), "hello,");
  EXPECT_THAT(a[5]._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a[6]._<double>(), 2.71);
  EXPECT_EQ(a[7]._<std::string>(), "world!");
}

TEST(TestAny, AssignToVectorElements)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  std::vector<any> a(8);
  a[0] = a0;
  a[1] = a1;
  a[2] = a2;
  a[3] = a3;
  a[4] = a4;
  a[5] = a5;
  a[6] = a6;
  a[7] = a7;

  EXPECT_EQ(a[0]._<bool>(), true);
  EXPECT_EQ(a[1]._<char>(), 'A');
  EXPECT_EQ(a[2]._<int>(), 6);
  EXPECT_EQ(a[3]._<double>(), 3.14);
  EXPECT_STREQ(a[4]._<const char*>(), "hello,");
  EXPECT_THAT(a[5]._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a[6]._<double>(), 2.71);
  EXPECT_EQ(a[7]._<std::string>(), "world!");
}

TEST(TestAny, MoveToVectorElements)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  std::vector<any> a(8);
  a[0] = std::move(a0);
  a[1] = std::move(a1);
  a[2] = std::move(a2);
  a[3] = std::move(a3);
  a[4] = std::move(a4);
  a[5] = std::move(a5);
  a[6] = std::move(a6);
  a[7] = std::move(a7);

  EXPECT_EQ(a[0]._<bool>(), true);
  EXPECT_EQ(a[1]._<char>(), 'A');
  EXPECT_EQ(a[2]._<int>(), 6);
  EXPECT_EQ(a[3]._<double>(), 3.14);
  EXPECT_STREQ(a[4]._<const char*>(), "hello,");
  EXPECT_THAT(a[5]._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a[6]._<double>(), 2.71);
  EXPECT_EQ(a[7]._<std::string>(), "world!");

  auto is_double = [](const any& s) { return s.is<double>(); };
  auto cnt       = std::count_if(a.begin(), a.end(), is_double);
  EXPECT_EQ(cnt, 2);
}

TEST(TestAny, TypeCheck)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  std::vector<any> a(8);
  a[0] = true;
  a[1] = 'A';
  a[2] = 6;
  a[3] = 3.14;
  a[4] = "hello,";
  a[5] = testArray;
  a[6] = 2.71;
  a[7] = std::string("world!");

  EXPECT_FALSE(a[0].is<long>());
  EXPECT_FALSE(a[1].is<long>());
  EXPECT_FALSE(a[2].is<long>());
  EXPECT_FALSE(a[3].is<long>());
  EXPECT_FALSE(a[4].is<long>());
  EXPECT_FALSE(a[5].is<long>());
  EXPECT_FALSE(a[6].is<long>());
  EXPECT_FALSE(a[7].is<long>());

  EXPECT_TRUE(a[0].is<bool>());
  EXPECT_TRUE(a[1].is<char>());
  EXPECT_TRUE(a[2].is<int>());
  EXPECT_TRUE(a[3].is<double>());
  EXPECT_TRUE(a[4].is<const char*>());
  EXPECT_TRUE(a[5].is<std::vector<int>>());
  EXPECT_TRUE(a[6].is<double>());
  EXPECT_TRUE(a[7].is<std::string>());
}

TEST(TestAny, NullptrType)
{
  using namespace BABYLON;

  any a = nullptr;
  EXPECT_TRUE(a.is<std::nullptr_t>());

  a = 0;
  EXPECT_FALSE(a.is<std::nullptr_t>());
  EXPECT_TRUE(a.is<int>());
}

TEST(TestAny, Swap)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a5 = testArray;
  any a7 = std::string("world!");
  EXPECT_THAT(a5._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a7._<std::string>(), "world!");

  swap(a5, a7);
  EXPECT_EQ(a5._<std::string>(), "world!");
  EXPECT_THAT(a7._<std::vector<int>>(), ::testing::ContainerEq(testArray));

  swap(a5, a7);
  EXPECT_THAT(a5._<std::vector<int>>(), ::testing::ContainerEq(testArray));
  EXPECT_EQ(a7._<std::string>(), "world!");
}

TEST(TestAny, CopyConstructor)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};
  std::vector<int> expected{6, 5, -2, 3, -2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  bool& l0(a0);
  l0 = false;

  char& l1(a1);
  l1 = 'B';

  int& l2(a2);
  l2 = 8;

  double& l3(a3);
  l3 = -3.14;

  const char*& l4 = a4;
  l4              = "Hello,";

  std::vector<int>& l5 = a5;
  l5[2]                = -2;

  double& l6 = a6;
  l6         = -2.71;

  std::string& l7 = a7;
  l7 += " ...";

  a5._<std::vector<int>>()[4] = -2;
  a7._<std::string>()[0]      = 'W';

  EXPECT_EQ(a0._<bool>(), false);
  EXPECT_EQ(a1._<char>(), 'B');
  EXPECT_EQ(a2._<int>(), 8);
  EXPECT_EQ(a3._<double>(), -3.14);
  EXPECT_STREQ(a4._<const char*>(), "Hello,");
  EXPECT_THAT(a5._<std::vector<int>>(), ::testing::ContainerEq(expected));
  EXPECT_EQ(a6._<double>(), -2.71);
  EXPECT_EQ(a7._<std::string>(), "World! ...");
}

TEST(TestAny, MoveConstructor)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};
  std::vector<int> expected{6, 5, 4, -4, 2, -4, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  bool&& r0(std::move(a0));
  r0 = true;

  char&& r1(std::move(a1));
  r1 = 'C';

  int&& r2(std::move(a2));
  r2 = 12;

  double&& r3(std::move(a3));
  r3 = 3.14159;

  const char*&& r4 = std::move(a4);
  r4               = "HEllo,";

  std::vector<int>&& r5 = std::move(a5);
  r5[3]                 = -4;

  double&& r6 = std::move(a6);
  r6          = 2.71828;

  std::string&& r7 = std::move(a7);
  r7 += " !!!";

  std::move(a5)._<std::vector<int>>()[5] = -4;
  std::move(a7)._<std::string>()[1]      = 'O';

  EXPECT_EQ(a0._<bool>(), true);
  EXPECT_EQ(a1._<char>(), 'C');
  EXPECT_EQ(a2._<int>(), 12);
  EXPECT_EQ(a3._<double>(), 3.14159);
  EXPECT_STREQ(a4._<const char*>(), "HEllo,");
  EXPECT_THAT(a5._<std::vector<int>>(), ::testing::ContainerEq(expected));
  EXPECT_EQ(a6._<double>(), 2.71828);
  EXPECT_EQ(a7._<std::string>(), "wOrld! !!!");
}

TEST(TestAny, FromAnyToTypeByCopy)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  bool x0{a0};
  char x1{a1};
  int x2{a2};
  double x3{a3};
  const char* x4      = a4;
  std::vector<int> x5 = a5;
  double x6           = a6;
  std::string x7      = a7;

  EXPECT_EQ(x0, true);
  EXPECT_EQ(x1, 'A');
  EXPECT_EQ(x2, 6);
  EXPECT_EQ(x3, 3.14);
  EXPECT_STREQ(x4, "hello,");
  EXPECT_THAT(x5, ::testing::ContainerEq(testArray));
  EXPECT_EQ(x6, 2.71);
  EXPECT_EQ(x7, "world!");
}

TEST(TestAny, FromAnyToTypeByMove)
{
  using namespace BABYLON;

  std::vector<int> testArray{6, 5, 4, 3, 2, 1, 0};

  any a0 = true;
  any a1 = 'A';
  any a2 = 6;
  any a3 = 3.14;
  any a4 = "hello,";
  any a5 = testArray;
  any a6 = 2.71;
  any a7 = std::string("world!");

  bool y0{std::move(a0)};
  char y1{std::move(a1)};
  int y2{std::move(a2)};
  double y3{std::move(a3)};
  const char* y4      = std::move(a4);
  std::vector<int> y5 = std::move(a5);
  double y6           = std::move(a6);
  std::string y7      = std::move(a7);

  EXPECT_EQ(y0, true);
  EXPECT_EQ(y1, 'A');
  EXPECT_EQ(y2, 6);
  EXPECT_EQ(y3, 3.14);
  EXPECT_STREQ(y4, "hello,");
  EXPECT_THAT(y5, ::testing::ContainerEq(testArray));
  EXPECT_EQ(y6, 2.71);
  EXPECT_EQ(y7, "world!");
}

TEST(TestAny, CustomType)
{
  using namespace BABYLON;

  any a = E(5);
  EXPECT_FALSE(a.is<int>());
  EXPECT_TRUE(a.is<E>());

  EXPECT_EQ(a._<E>()._a, 5);
  EXPECT_EQ(a.cast<E>()._a, 5);
  EXPECT_NE(a.cast<E>()._a, 0);
}
