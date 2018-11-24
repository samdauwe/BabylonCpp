#include <babylon/mesh/primitivegeometries/cylinder_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

CylinderGeometry::CylinderGeometry(
  const std::string& _id, Scene* scene, float _height, float _diameterTop,
  float _diameterBottom, unsigned int _tessellation, unsigned int _subdivisions,
  bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , height{_height}
    , diameterTop{_diameterTop}
    , diameterBottom{_diameterBottom}
    , tessellation{_tessellation}
    , subdivisions{_subdivisions}
    , side{_side}
{
}

CylinderGeometry::~CylinderGeometry()
{
}

std::unique_ptr<VertexData> CylinderGeometry::_regenerateVertexData()
{
  CylinderOptions options;
  options.height          = height;
  options.diameterTop     = diameterTop;
  options.diameterBottom  = diameterBottom;
  options.tessellation    = tessellation;
  options.subdivisions    = subdivisions;
  options.sideOrientation = side;

  return VertexData::CreateCylinder(options);
}

GeometryPtr CylinderGeometry::copy(const std::string& _id)
{
  return CylinderGeometry::New(_id, getScene(), height, diameterTop,
                               diameterBottom, tessellation, subdivisions,
                               canBeRegenerated(), nullptr, side);
}

json CylinderGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["height"]         = height;
  serializationObject["diameterTop"]    = diameterTop;
  serializationObject["diameterBottom"] = diameterBottom;
  serializationObject["tessellation"]   = tessellation;

  return serializationObject;
}

CylinderGeometryPtr CylinderGeometry::Parse(const json& parsedCylinder,
                                            Scene* scene)
{
  const auto parsedCylinderId = json_util::get_string(parsedCylinder, "id");
  if (parsedCylinderId.empty() || scene->getGeometryByID(parsedCylinderId)) {
    return nullptr; // null since geometry could be something else than a
                    // cylinder...
  }

  return CylinderGeometry::New(
    parsedCylinderId, scene,
    json_util::get_number(parsedCylinder, "height", 2.f),
    json_util::get_number(parsedCylinder, "diameterTop", 1.f),
    json_util::get_number(parsedCylinder, "diameterBottom", 1.f),
    json_util::get_number(parsedCylinder, "tessellation", 24u),
    json_util::get_number(parsedCylinder, "subdivisions", 1u),
    json_util::get_bool(parsedCylinder, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
