#include <babylon/lights/light_constants.h>

#include <babylon/lights/isortable_light.h>

namespace BABYLON {

int LightConstants::CompareLightsPriority(const ISortableLight& a, const ISortableLight& b)
{
  // shadow-casting lights have priority over non-shadow-casting lights
  // the renderPriority is a secondary sort criterion
  if (a.shadowEnabled != b.shadowEnabled) {
    return (b.shadowEnabled ? 1 : 0) - (a.shadowEnabled ? 1 : 0);
  }
  return b.renderPriority - a.renderPriority;
}

} // end of namespace BABYLON
