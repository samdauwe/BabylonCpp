#include <babylon/mesh/simplification/decimation_triangle.h>

namespace BABYLON {

DecimationTriangle::DecimationTriangle(
  const std::vector<DecimationVertex>& _vertices)
    : vertices{_vertices}
{
}

DecimationTriangle::~DecimationTriangle()
{
}

} // end of namespace BABYLON