#include <babylon/mesh/geometryprimitives/ground.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace GeometryPrimitives {

Ground::Ground(const string_t& _id, Scene* scene, unsigned int _width,
               unsigned int _height, unsigned int _subdivisions,
               bool canBeRegenerated, Mesh* mesh)
    : _Primitive{_id, scene, canBeRegenerated, mesh}
    , width{_width}
    , height{_height}
    , subdivisions{_subdivisions}
{
}

Ground::~Ground()
{
}

unique_ptr_t<VertexData> Ground::_regenerateVertexData()
{
  GroundOptions options(subdivisions);
  options.width  = width;
  options.height = height;

  return VertexData::CreateGround(options);
}

Geometry* Ground::copy(const string_t& _id)
{
  return Ground::New(_id, getScene(), width, height, subdivisions,
                     canBeRegenerated(), nullptr);
}

Ground* Ground::Parse(const Json::value& parsedGround, Scene* scene)
{
  const auto parsedGroundId = Json::GetString(parsedGround, "id");
  if (parsedGroundId.empty() || scene->getGeometryByID(parsedGroundId)) {
    return nullptr; // null since geometry could be something else than a
                    // ground...
  }

  return Ground::New(
    parsedGroundId, scene, Json::GetNumber(parsedGround, "width", 1u),
    Json::GetNumber(parsedGround, "height", 1u),
    Json::GetNumber(parsedGround, "subdivisions", 1u),
    Json::GetBool(parsedGround, "canBeRegenerated", true), nullptr);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
