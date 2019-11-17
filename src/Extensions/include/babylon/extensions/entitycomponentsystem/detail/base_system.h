///
/// anax
/// An open source C++ entity system.
///
/// anax
/// An open source C++ entity system.
///
/// Copyright (C) 2013-2014 Miguel Martin (miguel@miguel-martin.com)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///

#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_BASE_SYSTEM_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_BASE_SYSTEM_H

#include <vector>

#include <babylon/babylon_api.h>

#include <babylon/extensions/entitycomponentsystem/detail/filter.h>
#include <babylon/extensions/entitycomponentsystem/entity.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {

class World;

namespace detail {

/// \brief Describes the base class for a systems
///
/// A system is basically a container for entities with
/// a specific amount of components.
///
/// \author Miguel Martin
class BABYLON_SHARED_EXPORT BaseSystem {

public:
  BaseSystem(const Filter& filter);

  /// Destructor
  virtual ~BaseSystem() = 0; // = default

  /// \return The Component Filter attached to the System
  [[nodiscard]] const Filter& getFilter() const;

  /// \return The World attached to the System
  [[nodiscard]] World& getWorld() const;

  /// \return All the entities that are within the System
  [[nodiscard]] const std::vector<Entity>& getEntities() const;

private:
  /// Initializes the system, when a world is successfully attached to it.
  virtual void initialize()
  {
  }

  /// Occurs when an Entity is added to the system
  /// \param entity The Entity that is added to the system
  virtual void onEntityAdded(Entity& /*entity*/)
  {
  }

  /// Occurs when an Entity is removed to the system
  /// \param entity The Entity that is removed from the system
  virtual void onEntityRemoved(Entity& /*entity*/)
  {
  }

  /// Used to add an Entity to the system
  /// \param entity The Entity you wish to add
  /// \note This is called by the attached World object
  void add(Entity& entity);

  /// Used to remove an Entity from the system
  /// \param entity The Entity you wish to remove
  /// \note This is called by the attached World object
  void remove(Entity& entity);

  /// Used to set the attached World
  /// \param world The World to attach to
  /// \note This is called by the attached World object
  void setWorld(World& world);

  /// The World attached to the system
  World* m_world;

  /// The component filter
  Filter m_filter;

  /// The Entities that are attached to this system
  std::vector<Entity> m_entities;

  friend World;

}; // end of class BaseSystem

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_DETAIL_BASE_SYSTEM_H
