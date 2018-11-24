#include <babylon/proceduraltextureslibrary/normalmap/normal_map_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/normalmap/normal_map_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

NormalMapProceduralTexture::NormalMapProceduralTexture(const std::string& iName,
                                                       const Size& size,
                                                       Scene* scene,
                                                       Texture* fallbackTexture,
                                                       bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "normalMapProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , baseTexture{this, &NormalMapProceduralTexture::get_baseTexture,
                  &NormalMapProceduralTexture::set_baseTexture}
    , _baseTexture{nullptr}
{
  // Fragment shader
  Effect::ShadersStore()["normalMapProceduralTexturePixelShader"]
    = normalMapProceduralTexturePixelShader;

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

TexturePtr& NormalMapProceduralTexture::get_baseTexture()
{
  return _baseTexture;
}

void NormalMapProceduralTexture::set_baseTexture(const TexturePtr& texture)
{
  _baseTexture = texture;
  updateShaderUniforms();
}

json NormalMapProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<NormalMapProceduralTexture>
NormalMapProceduralTexture::Parse(const json& /*parsedTexture*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
