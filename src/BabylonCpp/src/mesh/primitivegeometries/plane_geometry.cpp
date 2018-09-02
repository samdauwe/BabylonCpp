#include <babylon/mesh/primitivegeometries/plane_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

PlaneGeometry::PlaneGeometry(const string_t& _id, Scene* scene, float _size,
                             bool canBeRegenerated, Mesh* mesh,
                             unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , size{_size}
    , side{_side}
{
}

PlaneGeometry::~PlaneGeometry()
{
}

unique_ptr_t<VertexData> PlaneGeometry::_regenerateVertexData()
{
  PlaneOptions options(size);
  options.sideOrientation = side;

  return VertexData::CreatePlane(options);
}

GeometryPtr PlaneGeometry::copy(const string_t& _id)
{
  return PlaneGeometry::New(_id, getScene(), size, canBeRegenerated(), nullptr,
                            side);
}

Json::object PlaneGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["size"] = picojson::value(static_cast<double>(size));

  return serializationObject;
}

PlaneGeometryPtr PlaneGeometry::Parse(const Json::value& parsedPlane,
                                      Scene* scene)
{
  const auto parsedPlaneId = Json::GetString(parsedPlane, "id");
  if (parsedPlaneId.empty() || scene->getGeometryByID(parsedPlaneId)) {
    return nullptr; // null since geometry could be something else than a
                    // plane...
  }

  return PlaneGeometry::New(
    parsedPlaneId, scene, Json::GetNumber(parsedPlane, "size", 1.f),
    Json::GetBool(parsedPlane, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
