#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

CloudProceduralTexture::CloudProceduralTexture(const std::string& iName,
                                               const Size& size, Scene* scene,
                                               Texture* fallbackTexture,
                                               bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "cloudProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , _skyColor{Color4(0.15f, 0.68f, 1.f, 1.f)}
    , _cloudColor{Color4(1.f, 1.f, 1.f, 1.f)}
{
  updateShaderUniforms();
}

CloudProceduralTexture::~CloudProceduralTexture()
{
}

void CloudProceduralTexture::updateShaderUniforms()
{
  setColor4("skyColor", _skyColor);
  setColor4("cloudColor", _cloudColor);
}

Color4& CloudProceduralTexture::skyColor()
{
  return _skyColor;
}

void CloudProceduralTexture::setSkyColor(const Color4& value)
{
  _skyColor = value;
  updateShaderUniforms();
}

Color4& CloudProceduralTexture::cloudColor()
{
  return _cloudColor;
}

void CloudProceduralTexture::setCloudColor(const Color4& value)
{
  _cloudColor = value;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
