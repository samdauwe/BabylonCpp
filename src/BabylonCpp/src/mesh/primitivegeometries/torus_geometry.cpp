#include <babylon/mesh/primitivegeometries/torus_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

TorusGeometry::TorusGeometry(const string_t& _id, Scene* scene, float _diameter,
                             float _thickness, unsigned int _tessellation,
                             bool canBeRegenerated, Mesh* mesh,
                             unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , diameter{_diameter}
    , thickness{_thickness}
    , tessellation{_tessellation}
    , side{_side}
{
}

TorusGeometry::~TorusGeometry()
{
}

unique_ptr_t<VertexData> TorusGeometry::_regenerateVertexData()
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = side;

  return VertexData::CreateTorus(options);
}

GeometryPtr TorusGeometry::copy(const string_t& _id)
{
  return TorusGeometry::New(_id, getScene(), diameter, thickness, tessellation,
                            canBeRegenerated(), nullptr, side);
}

Json::object TorusGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["diameter"]
    = picojson::value(static_cast<double>(diameter));
  serializationObject["thickness"]
    = picojson::value(static_cast<double>(thickness));
  serializationObject["tessellation"]
    = picojson::value(static_cast<double>(tessellation));

  return serializationObject;
}

TorusGeometryPtr TorusGeometry::Parse(const Json::value& parsedTorus,
                                      Scene* scene)
{
  const auto parsedTorusId = Json::GetString(parsedTorus, "id");
  if (parsedTorusId.empty() || scene->getGeometryByID(parsedTorusId)) {
    return nullptr; // null since geometry could be something else than a
                    // torus...
  }

  return TorusGeometry::New(
    parsedTorusId, scene, Json::GetNumber(parsedTorus, "diameter", 1.f),
    Json::GetNumber(parsedTorus, "thickness", 0.5f),
    Json::GetNumber(parsedTorus, "tessellation", 16u),
    Json::GetBool(parsedTorus, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
