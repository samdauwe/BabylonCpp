#include <babylon/mesh/primitivegeometries/torus_knot_geometry.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

TorusKnotGeometry::TorusKnotGeometry(const string_t& _id, Scene* scene,
                                     float _radius, float _tube,
                                     unsigned int _radialSegments,
                                     unsigned int _tubularSegments, float _p,
                                     float _q, bool canBeRegenerated,
                                     Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , radius{_radius}
    , tube{_tube}
    , radialSegments{_radialSegments}
    , tubularSegments{_tubularSegments}
    , p{_p}
    , q{_q}
    , side{_side}
{
}

TorusKnotGeometry::~TorusKnotGeometry()
{
}

unique_ptr_t<VertexData> TorusKnotGeometry::_regenerateVertexData()
{
  TorusKnotOptions options;
  options.radius          = radius;
  options.tube            = tube;
  options.radialSegments  = radialSegments;
  options.tubularSegments = tubularSegments;
  options.p               = p;
  options.q               = q;
  options.sideOrientation = side;

  return VertexData::CreateTorusKnot(options);
}

GeometryPtr TorusKnotGeometry::copy(const string_t& _id)
{
  return TorusKnotGeometry::New(_id, getScene(), radius, tube, radialSegments,
                                tubularSegments, p, q, canBeRegenerated(),
                                nullptr, side);
}

Json::object TorusKnotGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["radius"] = picojson::value(static_cast<double>(radius));
  serializationObject["tube"]   = picojson::value(static_cast<double>(tube));
  serializationObject["radialSegments"]
    = picojson::value(static_cast<double>(radialSegments));
  serializationObject["tubularSegments"]
    = picojson::value(static_cast<double>(tubularSegments));
  serializationObject["p"] = picojson::value(static_cast<double>(p));
  serializationObject["q"] = picojson::value(static_cast<double>(q));

  return serializationObject;
}

TorusKnotGeometryPtr
TorusKnotGeometry::Parse(const Json::value& parsedTorusKnot, Scene* scene)
{
  const auto parsedTorusKnotId = Json::GetString(parsedTorusKnot, "id");
  if (parsedTorusKnotId.empty() || scene->getGeometryByID(parsedTorusKnotId)) {
    return nullptr; // null since geometry could be something else than a torus
                    // knot...
  }

  return TorusKnotGeometry::New(
    parsedTorusKnotId, scene, Json::GetNumber(parsedTorusKnot, "radius", 2.f),
    Json::GetNumber(parsedTorusKnot, "tube", 0.5f),
    Json::GetNumber(parsedTorusKnot, "radialSegments", 32u),
    Json::GetNumber(parsedTorusKnot, "tubularSegments", 32u),
    Json::GetNumber(parsedTorusKnot, "p", 2.f),
    Json::GetNumber(parsedTorusKnot, "q", 3.f),
    Json::GetBool(parsedTorusKnot, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
