#include <babylon/mesh/primitivegeometries/plane_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

PlaneGeometry::PlaneGeometry(const std::string& _id, Scene* scene, float _size,
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

std::unique_ptr<VertexData> PlaneGeometry::_regenerateVertexData()
{
  PlaneOptions options(size);
  options.sideOrientation = side;

  return VertexData::CreatePlane(options);
}

GeometryPtr PlaneGeometry::copy(const std::string& _id)
{
  return PlaneGeometry::New(_id, getScene(), size, canBeRegenerated(), nullptr,
                            side);
}

json PlaneGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["size"] = size;

  return serializationObject;
}

PlaneGeometryPtr PlaneGeometry::Parse(const json& parsedPlane, Scene* scene)
{
  const auto parsedPlaneId = json_util::get_string(parsedPlane, "id");
  if (parsedPlaneId.empty() || scene->getGeometryByID(parsedPlaneId)) {
    return nullptr; // null since geometry could be something else than a
                    // plane...
  }

  return PlaneGeometry::New(
    parsedPlaneId, scene, json_util::get_number(parsedPlane, "size", 1.f),
    json_util::get_bool(parsedPlane, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
