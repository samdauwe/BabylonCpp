#include <babylon/meshes/simplification/decimation_triangle.h>

namespace BABYLON {

DecimationTriangle::DecimationTriangle(const std::vector<DecimationVertex>& iVertices)
    : vertices{iVertices}
{
  deleted       = false;
  isDirty       = false;
  deletePending = false;
  borderFactor  = 0;
}

DecimationTriangle::~DecimationTriangle() = default;

} // end of namespace BABYLON
