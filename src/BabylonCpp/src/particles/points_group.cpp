#include <babylon/particles/points_group.h>

namespace BABYLON {

PointsGroup::PointsGroup(
  size_t id, const std::function<void(CloudPoint* particle, size_t i, size_t s)>& posFunction)
    : _textureNb{0}
{
  groupID           = id;
  _positionFunction = posFunction;
}

PointsGroup::~PointsGroup() = default;

} // end of namespace BABYLON
