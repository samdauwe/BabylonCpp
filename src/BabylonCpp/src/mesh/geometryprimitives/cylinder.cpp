#include <babylon/mesh/geometryprimitives/cylinder.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

Cylinder::Cylinder(const string_t& _id, Scene* scene, float _height,
                   float _diameterTop, float _diameterBottom,
                   unsigned int _tessellation, unsigned int _subdivisions,
                   bool canBeRegenerated, Mesh* mesh, unsigned int _side)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , height{_height}
    , diameterTop{_diameterTop}
    , diameterBottom{_diameterBottom}
    , tessellation{_tessellation}
    , subdivisions{_subdivisions}
    , side{_side}
{
}

Cylinder::~Cylinder()
{
}

unique_ptr_t<VertexData> Cylinder::_regenerateVertexData()
{
  CylinderOptions options;
  options.height          = height;
  options.diameterTop     = diameterTop;
  options.diameterBottom  = diameterBottom;
  options.tessellation    = tessellation;
  options.subdivisions    = subdivisions;
  options.sideOrientation = side;

  return VertexData::CreateCylinder(options);
}

Geometry* Cylinder::copy(const string_t& _id)
{
  return Cylinder::New(_id, getScene(), height, diameterTop, diameterBottom,
                       tessellation, subdivisions, canBeRegenerated(), nullptr,
                       side);
}

Cylinder* Cylinder::Parse(const Json::value& parsedCylinder, Scene* scene)
{
  const auto parsedCylinderId = Json::GetString(parsedCylinder, "id");
  if (parsedCylinderId.empty() || scene->getGeometryByID(parsedCylinderId)) {
    return nullptr; // null since geometry could be something else than a
                    // cylinder...
  }

  return Cylinder::New(
    parsedCylinderId, scene, Json::GetNumber(parsedCylinder, "height", 2.f),
    Json::GetNumber(parsedCylinder, "diameterTop", 1.f),
    Json::GetNumber(parsedCylinder, "diameterBottom", 1.f),
    Json::GetNumber(parsedCylinder, "tessellation", 24u),
    Json::GetNumber(parsedCylinder, "subdivisions", 1u),
    Json::GetBool(parsedCylinder, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
