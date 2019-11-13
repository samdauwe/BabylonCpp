#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

WoodProceduralTexture::WoodProceduralTexture(const std::string& iName,
                                             const Size& size, Scene* scene,
                                             Texture* fallbackTexture,
                                             bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "woodProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , ampScale{this, &WoodProceduralTexture::get_ampScale,
               &WoodProceduralTexture::set_ampScale}
    , woodColor{this, &WoodProceduralTexture::get_woodColor,
                &WoodProceduralTexture::set_woodColor}
    , _ampScale{100.f}
    , _woodColor{Color3(0.32f, 0.17f, 0.09f)}
{
  // Fragment shader
  Effect::ShadersStore()["woodProceduralTexturePixelShader"]
    = woodProceduralTexturePixelShader;

  updateShaderUniforms();
}

WoodProceduralTexture::~WoodProceduralTexture() = default;

void WoodProceduralTexture::updateShaderUniforms()
{
  setFloat("ampScale", _ampScale);
  setColor3("woodColor", _woodColor);
}

float WoodProceduralTexture::get_ampScale() const
{
  return _ampScale;
}

void WoodProceduralTexture::set_ampScale(float value)
{
  _ampScale = value;
  updateShaderUniforms();
}

Color3& WoodProceduralTexture::get_woodColor()
{
  return _woodColor;
}

void WoodProceduralTexture::set_woodColor(const Color3& value)
{
  _woodColor = value;
  updateShaderUniforms();
}

json WoodProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<WoodProceduralTexture>
WoodProceduralTexture::Parse(const json& /*parsedTexture*/, Scene* /*scene*/,
                             const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
