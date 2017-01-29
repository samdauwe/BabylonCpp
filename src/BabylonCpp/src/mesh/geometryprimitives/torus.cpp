#include <babylon/mesh/geometryprimitives/torus.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

Torus::Torus(const std::string& _id, Scene* scene, float _diameter,
             float _thickness, unsigned int _tessellation,
             bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , diameter{_diameter}
    , thickness{_thickness}
    , tessellation{_tessellation}
    , side{_side}
{
}

Torus::~Torus()
{
}

std::unique_ptr<VertexData> Torus::_regenerateVertexData()
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = side;

  return VertexData::CreateTorus(options);
}

Geometry* Torus::copy(const std::string& _id)
{
  return new Torus(_id, getScene(), diameter, thickness, tessellation,
                   canBeRegenerated(), nullptr, side);
}

Torus* Torus::Parse(const Json::value& parsedTorus, Scene* scene)
{
  const auto parsedTorusId = Json::GetString(parsedTorus, "id");
  if (parsedTorusId.empty() || scene->getGeometryByID(parsedTorusId)) {
    return nullptr; // null since geometry could be something else than a
                    // torus...
  }

  return Torus::New(
    parsedTorusId, scene, Json::GetNumber(parsedTorus, "diameter", 1.f),
    Json::GetNumber(parsedTorus, "thickness", 0.5f),
    Json::GetNumber(parsedTorus, "tessellation", 16u),
    Json::GetBool(parsedTorus, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
