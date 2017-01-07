#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>

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
    , _ampScale{100.f}
    , _woodColor{Color3(0.32f, 0.17f, 0.09f)}
{
  updateShaderUniforms();
}

WoodProceduralTexture::~WoodProceduralTexture()
{
}

void WoodProceduralTexture::updateShaderUniforms()
{
  setFloat("ampScale", _ampScale);
  setColor3("woodColor", _woodColor);
}

float WoodProceduralTexture::ampScale() const
{
  return _ampScale;
}

void WoodProceduralTexture::setAmpScale(float value)
{
  _ampScale = value;
  updateShaderUniforms();
}

Color3& WoodProceduralTexture::woodColor()
{
  return _woodColor;
}

void WoodProceduralTexture::setWoodColor(const Color3& value)
{
  _woodColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
