#include <babylon/proceduraltextureslibrary/road/road_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/road/road_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

RoadProceduralTexture::RoadProceduralTexture(const std::string& iName,
                                             const RenderTargetTextureSize& size, Scene* scene,
                                             Texture* fallbackTexture, bool generateMipMaps)
    : ProceduralTexture{iName,           size,           "roadProceduralTexture", scene,
                        fallbackTexture, generateMipMaps}
    , roadColor{this, &RoadProceduralTexture::get_roadColor, &RoadProceduralTexture::set_roadColor}
    , _roadColor{Color3(0.53f, 0.53f, 0.53f)}
{
  // Fragment shader
  Effect::ShadersStore()["roadProceduralTexturePixelShader"] = roadProceduralTexturePixelShader;

  updateShaderUniforms();
}

RoadProceduralTexture::~RoadProceduralTexture() = default;

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

json RoadProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<RoadProceduralTexture> RoadProceduralTexture::Parse(const json& /*parsedTexture*/,
                                                                    Scene* /*scene*/,
                                                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
