#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/core/nullable.h>

struct E {
  E(int a) : _a{a}
  {
  }
  E(E&& e) : _a{std::move(e._a)}
  {
  }
  E(E const& e) : _a{e._a}
  {
  }
  E& operator=(E&&) = delete;
  E& operator=(E const&) = delete;

  int _a;
};

TEST(TestNullable, Initialization)
{
  using namespace BABYLON;

  // Null type
  {
    Nullable<int> test;
    EXPECT_FALSE(test.hasValue());
    EXPECT_FALSE(test);
    EXPECT_TRUE(test.isNull());
    EXPECT_TRUE(test == nullptr);
  }

  // With value
  {
    Nullable<int> test(7);
    EXPECT_TRUE(test.hasValue());
    EXPECT_TRUE(test);
    EXPECT_FALSE(test.isNull());
    EXPECT_FALSE(test == nullptr);
  }
}

TEST(TestNullable, Assignment)
{
  using namespace BABYLON;

  // Nullptr
  {
    Nullable<int> test = nullptr;
    EXPECT_FALSE(test.hasValue());
    EXPECT_FALSE(test);
    EXPECT_TRUE(test.isNull());
    EXPECT_TRUE(test == nullptr);
  }

  // With value
  {
    Nullable<int> test = 5;
    EXPECT_TRUE(test.hasValue());
    EXPECT_TRUE(test);
    EXPECT_FALSE(test.isNull());
    EXPECT_FALSE(test == nullptr);
    EXPECT_EQ(test.value, 5);
  }
}

TEST(TestNullable, CopyAssignment)
{
  using namespace BABYLON;

  // Null type
  {
    Nullable<int> test;
    auto cpy = test;
    EXPECT_FALSE(cpy.hasValue());
    EXPECT_FALSE(cpy);
    EXPECT_TRUE(cpy.isNull());
    EXPECT_TRUE(cpy == nullptr);
    EXPECT_EQ(cpy, test);
  }

  // With value
  {
    Nullable<int> test(7);
    auto cpy = test;
    EXPECT_TRUE(cpy.hasValue());
    EXPECT_TRUE(cpy);
    EXPECT_FALSE(cpy.isNull());
    EXPECT_EQ(cpy, test);
  }
}

TEST(TestNullable, MoveAssignment)
{
  using namespace BABYLON;

  // Null type
  {
    Nullable<int> test;
    auto m = std::move(test);
    EXPECT_EQ(m.hasValue(), test.hasValue());
    EXPECT_EQ(m, test);
    EXPECT_EQ(m.isNull(), test.isNull());
  }

  // With value
  {
    Nullable<int> test(7);
    auto m = std::move(test);
    EXPECT_FALSE(test.hasValue());
    EXPECT_FALSE(test);
    EXPECT_TRUE(test.isNull());
    EXPECT_TRUE(m.hasValue());
    EXPECT_TRUE(m);
    EXPECT_FALSE(m.isNull());
    EXPECT_EQ(m.value, 7);
  }
}

TEST(TestNullable, CopyConstructor)
{
  using namespace BABYLON;

  // Null type
  {
    Nullable<int> test;
    auto cpy(test);
    EXPECT_FALSE(cpy.hasValue());
    EXPECT_FALSE(cpy);
    EXPECT_TRUE(cpy.isNull());

    test = 7;
    EXPECT_TRUE(test);
    EXPECT_FALSE(cpy);
    EXPECT_EQ(test.value, 7);
  }

  // With value
  {
    Nullable<int> test(3);
    auto cpy(test);
    EXPECT_TRUE(cpy.hasValue());
    EXPECT_TRUE(cpy);
    EXPECT_FALSE(cpy.isNull());
    EXPECT_EQ(test.value, cpy.value);

    test = 8;
    EXPECT_NE(test.value, cpy.value);
    EXPECT_EQ(test.value, 8);
  }
}

TEST(TestNullable, MoveConstructor)
{
  using namespace BABYLON;

  // Null type
  {
    Nullable<int> test;
    auto m(std::move(test));
    EXPECT_EQ(m.hasValue(), test.hasValue());
    EXPECT_EQ(m, test);
    EXPECT_EQ(m.isNull(), test.isNull());
  }

  // With value
  {
    Nullable<int> test(7);
    auto m(std::move(test));
    EXPECT_FALSE(test.hasValue());
    EXPECT_FALSE(test);
    EXPECT_TRUE(test.isNull());
    EXPECT_TRUE(m.hasValue());
    EXPECT_TRUE(m);
    EXPECT_FALSE(m.isNull());
    EXPECT_EQ(m.value, 7);
  }
}

TEST(TestNullable, NullptrType)
{
  using namespace BABYLON;

  Nullable<int> test = nullptr;
  EXPECT_FALSE(test.hasValue());
  EXPECT_FALSE(test);
  EXPECT_TRUE(test.isNull());

  test = 0;
  EXPECT_TRUE(test.hasValue());
  EXPECT_TRUE(test);
  EXPECT_FALSE(test.isNull());
  EXPECT_EQ(test.value, 0);
}

TEST(TestNullable, Swap)
{
  using namespace BABYLON;

  Nullable<int> a = 5;
  Nullable<int> b = 7;
  EXPECT_EQ(a.value, 5);
  EXPECT_EQ(b.value, 7);

  swap(a, b);
  EXPECT_EQ(a.value, 7);
  EXPECT_EQ(b.value, 5);

  swap(a, b);
  EXPECT_EQ(a.value, 5);
  EXPECT_EQ(b.value, 7);
}

TEST(TestNullable, CustomType)
{
  using namespace BABYLON;

  Nullable<E> test = E(5);
  EXPECT_TRUE(test.hasValue());
  EXPECT_TRUE(test);
  EXPECT_FALSE(test.isNull());
  EXPECT_FALSE(test == nullptr);
  EXPECT_EQ(test.value._a, 5);
}
