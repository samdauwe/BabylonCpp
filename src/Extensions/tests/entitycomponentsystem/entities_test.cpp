#include <gtest/gtest.h>

#include <algorithm>

#define ANAX_TEST_CASE_BUILD

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>
#include <babylon/extensions/entitycomponentsystem/entity.h>
#include <babylon/extensions/entitycomponentsystem/world.h>

#include "components.h"
#include "systems.h"

using namespace BABYLON::Extensions::ECS;

// Since entities are heavily tied to a World, we are essentially also testing
// the world.
//
// Here are the possible test cases we need to test for:
//
// 1. Valid/invalid entity handles
//      ✓ New entity => should be valid
//      ✓ Kill new entity => should be invalid
//      ✓ Clearing world => should be invalid
//      ✓ Copy of entity => should both be valid
//      ✓ Copy entity and:
//          ✓ kill each (original, copy and both) => both should be invalid
//          ✓ Clearing world => both should be invalid
//      ✓ Duplicate an already invalid entity => copy should be invalid
// 3. De/activating entities
//      ✓ New entity => should be deactivated
//      - Duplicate entities: each copy (separate case) => both should be valid
//      ✓ Activate entity => is activated?
//      ✓ Activate -> Deactivate => is de-activated
//          * where appropriate (before/after world refresh)?
//      ✓ Activate -> Deactivate -> Activate => is activated?
//          * where appropriate
//      ✓ Activate entity multiple times
//      ✓ Deactivate entity multiple times
// 5. Adding and removing components
//      ✓ New entity => are no components added?
//      ✓ Adding a component => does hasComponent return true?
//      ✓ Adding multiple components => does it assert?
//      ✓ Removing a component => does hasComponent return false?
//      ✓ Removing all components => does hasComponent return false?
// 6. Retrieving an entity via index
//      ✓ Invalid index => assertion occurs?
//      ✓  Valid index => appropriate entity returned?
//      ✓ Multiple entities added/removed => appropriate entity returned?

template <class Container>
long countNonNull(const Container& c)
{
  return std::count_if(std::begin(c), std::end(c),
                       [](typename Container::const_reference x) { return x != nullptr; });
}

// Gross, I know, but oh well.
#define activateAndTest(w, e)                                                                      \
  {                                                                                                \
    (e).activate();                                                                                \
    EXPECT_FALSE((e).isActivated());                                                               \
    (w).refresh();                                                                                 \
    EXPECT_TRUE((e).isActivated());                                                                \
  }

#define deactivateAndTest(w, e)                                                                    \
  {                                                                                                \
    EXPECT_TRUE((e).isActivated());                                                                \
    (e).deactivate();                                                                              \
    EXPECT_TRUE((e).isActivated());                                                                \
    (w).refresh();                                                                                 \
    EXPECT_FALSE((e).isActivated());                                                               \
  }

/// Valid/invalid for one unique entity

TEST(TestEntities, Valid_entity_handle_new_entity)
{
  World world;
  auto e = world.createEntity();
  EXPECT_TRUE(e.isValid());
}

TEST(TestEntities, Invalid_entity_handles_killing_entities)
{
  World world;

  auto e = world.createEntity();
  e.kill();

  // should only be invalid after a refresh
  EXPECT_TRUE(e.isValid());
  world.refresh();
  EXPECT_FALSE(e.isValid());
}

TEST(TestEntities, Invalid_entity_handle_clearing_world)
{
  World world;

  auto e = world.createEntity();
  world.clear();

  EXPECT_FALSE(e.isValid());
}

/// Valid/invalid duplicate handles

TEST(TestEntities, Valid_entity_handle_duplicate_handles)
{
  World world;
  auto e     = world.createEntity();
  auto eCopy = e;

  EXPECT_TRUE(e.isValid());
  EXPECT_TRUE(eCopy.isValid());
}

TEST(TestEntities, Invalid_entity_handles_duplicate_and_kill_original)
{
  World world;
  auto e     = world.createEntity();
  auto eCopy = e;

  e.kill();

  EXPECT_TRUE(e.isValid());
  EXPECT_TRUE(eCopy.isValid());
  world.refresh();
  EXPECT_FALSE(e.isValid());
  EXPECT_FALSE(eCopy.isValid());
}

TEST(TestEntities, Invalid_entity_handles_duplicate_and_kill_clone)
{
  World world;
  auto e     = world.createEntity();
  auto eCopy = e;

  eCopy.kill();

  EXPECT_TRUE(e.isValid());
  EXPECT_TRUE(eCopy.isValid());
  world.refresh();
  EXPECT_FALSE(e.isValid());
  EXPECT_FALSE(eCopy.isValid());
}

TEST(TestEntities, Invalid_entity_handles_duplicate_and_kill_both)
{
  World world;
  auto e     = world.createEntity();
  auto eCopy = e;

  e.kill();
  eCopy.kill();

  EXPECT_TRUE(e.isValid());
  EXPECT_TRUE(eCopy.isValid());
  world.refresh();
  EXPECT_FALSE(e.isValid());
  EXPECT_FALSE(eCopy.isValid());
}

