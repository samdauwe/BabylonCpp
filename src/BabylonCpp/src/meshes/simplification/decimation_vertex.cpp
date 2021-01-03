#include <babylon/meshes/simplification/decimation_vertex.h>

namespace BABYLON {

DecimationVertex::DecimationVertex(const Vector3& _position, int _id) : position{_position}, id{_id}
{
  isBorder        = true;
  triangleCount   = 0;
  triangleStart   = 0;
  originalOffsets = {};
}
DecimationVertex::~DecimationVertex() = default;

void DecimationVertex::updatePosition(const Vector3& newPosition)
{
  position.copyFrom(newPosition);
}

} // end of namespace BABYLON
