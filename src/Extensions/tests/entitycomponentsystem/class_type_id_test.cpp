#include <gtest/gtest.h>

#include <babylon/extensions/entitycomponentsystem/detail/class_type_id.h>

struct Type {
};

TEST(TestClassTypeId, Incremental_IDs_for_types)
{
  using namespace BABYLON::Extensions::ECS::detail;

  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<bool>(), 0);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<char>(), 1);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<unsigned char>(), 2);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<short>(), 3);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<unsigned short>(), 4);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<int>(), 5);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<unsigned int>(), 6);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<float>(), 7);
  EXPECT_EQ(ClassTypeId<Type>::GetTypeId<double>(), 8);
}
