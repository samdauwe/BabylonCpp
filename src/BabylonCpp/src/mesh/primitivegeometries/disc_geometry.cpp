#include <babylon/mesh/primitivegeometries/disc_geometry.h>

#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {

DiscGeometry::DiscGeometry(const std::string& _id, Scene* scene, float _radius,
                           unsigned int _tessellation, bool canBeRegenerated,
                           Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , radius{_radius}
    , tessellation{_tessellation}
    , side{_side}
{
}

DiscGeometry::~DiscGeometry()
{
}

std::unique_ptr<VertexData> DiscGeometry::_regenerateVertexData()
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = side;

  return VertexData::CreateDisc(options);
}

GeometryPtr DiscGeometry::copy(const std::string& _id)
{
  return DiscGeometry::New(_id, getScene(), radius, tessellation,
                           canBeRegenerated(), nullptr, side);
}

} // end of namespace BABYLON
