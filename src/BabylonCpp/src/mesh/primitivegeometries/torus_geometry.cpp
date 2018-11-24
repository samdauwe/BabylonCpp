#include <babylon/mesh/primitivegeometries/torus_geometry.h>

#include <babylon/core/json_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

TorusGeometry::TorusGeometry(const std::string& _id, Scene* scene,
                             float _diameter, float _thickness,
                             unsigned int _tessellation, bool canBeRegenerated,
                             Mesh* mesh, unsigned int _side)
    : _PrimitiveGeometry{_id, scene, canBeRegenerated, mesh}
    , diameter{_diameter}
    , thickness{_thickness}
    , tessellation{_tessellation}
    , side{_side}
{
}

TorusGeometry::~TorusGeometry()
{
}

std::unique_ptr<VertexData> TorusGeometry::_regenerateVertexData()
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = side;

  return VertexData::CreateTorus(options);
}

GeometryPtr TorusGeometry::copy(const std::string& _id)
{
  return TorusGeometry::New(_id, getScene(), diameter, thickness, tessellation,
                            canBeRegenerated(), nullptr, side);
}

json TorusGeometry::serialize() const
{
  auto serializationObject = _PrimitiveGeometry::serialize();

  serializationObject["diameter"]     = diameter;
  serializationObject["thickness"]    = thickness;
  serializationObject["tessellation"] = tessellation;

  return serializationObject;
}

TorusGeometryPtr TorusGeometry::Parse(const json& parsedTorus, Scene* scene)
{
  const auto parsedTorusId = json_util::get_string(parsedTorus, "id");
  if (parsedTorusId.empty() || scene->getGeometryByID(parsedTorusId)) {
    return nullptr; // null since geometry could be something else than a
                    // torus...
  }

  return TorusGeometry::New(
    parsedTorusId, scene, json_util::get_number(parsedTorus, "diameter", 1.f),
    json_util::get_number(parsedTorus, "thickness", 0.5f),
    json_util::get_number(parsedTorus, "tessellation", 16u),
    json_util::get_bool(parsedTorus, "canBeRegenerated", true), nullptr);
}

} // end of namespace BABYLON
