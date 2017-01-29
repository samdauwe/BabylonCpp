#include <babylon/mesh/geometryprimitives/sphere.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

Sphere::Sphere(const std::string& _id, Scene* scene, unsigned int _segments,
               float _diameter, bool canBeRegenerated, Mesh* mesh,
               unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , segments{_segments}
    , diameter{_diameter}
    , side{_side}
{
}

Sphere::~Sphere()
{
}

std::unique_ptr<VertexData> Sphere::_regenerateVertexData()
{
  SphereOptions options(diameter);
  options.segments        = segments;
  options.sideOrientation = side;

  return VertexData::CreateSphere(options);
}

Geometry* Sphere::copy(const std::string& _id)
{
  return new Sphere(_id, getScene(), segments, diameter, canBeRegenerated(),
                    nullptr, side);
}

Sphere* Sphere::Parse(const Json::value& parsedSphere, Scene* scene)
{
  const auto parsedSphereId = Json::GetString(parsedSphere, "id");
  if (parsedSphereId.empty() || scene->getGeometryByID(parsedSphereId)) {
    return nullptr; // null since geometry could be something else than a
                    // sphere...
  }

  return Sphere::New(
    parsedSphereId, scene, Json::GetNumber(parsedSphere, "segments", 32u),
    Json::GetNumber(parsedSphere, "diameter", 1.f),
    Json::GetBool(parsedSphere, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
