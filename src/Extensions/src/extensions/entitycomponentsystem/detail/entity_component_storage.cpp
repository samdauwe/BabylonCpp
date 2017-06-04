#include <babylon/extensions/entitycomponentsystem/detail/entity_component_storage.h>

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>
#include <babylon/extensions/entitycomponentsystem/util/container_utils.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

EntityComponentStorage::EntityComponentStorage(std::size_t entityAmount)
    : m_componentEntries(entityAmount)
{
}

void EntityComponentStorage::addComponent(Entity& entity, Component* component,
                                          TypeId componentTypeId)
{
  ANAX_ASSERT(entity.isValid(),
              "invalid entity cannot have components added to it");

  auto index                   = entity.getId().index;
  auto& componentDataForEntity = m_componentEntries[index];

  componentDataForEntity.components[componentTypeId].reset(component);
  componentDataForEntity.componentTypeList[componentTypeId] = true;
}

void EntityComponentStorage::removeComponent(Entity& entity,
                                             TypeId componentTypeId)
{
  ANAX_ASSERT(entity.isValid(), "invalid entity cannot remove components");

  auto index                   = entity.getId().index;
  auto& componentDataForEntity = m_componentEntries[index];

  componentDataForEntity.components[componentTypeId].reset();
  componentDataForEntity.componentTypeList[componentTypeId] = false;
}

void EntityComponentStorage::removeAllComponents(Entity& entity)
{
  auto index                   = entity.getId().index;
  auto& componentDataForEntity = m_componentEntries[index];

  for (auto& c : componentDataForEntity.components)
    c.reset();
  componentDataForEntity.componentTypeList.reset();
}

Component& EntityComponentStorage::getComponent(const Entity& entity,
                                                TypeId componentTypeId) const
{
  ANAX_ASSERT(entity.isValid() && hasComponent(entity, componentTypeId),
              "Entity is not valid or does not contain component");

  return *getComponentsImpl(entity)[componentTypeId];
}

ComponentTypeList
EntityComponentStorage::getComponentTypeList(const Entity& entity) const
{
  ANAX_ASSERT(entity.isValid(),
              "invalid entity cannot retrieve the component list");

  return m_componentEntries[entity.getId().index].componentTypeList;
}

ComponentArray EntityComponentStorage::getComponents(const Entity& entity) const
{
  ANAX_ASSERT(entity.isValid(),
              "invalid entity cannot retrieve components, as it has none");

  auto& componentsToConvert = getComponentsImpl(entity);

  ComponentArray temp;
  temp.reserve(componentsToConvert.size());

  for (auto& i : componentsToConvert)
    temp.emplace_back(i.get());

  return temp;
}

bool EntityComponentStorage::hasComponent(const Entity& entity,
                                          TypeId componentTypeId) const
{
  ANAX_ASSERT(entity.isValid(),
              "invalid entity cannot check if it has components");

  auto& components = getComponentsImpl(entity);

  return components.size() > componentTypeId
         && components[componentTypeId] != nullptr;
}

void EntityComponentStorage::resize(std::size_t entityAmount)
{
  m_componentEntries.resize(entityAmount);
}

void EntityComponentStorage::clear()
{
  m_componentEntries.clear();
}

EntityComponentStorage::ImplComponentArray&
EntityComponentStorage::getComponentsImpl(const Entity& e)
{
  return m_componentEntries[e.getId().index].components;
}

const EntityComponentStorage::ImplComponentArray&
EntityComponentStorage::getComponentsImpl(const Entity& e) const
{
  return m_componentEntries[e.getId().index].components;
}

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON
