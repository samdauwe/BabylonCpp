#include <babylon/extensions/entitycomponentsystem/entity.h>

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>
#include <babylon/extensions/entitycomponentsystem/world.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {

Entity::Entity()
{
}

Entity::Entity(World& world, Entity::Id id) : m_id(id), m_world(&world)
{
}

World& Entity::getWorld() const
{
  ANAX_ASSERT(m_world, "world reference in entity is null");

  return *m_world;
}

const Entity::Id& Entity::getId() const
{
  return m_id;
}

bool Entity::isValid() const
{
  // Allows an empty Entity handle to be checked.
  if (m_world == nullptr)
    return false;

  return getWorld().isValid(*this);
}

bool Entity::isActivated() const
{
  return getWorld().isActivated(*this);
}

void Entity::activate()
{
  getWorld().activateEntity(*this);
}

void Entity::deactivate()
{
  getWorld().deactivateEntity(*this);
}

void Entity::kill()
{
  getWorld().killEntity(*this);
}

void Entity::removeAllComponents()
{
  getWorld().m_entityAttributes.componentStorage.removeAllComponents(*this);
}

ComponentArray Entity::getComponents() const
{
  return getWorld().m_entityAttributes.componentStorage.getComponents(*this);
}

detail::ComponentTypeList Entity::getComponentTypeList() const
{
  return getWorld().m_entityAttributes.componentStorage.getComponentTypeList(
    *this);
}

bool Entity::operator==(const Entity& entity) const
{
  return m_id == entity.m_id && entity.m_world == m_world;
}

void Entity::addComponent(Component* component, detail::TypeId componentTypeId)
{
  getWorld().m_entityAttributes.componentStorage.addComponent(*this, component,
                                                              componentTypeId);
}

void Entity::removeComponent(detail::TypeId componentTypeId)
{
  getWorld().m_entityAttributes.componentStorage.removeComponent(
    *this, componentTypeId);
}

Component& Entity::getComponent(detail::TypeId componentTypeId) const
{
  return getWorld().m_entityAttributes.componentStorage.getComponent(
    *this, componentTypeId);
}

bool Entity::hasComponent(detail::TypeId componentTypeId) const
{
  return getWorld().m_entityAttributes.componentStorage.hasComponent(
    *this, componentTypeId);
}

} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON
