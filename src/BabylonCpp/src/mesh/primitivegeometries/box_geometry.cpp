#include <babylon/mesh/primitivegeometries/box_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

BoxGeometry::BoxGeometry(const std::string& _id, Scene* scene, float _size,
                         bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , size{_size}
    , side{_side}
{
}

BoxGeometry::~BoxGeometry()
{
}

std::unique_ptr<VertexData> BoxGeometry::_regenerateVertexData()
{
  BoxOptions options(size);
  options.sideOrientation = side;

  return VertexData::CreateBox(options);
}

GeometryPtr BoxGeometry::copy(const std::string& _id)
{
  return BoxGeometry::New(_id, getScene(), size, canBeRegenerated(), nullptr,
                          side);
}

json BoxGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["size"] = size;

  return serializationObject;
}

BoxGeometryPtr BoxGeometry::Parse(const json& parsedBox, Scene* scene)
{
  const auto parsedBoxId = json_util::get_string(parsedBox, "id");
  if (parsedBoxId.empty() || scene->getGeometryByID(parsedBoxId)) {
    return nullptr; // null since geometry could be something else than a box...
  }

  return BoxGeometry::New(
    parsedBoxId, scene, json_util::get_number(parsedBox, "size", 1.f),
    json_util::get_bool(parsedBox, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
