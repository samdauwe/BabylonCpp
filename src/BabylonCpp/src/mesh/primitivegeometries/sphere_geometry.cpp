#include <babylon/mesh/primitivegeometries/sphere_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SphereGeometry::SphereGeometry(const string_t& _id, Scene* scene,
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

unique_ptr_t<VertexData> SphereGeometry::_regenerateVertexData()
{
  SphereOptions options(diameter);
  options.segments        = segments;
  options.sideOrientation = side;

  return VertexData::CreateSphere(options);
}

GeometryPtr SphereGeometry::copy(const string_t& _id)
{
  return SphereGeometry::New(_id, getScene(), segments, diameter,
                             canBeRegenerated(), nullptr, side);
}

Json::object SphereGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["segments"]
    = picojson::value(static_cast<double>(segments));
  serializationObject["diameter"]
    = picojson::value(static_cast<double>(diameter));

  return serializationObject;
}

SphereGeometryPtr SphereGeometry::Parse(const Json::value& parsedSphere,
                                        Scene* scene)
{
  const auto parsedSphereId = Json::GetString(parsedSphere, "id");
  if (parsedSphereId.empty() || scene->getGeometryByID(parsedSphereId)) {
    return nullptr; // null since geometry could be something else than a
                    // sphere...
  }

  return SphereGeometry::New(
    parsedSphereId, scene, Json::GetNumber(parsedSphere, "segments", 32u),
    Json::GetNumber(parsedSphere, "diameter", 1.f),
    Json::GetBool(parsedSphere, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
