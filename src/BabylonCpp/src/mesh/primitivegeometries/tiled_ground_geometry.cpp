#include <babylon/mesh/primitivegeometries/tiled_ground_geometry.h>

#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {

TiledGroundGeometry::TiledGroundGeometry(const string_t& _id, Scene* scene,
                                         float _xmin, float _zmin, float _xmax,
                                         float _zmax,
                                         const ISize& _subdivisions,
                                         const ISize& _precision,
                                         bool canBeRegenerated, Mesh* mesh)

    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , xmin{_xmin}
    , zmin{_zmin}
    , xmax{_xmax}
    , zmax{_zmax}
    , subdivisions{_subdivisions.height, _subdivisions.width}
    , precision{_precision.height, _precision.width}
{
}

TiledGroundGeometry::~TiledGroundGeometry()
{
}

unique_ptr_t<VertexData> TiledGroundGeometry::_regenerateVertexData()
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

Geometry* TiledGroundGeometry::copy(const string_t& _id)
{
  return TiledGroundGeometry::New(_id, getScene(), xmin, zmin, xmax, zmax,
                                  subdivisions, precision, canBeRegenerated(),
                                  nullptr);
}

} // end of namespace BABYLON
