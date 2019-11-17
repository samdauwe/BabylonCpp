#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <babylon/math/vector2.h>

template <typename T>
class PrimitiveTypeProperty {

public:
  PrimitiveTypeProperty()
      : value{this, &PrimitiveTypeProperty::getValue, &PrimitiveTypeProperty::setValue}
  {
  }

public:
  BABYLON::Property<PrimitiveTypeProperty, T> value;

private:
  T _value = {};

  [[nodiscard]] T getValue() const
  {
    return _value;
  }

  void setValue(T newValue)
  {
    _value = newValue;
  }
};

template <typename T>
class ClassTypeProperty {

public:
  ClassTypeProperty()
      : readOnly{this, &ClassTypeProperty::getValue}
      , readWrite{this, &ClassTypeProperty::getValue, &ClassTypeProperty::setValue}
  {
  }

public:
  BABYLON::ReadOnlyProperty<ClassTypeProperty, T> readOnly;
  BABYLON::Property<ClassTypeProperty, T> readWrite;

private:
  T _value;

  T& getValue()
  {
    return _value;
  }

  void setValue(const T& newValue)
  {
    _value = newValue;
  }
};

TEST(TestProperties, ReadOnlyProperty)
{
  ClassTypeProperty<BABYLON::Vector2> a;
  a.readWrite = 4;
  EXPECT_FLOAT_EQ(a.readOnly().x, 4.f);
}

TEST(TestProperties, ReadWritePrimitiveProperty)
{
  // Integer types
  {
    PrimitiveTypeProperty<int> a;
    a.value = 5;
    EXPECT_EQ(a.value, 5);
    // Get by value is only supported mode
    int b = a.value; // NOLINT
    b     = 6;
    EXPECT_EQ(a.value, 5);
    EXPECT_EQ(b, 6);
  }
  // String types
  {
    PrimitiveTypeProperty<std::string> a;
    a.value = "a";
    EXPECT_EQ(a.value(), "a");
    // Get by value is only supported mode
    std::string b = a.value;
    b             = "b";
    EXPECT_EQ(a.value(), "a");
    EXPECT_EQ(b, "b");
  }
}

TEST(TestProperties, ReadWriteClassProperty)
{
  ClassTypeProperty<BABYLON::Vector2> a;
  a.readWrite = BABYLON::Vector2(1, 2);
  EXPECT_FLOAT_EQ(a.readWrite().x, 1.f);
  EXPECT_FLOAT_EQ(a.readWrite().y, 2.f);
  // Get by reference
  auto& v1 = a.readWrite();
  EXPECT_FLOAT_EQ(v1.x, 1.f);
  EXPECT_FLOAT_EQ(v1.y, 2.f);
  v1.x = 2.f;
  EXPECT_FLOAT_EQ(v1.x, 2.f);
  EXPECT_FLOAT_EQ(v1.y, 2.f);
  // Get by value
  auto v2 = a.readWrite();
  EXPECT_FLOAT_EQ(v1.x, 2.f);
  EXPECT_FLOAT_EQ(v1.y, 2.f);
  v2.x = 3.f;
  EXPECT_FLOAT_EQ(v1.x, 2.f);
  EXPECT_FLOAT_EQ(v1.y, 2.f);
}
