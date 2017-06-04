#include <gtest/gtest.h>

#include <sstream>

#define ANAX_TEST_CASE_BUILD

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>
#include <babylon/extensions/entitycomponentsystem/world.h>

using namespace BABYLON::Extensions::ECS;

#include "systems.h"

// Since systems are tightly bound to the world attached to them,
// we will also be testing that the world.
//
// Here are the possible test cases we need to test for:
// 1. Constructing a system
//    ✓  Does the getWorld() function assert?
//    ✓  Are there no entities attached to the System?
// 2. Adding a system
//    ✓ Does the system exist in the world?
//    ✓ Is the system's getWorld() work appropriately?
//    ✓ Are there no entities attached to the System?
//    ✓ Does adding multiple systems of the same type assert?
// 3. Removing a system
//    ✓ Does the getWorld() function assert?
//    ✓ Does the system no longer exist in the world?
//    ✓ Are there no entities attached to the system?
//
TEST(TestSystems, New_system)
{
  MovementSystem system;
  EXPECT_EQ(system.getEntities().size(), 0);
  EXPECT_DEATH(system.getWorld(), "No world attached to system");
}

TEST(TestSystems, Adding_a_system_to_a_world)
{
  World world;
  MovementSystem system;
  world.addSystem(system);

  EXPECT_TRUE(world.doesSystemExist(system));
  EXPECT_EQ(system.getEntities().size(), 0);
  EXPECT_EQ(&system.getWorld(), &world);
}

TEST(TestSystems, Adding_multiple_systems_of_the_same_type)
{
  World world;
  MovementSystem m1;
  MovementSystem m2;

  world.addSystem(m1);

  EXPECT_DEATH(world.addSystem(m2),
               "System of this type is already contained within the world");
}

TEST(TestSystems, Removing_a_system_from_a_world)
{
  World world;
  MovementSystem system;
  world.addSystem(system);

  Entity e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();
  e.activate();

  world.refresh();

  EXPECT_EQ(system.getEntities().size(), 1);

  world.removeSystem<decltype(system)>();

  EXPECT_DEATH(system.getWorld(), "No world attached to system");
  EXPECT_FALSE(world.doesSystemExist(system));
  EXPECT_EQ(system.getEntities().size(), 0);
}

TEST(TestSystems, Removing_a_system_that_is_not_in_the_world)
{
  World world;
  MovementSystem system;

  EXPECT_FALSE(world.doesSystemExist(system));
  EXPECT_DEATH(world.removeSystem<decltype(system)>(),
               "System does not exist in world");
}

TEST(TestSystems, Removing_all_systems_from_the_world)
{
  World world;
  MovementSystem movementSystem;
  PlayerSystem playerSystem;
  world.addSystem(movementSystem);
  world.addSystem(playerSystem);

  world.removeAllSystems();

  EXPECT_FALSE(world.doesSystemExist(movementSystem));
  EXPECT_FALSE(world.doesSystemExist(playerSystem));
}

TEST(TestSystems,
     Attempt_to_add_an_Entity_that_does_confine_to_a_systems_filter)
{
  World world;
  MovementSystem system;
  world.addSystem(system);

  auto e = world.createEntity();

  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();
  e.addComponent<NPCComponent>();
  e.activate();

  world.refresh();
}

TEST(TestSystems,
     Attempt_to_add_an_Entity_that_does_NOT_confine_to_a_system_filter)
{
  World world;
  PlayerSystem system;
  world.addSystem(system);
  auto e = world.createEntity();

  e.addComponent<PlayerComponent>();
  e.addComponent<NPCComponent>();
  e.activate();

  world.refresh();
}

TEST(TestSystems, Removing_components_with_a_system_attached_to_the_world)
{
  World world;
  MovementSystem moveSystem;

  world.addSystem(moveSystem);

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();
  e.activate();

  // make sure we don't get any exceptions
  world.refresh();
  moveSystem.update();

  e.removeComponent<PositionComponent>();
  e.activate();

  EXPECT_FALSE(e.hasComponent<PositionComponent>());

  // make sure we don't get any exceptions (again)
  world.refresh();
  moveSystem.update();
}

TEST(TestSystems, Removing_all_components_with_a_system_attached_to_the_world)
{
  World world;
  MovementSystem moveSystem;

  world.addSystem(moveSystem);

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();

  // make sure we don't get any exceptions
  e.activate();
  world.refresh();

  e.removeAllComponents();
  e.activate();

  EXPECT_FALSE(e.hasComponent<PositionComponent>());
  EXPECT_FALSE(e.hasComponent<VelocityComponent>());

  world.refresh();
  moveSystem.update();
}
