#include <babylon/mesh/primitivegeometries/ground_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

GroundGeometry::GroundGeometry(const std::string& _id, Scene* scene,
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

std::unique_ptr<VertexData> GroundGeometry::_regenerateVertexData()
{
  GroundOptions options(subdivisions);
  options.width  = width;
  options.height = height;

  return VertexData::CreateGround(options);
}

GeometryPtr GroundGeometry::copy(const std::string& _id)
{
  return GroundGeometry::New(_id, getScene(), width, height, subdivisions,
                             canBeRegenerated(), nullptr);
}

json GroundGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["width"]        = width;
  serializationObject["height"]       = height;
  serializationObject["subdivisions"] = subdivisions;

  return serializationObject;
}

GroundGeometryPtr GroundGeometry::Parse(const json& parsedGround, Scene* scene)
{
  const auto parsedGroundId = json_util::get_string(parsedGround, "id");
  if (parsedGroundId.empty() || scene->getGeometryByID(parsedGroundId)) {
    return nullptr; // null since geometry could be something else than a
                    // ground...
  }

  return GroundGeometry::New(
    parsedGroundId, scene, json_util::get_number(parsedGround, "width", 1u),
    json_util::get_number(parsedGround, "height", 1u),
    json_util::get_number(parsedGround, "subdivisions", 1u),
    json_util::get_bool(parsedGround, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
