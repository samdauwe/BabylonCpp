#include <babylon/mesh/simplification/decimation_vertex.h>

namespace BABYLON {

DecimationVertex::DecimationVertex(const Vector3& _position, int _id)
    : position{_position},
      id{_id},
      isBorder{true},
      triangleStart{0},
      triangleCount{0}
{
}
DecimationVertex::~DecimationVertex()
{
}

void DecimationVertex::updatePosition(const Vector3& newPosition)
{
  position.copyFrom(newPosition);
}

} // end of namespace BABYLON
