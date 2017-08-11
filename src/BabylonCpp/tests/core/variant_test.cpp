#include <gtest/gtest.h>

#include <babylon/core/variant.h>

struct test {
  int* holder;
  test()
  {
    holder = new int();
  }

  test(test&& old) : holder(nullptr)
  {
    std::swap(holder, old.holder);
  }
  test(const test& old)
  {
    holder = new int(*old.holder);
  }
  ~test()
  {
    delete holder;
  }
};

TEST(TestVariant, Basic)
{
  using namespace BABYLON;

  Variant<std::string, test> d;
  d.set<std::string>("First string");
  EXPECT_TRUE(d.is<std::string>());
  EXPECT_FALSE(d.is<test>());
  EXPECT_EQ(d.get<std::string>(), "First string");

  d.set<test>();
  *d.get<test>().holder = 42;
  Variant<std::string, test> e(std::move(d));
  EXPECT_FALSE(d.is<std::string>());
  EXPECT_TRUE(d.is<test>());
  EXPECT_EQ(*e.get<test>().holder, 42);

  *e.get<test>().holder = 43;
  d                     = e;
  EXPECT_FALSE(d.is<std::string>());
  EXPECT_TRUE(d.is<test>());
  EXPECT_EQ(*d.get<test>().holder, 43);
}
