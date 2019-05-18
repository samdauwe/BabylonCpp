#include <babylon/meshes/simplification/decimation_triangle.h>

namespace BABYLON {

DecimationTriangle::DecimationTriangle(
  const std::vector<DecimationVertex>& iVertices)
    : deleted{false}
    , isDirty{false}
    , borderFactor{0}
    , deletePending{false}
    , vertices{iVertices}
{
}

DecimationTriangle::~DecimationTriangle()
{
}

} // end of namespace BABYLON
