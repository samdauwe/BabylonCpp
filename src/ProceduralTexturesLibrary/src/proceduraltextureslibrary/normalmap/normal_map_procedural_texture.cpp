#include <babylon/proceduraltextureslibrary/normalmap/normal_map_procedural_texture.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

NormalMapProceduralTexture::NormalMapProceduralTexture(const std::string& iName,
                                                       const Size& size,
                                                       Scene* scene,
                                                       Texture* fallbackTexture,
                                                       bool generateMipMaps)
    : ProceduralTexture{
        iName,           size,           "normalMapProceduralTexture", scene,
        fallbackTexture, generateMipMaps}
{
  updateShaderUniforms();
}

NormalMapProceduralTexture::~NormalMapProceduralTexture()
{
}

void NormalMapProceduralTexture::updateShaderUniforms()
{
  setTexture("baseSampler", _baseTexture);
  setFloat("size", static_cast<float>(getRenderSize().width));
}

void NormalMapProceduralTexture::render(bool useCameraPostProcess)
{
  ProceduralTexture::render(useCameraPostProcess);
}

void NormalMapProceduralTexture::resize(const Size& size, bool generateMipMaps)
{
  ProceduralTexture::resize(size, generateMipMaps);

  // We need to update the "size" uniform
  updateShaderUniforms();
}

Texture* NormalMapProceduralTexture::baseTexture() const
{
  return _baseTexture;
}

void NormalMapProceduralTexture::setBaseTexture(Texture* texture)
{
  _baseTexture = texture;
  updateShaderUniforms();
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
