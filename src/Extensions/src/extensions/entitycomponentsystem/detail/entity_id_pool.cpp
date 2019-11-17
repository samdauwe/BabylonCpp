#include <babylon/extensions/entitycomponentsystem/detail/entity_id_pool.h>

#include <babylon/extensions/entitycomponentsystem/detail/anax_assert.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

EntityIdPool::EntityIdPool(std::size_t poolSize) : m_nextId(0), m_counts(poolSize)
{
}

Entity::Id EntityIdPool::create()
{
  Entity::Id id;

  // if we need to add more entities to the pool
  if (!m_freeList.empty()) {
    id = m_freeList.back();
    m_freeList.pop_back();
  }
  else {
    id.index = m_nextId++;
    // an ID given out cannot have a counter of 0.
    // 0 is an "invalid" counter, thus we must update
    // the counter within the pool for the corresponding entity
    m_counts[id.index] = id.counter = 1;
  }

  return id;
}

void EntityIdPool::remove(Entity::Id id)
{
  auto& counter = m_counts[id.index];
  ++counter; // increment the counter in the cache
  m_freeList.emplace_back(static_cast<Entity::Id::int_type>(id.index),
                          counter); // add the ID to the freeList
}

Entity::Id EntityIdPool::get(std::size_t index) const
{
  if (index < m_counts.size())
    return Entity::Id{index, m_counts[index]};
  else
    return Entity::Id{index, 0};
}

bool EntityIdPool::isValid(Entity::Id id) const
{
  if (id.index >= m_counts.size())
    return false;
  else
    return (id.counter == m_counts[id.index]) && (id.counter > 0);
}

std::size_t EntityIdPool::getSize() const
{
  return m_counts.size();
}

void EntityIdPool::resize(std::size_t amount)
{
  m_counts.resize(amount);
}

void EntityIdPool::clear()
{
  m_counts.clear();
  m_freeList.clear();
  m_nextId = 0;
}

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON
