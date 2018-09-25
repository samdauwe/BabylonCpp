#include <babylon/proceduraltextureslibrary/perlinnoise/perlin_noise_procedural_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

PerlinNoiseProceduralTexture::PerlinNoiseProceduralTexture(
  const std::string& iName, const Size& size, Scene* scene,
  Texture* fallbackTexture, bool generateMipMaps)
    : ProceduralTexture{iName,
                        size,
                        "perlinNoiseProceduralTexture",
                        scene,
                        fallbackTexture,
                        generateMipMaps}
    , time{0.f}
    , timeScale{1.f}
    , translationSpeed{1.f}
    , _currentTranslation{0.f}
{
  updateShaderUniforms();
}

PerlinNoiseProceduralTexture::~PerlinNoiseProceduralTexture()
{
}

void PerlinNoiseProceduralTexture::updateShaderUniforms()
{
  setFloat("size", static_cast<float>(getRenderSize().width));

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

void PerlinNoiseProceduralTexture::resize(const Size& size,
                                          bool generateMipMaps)
{
  ProceduralTexture::resize(size, generateMipMaps);
}

Json::object PerlinNoiseProceduralTexture::serialize() const
{
  return Json::object();
}

std::unique_ptr<PerlinNoiseProceduralTexture>
PerlinNoiseProceduralTexture::Parse(const Json::value& /*parsedTexture*/,
                                    Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON
