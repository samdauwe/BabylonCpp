#include <babylon/meshes/simplification/reference.h>

namespace BABYLON {

Reference::Reference(int _vertexId, int _triangleId)
    : vertexId{_vertexId}, triangleId{_triangleId}
{
}

Reference::~Reference() = default;

} // end of namespace BABYLON
