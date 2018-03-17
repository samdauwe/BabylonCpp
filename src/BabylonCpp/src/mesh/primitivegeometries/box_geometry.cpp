#include <babylon/mesh/primitivegeometries/box_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

BoxGeometry::BoxGeometry(const string_t& _id, Scene* scene, float _size,
                         bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , size{_size}
    , side{_side}
{
}

BoxGeometry::~BoxGeometry()
{
}

unique_ptr_t<VertexData> BoxGeometry::_regenerateVertexData()
{
  BoxOptions options(size);
  options.sideOrientation = side;

  return VertexData::CreateBox(options);
}

Geometry* BoxGeometry::copy(const string_t& _id)
{
  return BoxGeometry::New(_id, getScene(), size, canBeRegenerated(), nullptr,
                          side);
}

Json::object BoxGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["size"] = picojson::value(static_cast<double>(size));

  return serializationObject;
}

BoxGeometry* BoxGeometry::Parse(const Json::value& parsedBox, Scene* scene)
{
  const auto parsedBoxId = Json::GetString(parsedBox, "id");
  if (parsedBoxId.empty() || scene->getGeometryByID(parsedBoxId)) {
    return nullptr; // null since geometry could be something else than a box...
  }

  return BoxGeometry::New(
    parsedBoxId, scene, Json::GetNumber(parsedBox, "size", 1.f),
    Json::GetBool(parsedBox, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
