#include <babylon/proceduraltextureslibrary/road/road_procedural_texture.h>

#include <babylon/core/json.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

RoadProceduralTexture::RoadProceduralTexture(const std::string& iName,
                                             const Size& size, Scene* scene,
                                             Texture* fallbackTexture,
                                             bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "roadProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , roadColor{this, &RoadProceduralTexture::get_roadColor,
                &RoadProceduralTexture::set_roadColor}
    , _roadColor{Color3(0.53f, 0.53f, 0.53f)}
{
  updateShaderUniforms();
}

RoadProceduralTexture::~RoadProceduralTexture()
{
}

void RoadProceduralTexture::updateShaderUniforms()
{
  setColor3("roadColor", _roadColor);
}

Color3& RoadProceduralTexture::get_roadColor()
{
  return _roadColor;
}

void RoadProceduralTexture::set_roadColor(const Color3& value)
{
  _roadColor = value;
  updateShaderUniforms();
}

Json::object RoadProceduralTexture::serialize() const
{
  return Json::object();
}

unique_ptr_t<RoadProceduralTexture>
RoadProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                             Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
