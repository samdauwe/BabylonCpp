#include <babylon/mesh/simplification/reference.h>

namespace BABYLON {

Reference::Reference(int _vertexId, int _triangleId)
    : vertexId{_vertexId}, triangleId{_triangleId}
{
}

Reference::~Reference()
{
}

} // end of namespace BABYLON
