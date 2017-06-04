#ifndef BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_SYSTEM_H
#define BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_SYSTEM_H

#include <cstddef>
#include <vector>

#include <babylon/extensions/entitycomponentsystem/filter_options.h>

#include <babylon/extensions/entitycomponentsystem/detail/base_system.h>
#include <babylon/extensions/entitycomponentsystem/detail/class_type_id.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {

/// \brief A class that follows the CRTP pattern, used to define custom systems
/// \tparam T The System you are defining
/// \tparam RequirementList
///
/// This class uses the CRTP pattern to make a unique identifier for each system
/// class
///
/// \see BaseSystem
/// If you wish to store systems generically and for further documentation.
///
/// \author Miguel Martin
template <class RequireList, class ExcludeList = Excludes<>>
class System : public detail::BaseSystem {
public:
  /// Default constructor
  System() : BaseSystem{detail::MakeFilter<RequireList, ExcludeList>()}
  {
  }
};

template <class T>
detail::TypeId SystemTypeId()
{
  return detail::ClassTypeId<detail::BaseSystem>::GetTypeId<T>();
}

} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON

#endif // BABYLON_EXTENSIONS_ENTITY_COMPONENT_SYSTEM_SYSTEM_H
