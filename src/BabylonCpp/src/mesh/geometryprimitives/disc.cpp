#include <babylon/mesh/geometryprimitives/disc.h>

#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace GeometryPrimitives {

Disc::Disc(const std::string& _id, Scene* scene, float _radius,
           unsigned int _tessellation, bool canBeRegenerated, Mesh* mesh,
           unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , radius{_radius}
    , tessellation{_tessellation}
    , side{_side}
{
}

Disc::~Disc()
{
}

std::unique_ptr<VertexData> Disc::_regenerateVertexData()
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = side;

  return VertexData::CreateDisc(options);
}

Geometry* Disc::copy(const std::string& _id)
{
  return new Disc(_id, getScene(), radius, tessellation, canBeRegenerated(),
                  nullptr, side);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
