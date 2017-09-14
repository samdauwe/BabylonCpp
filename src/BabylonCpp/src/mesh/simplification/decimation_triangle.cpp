#include <babylon/mesh/simplification/decimation_triangle.h>

namespace BABYLON {

DecimationTriangle::DecimationTriangle(
  const vector_t<DecimationVertex>& _vertices)
    : vertices{_vertices}
{
}

DecimationTriangle::~DecimationTriangle()
{
}

} // end of namespace BABYLON