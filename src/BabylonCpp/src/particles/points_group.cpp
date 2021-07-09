#include <babylon/particles/points_group.h>

namespace BABYLON {

PointsGroup::PointsGroup(
  size_t id, const std::function<void(CloudPoint* particle, size_t i, size_t s)>& posFunction)
    : groupID{this, &PointsGroup::get_groupID, &PointsGroup::set_groupID}, _textureNb{0}
{
  groupId           = id;
  _positionFunction = posFunction;
}

PointsGroup::~PointsGroup() = default;

int PointsGroup::get_groupID() const
{
  return groupId;
}

void PointsGroup::set_groupID(int newGroupID)
{
  groupId = newGroupID;
}

} // end of namespace BABYLON
