#include <gtest/gtest.h>

#include <babylon/extensions/entitycomponentsystem/filter_options.h>

#include "components.h"

constexpr const unsigned MAXIMUM_AMOUNT_OF_COMPONENT_TYPES_TO_TEST = 4;

using namespace BABYLON::Extensions::ECS;
using namespace BABYLON::Extensions::ECS::detail;

namespace {
// I don't think this is possible with functions
template <class... Types>
struct Assigner {
  void operator()(ComponentTypeList& list) const;
};

template <class Type>
struct Assigner<Type> {
  void operator()(ComponentTypeList& list) const
  {
    list[ComponentTypeId<Type>()] = true;
  }
};

template <class Type, class... Types>
struct Assigner<Type, Types...> {
  void operator()(ComponentTypeList& list) const
  {
    Assigner<Type>()(list);
    Assigner<Types...>()(list);
  }
};

template <>
struct Assigner<> {
  void operator()(ComponentTypeList& /*list*/) const
  {
  }
};

template <class... Types>
ComponentTypeList createTypeList()
{
  ComponentTypeList temp(MAXIMUM_AMOUNT_OF_COMPONENT_TYPES_TO_TEST);
  for (size_t i = 0; i < MAXIMUM_AMOUNT_OF_COMPONENT_TYPES_TO_TEST; ++i)
    temp[i]     = false;
  Assigner<Types...>()(temp);
  return temp;
}
}

// All possible test cases:
// ========================
// 1. requires
//      - Test for:
//          => filter should fail
//          => filter should pass
// 2 excludes
//      - Test for:
//          => filter should fail
//          => filter should pass
// 3 requires and excludes
//      - Test for:
//          => filter should pass
//          => filter should fail via require
//          => filter should fail via excludes
//          => filter should fail via require and excludes
//

TEST(TestComponentFilter, Requires_pass)
{
  auto filter
    = MakeFilter<Requires<PositionComponent, VelocityComponent>, Excludes<>>();
  auto typeList = createTypeList<PositionComponent, VelocityComponent>();

  EXPECT_TRUE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Requires_fail)
{
  auto filter
    = MakeFilter<Requires<PositionComponent, VelocityComponent>, Excludes<>>();
  auto typeList = createTypeList<PlayerComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Excludes_pass)
{
  auto filter
    = MakeFilter<Requires<>, Excludes<PositionComponent, VelocityComponent>>();
  auto typeList = createTypeList();

  EXPECT_TRUE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Excludes_fail_via_one)
{
  auto filter
    = MakeFilter<Requires<>, Excludes<PositionComponent, VelocityComponent>>();
  auto typeList = createTypeList<PositionComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Excludes_fail_via_all)
{
  auto filter
    = MakeFilter<Requires<>, Excludes<PositionComponent, VelocityComponent>>();
  auto typeList = createTypeList<PlayerComponent, VelocityComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Requires_and_excludes_pass)
{
  auto filter = MakeFilter<Requires<PositionComponent, VelocityComponent>,
                           Excludes<PlayerComponent>>();
  auto typeList = createTypeList<PositionComponent, VelocityComponent>();

  EXPECT_TRUE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Requires_and_excludes_fail_via_requires)
{
  auto filter = MakeFilter<Requires<PositionComponent, VelocityComponent>,
                           Excludes<PlayerComponent>>();
  auto typeList = createTypeList<VelocityComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Requires_and_excludes_fail_via_excludes)
{
  auto filter = MakeFilter<Requires<PositionComponent, VelocityComponent>,
                           Excludes<PlayerComponent>>();
  auto typeList
    = createTypeList<VelocityComponent, PositionComponent, PlayerComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}

TEST(TestComponentFilter, Requires_and_excludes_fail_via_both)
{
  auto filter = MakeFilter<Requires<PositionComponent, VelocityComponent>,
                           Excludes<PlayerComponent>>();
  auto typeList = createTypeList<PlayerComponent>();

  EXPECT_FALSE(filter.doesPassFilter(typeList));
}
