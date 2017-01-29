#include <babylon/mesh/geometryprimitives/plane.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

Plane::Plane(const std::string& _id, Scene* scene, float _size,
             bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}, size{_size}, side{_side}
{
}

Plane::~Plane()
{
}

std::unique_ptr<VertexData> Plane::_regenerateVertexData()
{
  PlaneOptions options(size);
  options.sideOrientation = side;

  return VertexData::CreatePlane(options);
}

Geometry* Plane::copy(const std::string& _id)
{
  return new Plane(_id, getScene(), size, canBeRegenerated(), nullptr, side);
}

Plane* Plane::Parse(const Json::value& parsedPlane, Scene* scene)
{
  const auto parsedPlaneId = Json::GetString(parsedPlane, "id");
  if (parsedPlaneId.empty() || scene->getGeometryByID(parsedPlaneId)) {
    return nullptr; // null since geometry could be something else than a
                    // plane...
  }

  return Plane::New(
    parsedPlaneId, scene, Json::GetNumber(parsedPlane, "size", 1.f),
    Json::GetBool(parsedPlane, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
