#include <babylon/mesh/geometryprimitives/ribbon.h>

#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace GeometryPrimitives {

Ribbon::Ribbon(const string_t& _id, Scene* scene,
               const vector_t<vector_t<Vector3>>& _pathArray, bool _closeArray,
               bool _closePath, int _offset, bool canBeRegenerated, Mesh* mesh,
               unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , pathArray{_pathArray}
    , closeArray{_closeArray}
    , closePath{_closePath}
    , offset{_offset}
    , side{_side}
{
}

Ribbon::~Ribbon()
{
}

unique_ptr_t<VertexData> Ribbon::_regenerateVertexData()
{
  RibbonOptions options(pathArray, offset);
  options.closeArray      = closeArray;
  options.closePath       = closePath;
  options.sideOrientation = side;

  return VertexData::CreateRibbon(options);
}

Geometry* Ribbon::copy(const string_t& _id)
{
  return Ribbon::New(_id, getScene(), pathArray, closeArray, closePath, offset,
                     canBeRegenerated(), nullptr, side);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