TEST(TestEntities, Invalid_entity_handles_duplicate_and_clear_world)
{
  World world;
  auto e     = world.createEntity();
  auto eCopy = e;

  world.clear();
  EXPECT_FALSE(e.isValid());
  EXPECT_FALSE(eCopy.isValid());
}

TEST(TestEntities, Duplicate_invalid_killed_entity_handle)
{
  World world;

  auto e1 = world.createEntity();
  e1.kill();

  // refresh the world (so that the ID will be invalid)
  world.refresh();
  EXPECT_FALSE(e1.isValid());

  // create another handle, that is the same as the previous
  Entity e2 = e1;

  // this handle should also be invalid
  EXPECT_FALSE(e2.isValid());
}

// Activation/deactivation

TEST(TestEntities, isActivated_new_entity_false)
{
  World world;

  auto e = world.createEntity();
  EXPECT_FALSE(e.isActivated());
}

TEST(TestEntities, Activate)
{
  World world;
  auto e = world.createEntity();

  activateAndTest(world, e);
}

TEST(TestEntities, Activate_to_Deactivate)
{
  World world;
  auto e = world.createEntity();

  activateAndTest(world, e);
  deactivateAndTest(world, e);
}

TEST(TestEntities, Activate_to_Deactivate_to_Activate)
{
  World world;
  auto e = world.createEntity();

  activateAndTest(world, e);
  deactivateAndTest(world, e);
  activateAndTest(world, e);
}

TEST(TestEntities, Activating_an_entity_multiple_times)
{
  World world;
  MovementSystem movementSystem;
  world.addSystem(movementSystem);

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();

  auto activate = [&]() {
    for (int i = 0; i < 100; ++i) {
      e.activate();
    }
    world.refresh();
  };

  activate();

  EXPECT_TRUE(e.isActivated());
  EXPECT_EQ(movementSystem.getEntities().size(), 1);

  activate();

  EXPECT_TRUE(e.isActivated());
  EXPECT_EQ(countNonNull(e.getComponents()), 2);
}

// Duplicate code of above (refactor pls)
TEST(TestEntities, De_activating_an_entity_multiple_times)
{
  World world;
  MovementSystem movementSystem;
  world.addSystem(movementSystem);

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();

  e.activate();
  world.refresh();

  EXPECT_TRUE(e.isActivated());
  EXPECT_EQ(movementSystem.getEntities().size(), 1);

  auto deactivate = [&]() {
    for (int i = 0; i < 100; ++i) {
      e.deactivate();
    }
    world.refresh();
  };

  deactivate();

  EXPECT_FALSE(e.isActivated());
  EXPECT_EQ(movementSystem.getEntities().size(), 0);

  deactivate();

  EXPECT_FALSE(e.isActivated());
  EXPECT_EQ(countNonNull(e.getComponents()), 2);
}

TEST(TestEntities, New_entity_no_components_added)
{
  World world;

  auto e = world.createEntity();

  EXPECT_EQ(countNonNull(e.getComponents()), 0);
  EXPECT_FALSE(e.hasComponent<PositionComponent>());
}

TEST(TestEntities, Adding_components)
{
  World world;

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();

  EXPECT_TRUE(e.hasComponent<PositionComponent>() == true);
  EXPECT_NO_THROW(e.getComponent<PositionComponent>());
  EXPECT_EQ(countNonNull(e.getComponents()), 1);
}

TEST(TestEntities, Removing_components)
{
  World world;

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.removeComponent<PositionComponent>();

  EXPECT_FALSE(e.hasComponent<PositionComponent>());
  EXPECT_EQ(countNonNull(e.getComponents()), 0);
}

TEST(TestEntities, Removing_all_components)
{
  World world;

  auto e = world.createEntity();
  e.addComponent<PositionComponent>();
  e.addComponent<VelocityComponent>();

  EXPECT_TRUE(e.hasComponent<PositionComponent>());
  EXPECT_TRUE(e.hasComponent<VelocityComponent>());
  EXPECT_NO_THROW(e.getComponent<PositionComponent>());
  EXPECT_NO_THROW(e.getComponent<VelocityComponent>());

  e.removeAllComponents();

  EXPECT_EQ(countNonNull(e.getComponents()), 0);
  EXPECT_FALSE(e.hasComponent<PositionComponent>());
  EXPECT_FALSE(e.hasComponent<VelocityComponent>());
}

TEST(TestEntities, Retrieving_an_Entity_via_ID_index_VALID_index)
{
  World world;

  auto e1 = world.createEntity();
  auto e2 = world.getEntity(e1.getId().index);

  EXPECT_TRUE(e1.isValid());
  EXPECT_TRUE(e2.isValid());
  EXPECT_EQ(e1, e2);
}

TEST(TestEntities, Retrieving_an_Entity_via_ID_index_VALID_index_multiple_entities_created)
{
  World world;

  auto e1 = world.createEntity();

  world.createEntities(1000);
  for (int i = 0; i < 1000; ++i)
    world.createEntity();

  auto e2 = world.getEntity(e1.getId().index);

  EXPECT_TRUE(e1.isValid());
  EXPECT_TRUE(e2.isValid());
  EXPECT_EQ(e1, e2);
}
