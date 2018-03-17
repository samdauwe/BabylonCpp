#include <babylon/mesh/primitivegeometries/cylinder_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

CylinderGeometry::CylinderGeometry(
  const string_t& _id, Scene* scene, float _height, float _diameterTop,
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

unique_ptr_t<VertexData> CylinderGeometry::_regenerateVertexData()
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

Geometry* CylinderGeometry::copy(const string_t& _id)
{
  return CylinderGeometry::New(_id, getScene(), height, diameterTop,
                               diameterBottom, tessellation, subdivisions,
                               canBeRegenerated(), nullptr, side);
}

Json::object CylinderGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["height"] = picojson::value(static_cast<double>(height));
  serializationObject["diameterTop"]
    = picojson::value(static_cast<double>(diameterTop));
  serializationObject["diameterBottom"]
    = picojson::value(static_cast<double>(diameterBottom));
  serializationObject["tessellation"]
    = picojson::value(static_cast<double>(tessellation));

  return serializationObject;
}

CylinderGeometry* CylinderGeometry::Parse(const Json::value& parsedCylinder,
                                          Scene* scene)
{
  const auto parsedCylinderId = Json::GetString(parsedCylinder, "id");
  if (parsedCylinderId.empty() || scene->getGeometryByID(parsedCylinderId)) {
    return nullptr; // null since geometry could be something else than a
                    // cylinder...
  }

  return CylinderGeometry::New(
    parsedCylinderId, scene, Json::GetNumber(parsedCylinder, "height", 2.f),
    Json::GetNumber(parsedCylinder, "diameterTop", 1.f),
    Json::GetNumber(parsedCylinder, "diameterBottom", 1.f),
    Json::GetNumber(parsedCylinder, "tessellation", 24u),
    Json::GetNumber(parsedCylinder, "subdivisions", 1u),
    Json::GetBool(parsedCylinder, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
