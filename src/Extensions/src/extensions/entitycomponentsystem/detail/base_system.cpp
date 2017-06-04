#include <babylon/extensions/entitycomponentsystem/detail/base_system.h>

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>

#include <algorithm>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

BaseSystem::BaseSystem(const Filter& filter)
    : m_world(nullptr), m_filter(filter)
{
}

BaseSystem::~BaseSystem()
{
}

const Filter& BaseSystem::getFilter() const
{
  return m_filter;
}

World& BaseSystem::getWorld() const
{
  ANAX_ASSERT(m_world != nullptr, "No world attached to system");
  return *m_world;
}

const std::vector<Entity>& BaseSystem::getEntities() const
{
  return m_entities;
}

void BaseSystem::add(Entity& entity)
{
  m_entities.push_back(entity);
  onEntityAdded(entity);
}

void BaseSystem::remove(Entity& entity)
{
  m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity),
                   m_entities.end());

  onEntityRemoved(entity);
}

void BaseSystem::setWorld(World& world)
{
  m_world = &world;
  initialize();
}

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON
