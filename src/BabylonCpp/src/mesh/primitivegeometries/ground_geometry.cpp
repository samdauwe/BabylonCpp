#include <babylon/mesh/primitivegeometries/ground_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

GroundGeometry::GroundGeometry(const string_t& _id, Scene* scene,
                               unsigned int _width, unsigned int _height,
                               unsigned int _subdivisions,
                               bool canBeRegenerated, Mesh* mesh)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , width{_width}
    , height{_height}
    , subdivisions{_subdivisions}
{
}

GroundGeometry::~GroundGeometry()
{
}

unique_ptr_t<VertexData> GroundGeometry::_regenerateVertexData()
{
  GroundOptions options(subdivisions);
  options.width  = width;
  options.height = height;

  return VertexData::CreateGround(options);
}

GeometryPtr GroundGeometry::copy(const string_t& _id)
{
  return GroundGeometry::New(_id, getScene(), width, height, subdivisions,
                             canBeRegenerated(), nullptr);
}

Json::object GroundGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["width"]  = picojson::value(static_cast<double>(width));
  serializationObject["height"] = picojson::value(static_cast<double>(height));
  serializationObject["subdivisions"]
    = picojson::value(static_cast<double>(subdivisions));

  return serializationObject;
}

GroundGeometryPtr GroundGeometry::Parse(const Json::value& parsedGround,
                                        Scene* scene)
{
  const auto parsedGroundId = Json::GetString(parsedGround, "id");
  if (parsedGroundId.empty() || scene->getGeometryByID(parsedGroundId)) {
    return nullptr; // null since geometry could be something else than a
                    // ground...
  }

  return GroundGeometry::New(
    parsedGroundId, scene, Json::GetNumber(parsedGround, "width", 1u),
    Json::GetNumber(parsedGround, "height", 1u),
    Json::GetNumber(parsedGround, "subdivisions", 1u),
    Json::GetBool(parsedGround, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
