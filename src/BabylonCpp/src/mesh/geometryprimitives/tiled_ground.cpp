#include <babylon/mesh/geometryprimitives/tiled_ground.h>

#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace GeometryPrimitives {

TiledGround::TiledGround(const std::string& _id, Scene* scene, float _xmin,
                         float _zmin, float _xmax, float _zmax,
                         const ISize& _subdivisions, const ISize& _precision,
                         bool canBeRegenerated, Mesh* mesh)

    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , xmin{_xmin}
    , zmin{_zmin}
    , xmax{_xmax}
    , zmax{_zmax}
    , subdivisions{_subdivisions.height, _subdivisions.width}
    , precision{_precision.height, _precision.width}
{
}

TiledGround::~TiledGround()
{
}

std::unique_ptr<VertexData> TiledGround::_regenerateVertexData()
{
  TiledGroundOptions options;
  options.xmin         = xmin;
  options.zmin         = zmin;
  options.xmax         = xmax;
  options.zmax         = zmax;
  options.subdivisions = subdivisions;
  options.precision    = precision;

  return VertexData::CreateTiledGround(options);
}

Geometry* TiledGround::copy(const std::string& _id)
{
  return TiledGround::New(_id, getScene(), xmin, zmin, xmax, zmax, subdivisions,
                          precision, canBeRegenerated(), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
