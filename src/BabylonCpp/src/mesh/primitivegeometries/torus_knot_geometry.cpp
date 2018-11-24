#include <babylon/mesh/primitivegeometries/torus_knot_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

TorusKnotGeometry::TorusKnotGeometry(const std::string& _id, Scene* scene,
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

std::unique_ptr<VertexData> TorusKnotGeometry::_regenerateVertexData()
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

GeometryPtr TorusKnotGeometry::copy(const std::string& _id)
{
  return TorusKnotGeometry::New(_id, getScene(), radius, tube, radialSegments,
                                tubularSegments, p, q, canBeRegenerated(),
                                nullptr, side);
}

json TorusKnotGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["radius"]          = radius;
  serializationObject["tube"]            = tube;
  serializationObject["radialSegments"]  = radialSegments;
  serializationObject["tubularSegments"] = tubularSegments;
  serializationObject["p"]               = p;
  serializationObject["q"]               = q;

  return serializationObject;
}

TorusKnotGeometryPtr TorusKnotGeometry::Parse(const json& parsedTorusKnot,
                                              Scene* scene)
{
  const auto parsedTorusKnotId = json_util::get_string(parsedTorusKnot, "id");
  if (parsedTorusKnotId.empty() || scene->getGeometryByID(parsedTorusKnotId)) {
    return nullptr; // null since geometry could be something else than a torus
                    // knot...
  }

  return TorusKnotGeometry::New(
    parsedTorusKnotId, scene,
    json_util::get_number(parsedTorusKnot, "radius", 2.f),
    json_util::get_number(parsedTorusKnot, "tube", 0.5f),
    json_util::get_number(parsedTorusKnot, "radialSegments", 32u),
    json_util::get_number(parsedTorusKnot, "tubularSegments", 32u),
    json_util::get_number(parsedTorusKnot, "p", 2.f),
    json_util::get_number(parsedTorusKnot, "q", 3.f),
    json_util::get_bool(parsedTorusKnot, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
