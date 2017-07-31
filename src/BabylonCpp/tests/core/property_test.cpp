#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>

#include <babylon/core/property.h>

using namespace BABYLON;
using namespace Meta;

struct IntTestClass {
  int Int3() const
  {
    return 3;
  }
  int Int5() const
  {
    return 5;
  }
};

/** consumes 8 bytes, because of alignment */
template <int I>
struct PropertyTest {
  using _self = PropertyTest;

private:
  int value_;

  int const& getValue() const
  {
    return value_;
  }
  int const& setValue(int const& x)
  {
    return value_ = x;
  }

public:
  PROPERTY(prop1, _self, getValue, setValue);
  PROPERTY(prop2, _self, getValue, setValue);
  PROPERTY(prop3, _self, getValue, setValue);

}; // end of struct PropertyTest
/** consumes 8 bytes, because of alignment */
static_assert(sizeof(PropertyTest<0>) == sizeof(int) + sizeof(int),
              "External property tester is supposed to be only its value and"
              " the property alignment!");

template <typename T>
struct PropertyWithStorageTest {
  using _self = PropertyWithStorageTest;

public:
  T const& getValue() const
  {
    return prop1.value;
  }
  T const& setValue(T const& x)
  {
    return prop1.value = x;
  }

  PROPERTY_WITH_STORAGE(T, prop1, _self, getValue, setValue);
};
static_assert(sizeof(PropertyWithStorageTest<char>) == sizeof(char),
              "Supposed to be equal in size as what it's storing!");
static_assert(sizeof(PropertyWithStorageTest<int>) == sizeof(int),
              "Supposed to be equal in size as what it's storing!");
static_assert(sizeof(PropertyWithStorageTest<long>) == sizeof(long),
              "Supposed to be equal in size as what it's storing!");

struct MyClass {

  int const& myGetter() const
  {
    return property.value;
  }
  int const& mySetter(std::string const& x)
  {
    return property.value = atoi(x.c_str());
  }
  int const& myIntSetter(int x)
  {
    return property.value = x;
  }
  PROPERTY_WITH_STORAGE(int, property, MyClass, myGetter, mySetter);
  PROPERTY(asInt, MyClass, myGetter, myIntSetter);
};

struct OffsetOfTest {
  int i;
  size_t j;
};

TEST(TestProperty, MemberPointers)
{
  std::map<std::string, int (IntTestClass::*)() const> ints;
  ints["3"] = &IntTestClass::Int3;
  ints["5"] = &IntTestClass::Int5;

  IntTestClass test;
  EXPECT_EQ((test.*ints["3"])() + (test.*ints["5"])(), 8);
}

TEST(TestProperty, OffsetOfTest)
{
  using namespace Property::Impl;
  EXPECT_EQ(offset_of<OffsetOfTest>(&OffsetOfTest::j),
            offsetof(OffsetOfTest, j));
  EXPECT_EQ(offset_of<OffsetOfTest>(&OffsetOfTest::i),
            offsetof(OffsetOfTest, i));
}

TEST(TestProperty, PropertyTest)
{
  PropertyTest<0> x;
  PropertyTest<0> y;
  x.prop1 = 5;
  y       = x;
  // sizeof PropertyTest
  EXPECT_EQ(sizeof(x), 8);
  EXPECT_EQ(x.prop1, 5);
  EXPECT_EQ(y.prop2, 5);
  // prop1 offset
  EXPECT_EQ(Property::Impl::offset_of_property<decltype(x)>(&x.prop1), 4);
  // prop2 offset
  EXPECT_EQ(Property::Impl::offset_of_property<decltype(x)>(&x.prop2), 5);
}

TEST(TestProperty, PropertyWithStorageTest)
{
  PropertyWithStorageTest<int> x;
  x.prop1 = 5;
  auto y  = x;
  // sizeof PropertyWithStorageTest
  EXPECT_EQ(sizeof(x), 4);
  EXPECT_EQ(x.prop1, 5);
  EXPECT_EQ(y.prop1, 5);
}

TEST(TestProperty, SizeOf)
{
  MyClass a;
  a.property = "5";
  EXPECT_EQ(a.property + 3, 8);
  EXPECT_EQ(sizeof(a), sizeof(long));
}
