#include <babylon/mesh/primitivegeometries/sphere_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SphereGeometry::SphereGeometry(const std::string& _id, Scene* scene,
                               unsigned int _segments, float _diameter,
                               bool canBeRegenerated, Mesh* mesh,
                               unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , segments{_segments}
    , diameter{_diameter}
    , side{_side}
{
}

SphereGeometry::~SphereGeometry()
{
}

std::unique_ptr<VertexData> SphereGeometry::_regenerateVertexData()
{
  SphereOptions options(diameter);
  options.segments        = segments;
  options.sideOrientation = side;

  return VertexData::CreateSphere(options);
}

GeometryPtr SphereGeometry::copy(const std::string& _id)
{
  return SphereGeometry::New(_id, getScene(), segments, diameter,
                             canBeRegenerated(), nullptr, side);
}

json SphereGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["segments"] = segments;
  serializationObject["diameter"] = diameter;

  return serializationObject;
}

SphereGeometryPtr SphereGeometry::Parse(const json& parsedSphere, Scene* scene)
{
  const auto parsedSphereId = json_util::get_string(parsedSphere, "id");
  if (parsedSphereId.empty() || scene->getGeometryByID(parsedSphereId)) {
    return nullptr; // null since geometry could be something else than a
                    // sphere...
  }

  return SphereGeometry::New(
    parsedSphereId, scene, json_util::get_number(parsedSphere, "segments", 32u),
    json_util::get_number(parsedSphere, "diameter", 1.f),
    json_util::get_bool(parsedSphere, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
