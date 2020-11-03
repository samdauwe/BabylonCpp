#include <babylon/proceduraltextureslibrary/perlinnoise/perlin_noise_procedural_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/proceduraltextureslibrary/perlinnoise/perlin_noise_procedural_texture_fragment_fx.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

PerlinNoiseProceduralTexture::PerlinNoiseProceduralTexture(const std::string& iName,
                                                           const RenderTargetTextureSize& size,
                                                           Scene* scene, Texture* fallbackTexture,
                                                           bool generateMipMaps)
    : ProceduralTexture{iName,           size,           "perlinNoiseProceduralTexture", scene,
                        fallbackTexture, generateMipMaps}
    , time{0.f}
    , timeScale{1.f}
    , translationSpeed{1.f}
    , _currentTranslation{0.f}
{
  // Fragment shader
  Effect::ShadersStore()["perlinNoiseProceduralTexturePixelShader"]
    = perlinNoiseProceduralTexturePixelShader;

  updateShaderUniforms();
}

PerlinNoiseProceduralTexture::~PerlinNoiseProceduralTexture() = default;

void PerlinNoiseProceduralTexture::updateShaderUniforms()
{
  setFloat("size", std::holds_alternative<int>(getRenderSize()) ?
                     static_cast<float>(std::get<int>(getRenderSize())) :
                     std::get<float>(getRenderSize()));

  auto scene = getScene();

  if (!scene) {
    return;
  }

  auto deltaTime = scene->getEngine()->getDeltaTime();

  time += deltaTime;
  setFloat("time", time * timeScale / 1000.f);

  _currentTranslation += deltaTime * translationSpeed / 1000.f;
  setFloat("translationSpeed", _currentTranslation);
}

void PerlinNoiseProceduralTexture::render(bool useCameraPostProcess)
{
  updateShaderUniforms();
  ProceduralTexture::render(useCameraPostProcess);
}

void PerlinNoiseProceduralTexture::resize(const Size& size, bool generateMipMaps)
{
  ProceduralTexture::resize(size, generateMipMaps);
}

json PerlinNoiseProceduralTexture::serialize() const
{
  return nullptr;
}

std::unique_ptr<PerlinNoiseProceduralTexture>
PerlinNoiseProceduralTexture::Parse(const json& /*parsedTexture*/, Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
