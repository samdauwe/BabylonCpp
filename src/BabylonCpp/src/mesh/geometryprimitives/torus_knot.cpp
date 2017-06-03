#include <babylon/mesh/geometryprimitives/torus_knot.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

TorusKnot::TorusKnot(const std::string& _id, Scene* scene, float _radius,
                     float _tube, unsigned int _radialSegments,
                     unsigned int _tubularSegments, float _p, float _q,
                     bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , radius{_radius}
    , tube{_tube}
    , radialSegments{_radialSegments}
    , tubularSegments{_tubularSegments}
    , p{_p}
    , q{_q}
    , side{_side}
{
}

TorusKnot::~TorusKnot()
{
}

std::unique_ptr<VertexData> TorusKnot::_regenerateVertexData()
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

Geometry* TorusKnot::copy(const std::string& _id)
{
  return TorusKnot::New(_id, getScene(), radius, tube, radialSegments,
                        tubularSegments, p, q, canBeRegenerated(), nullptr,
                        side);
}

TorusKnot* TorusKnot::Parse(const Json::value& parsedTorusKnot, Scene* scene)
{
  const auto parsedTorusKnotId = Json::GetString(parsedTorusKnot, "id");
  if (parsedTorusKnotId.empty() || scene->getGeometryByID(parsedTorusKnotId)) {
    return nullptr; // null since geometry could be something else than a torus
                    // knot...
  }

  return TorusKnot::New(
    parsedTorusKnotId, scene, Json::GetNumber(parsedTorusKnot, "radius", 2.f),
    Json::GetNumber(parsedTorusKnot, "tube", 0.5f),
    Json::GetNumber(parsedTorusKnot, "radialSegments", 32u),
    Json::GetNumber(parsedTorusKnot, "tubularSegments", 32u),
    Json::GetNumber(parsedTorusKnot, "p", 2.f),
    Json::GetNumber(parsedTorusKnot, "q", 3.f),
    Json::GetBool(parsedTorusKnot, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
