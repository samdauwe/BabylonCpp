#include <babylon/extensions/entitycomponentsystem/detail/filter.h>

namespace BABYLON {
namespace Extensions {
namespace ECS {
namespace detail {

bool Filter::doesPassFilter(const ComponentTypeList& componentTypeList) const
{
  for (std::size_t i = 0; i < m_requires.size(); ++i) {
    if (m_requires[i] == true && componentTypeList[i] == false) {
      return false;
    }
  }

  if ((m_excludes & componentTypeList).any()) {
    return false;
  }

  return true;
}

} // end of namespace detail
} // end of namespace ECS
} // end of namespace Extensions
} // end of namespace BABYLON
