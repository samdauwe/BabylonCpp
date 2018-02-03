#include <babylon/mesh/simplification/decimation_triangle.h>

namespace BABYLON {

DecimationTriangle::DecimationTriangle(
  const vector_t<DecimationVertex>& iVertices)
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
