#include <babylon/materials/textures/procedurals/noise_procedural_texture.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

NoiseProceduralTexture::NoiseProceduralTexture(const std::string& iName, int size, Scene* scene,
                                               Texture* fallbackTexture, bool generateMipMaps)
    : ProceduralTexture(iName, size, "noise", scene ? scene : Engine::LastCreatedScene(),
                        fallbackTexture, generateMipMaps)
    , time{0.f}
    , brightness{0.2f}
    , octaves{3}
    , persistence{0.8f}
    , animationSpeedFactor{1.f}
{
  autoClear = false;
  _updateShaderUniforms();
}

NoiseProceduralTexture::~NoiseProceduralTexture() = default;

void NoiseProceduralTexture::_updateShaderUniforms()
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  time += scene->getAnimationRatio() * animationSpeedFactor * 0.01f;

  setFloat("brightness", brightness);
  setFloat("persistence", persistence);
  setFloat("timeScale", time);
}

std::string NoiseProceduralTexture::_getDefines() const
{
  return "#define OCTAVES " + std::to_string(octaves);
}

void NoiseProceduralTexture::render(bool useCameraPostProcess)
{
  _updateShaderUniforms();
  ProceduralTexture::render(useCameraPostProcess);
}

json NoiseProceduralTexture::serialize() const
{
  return nullptr;
}

NoiseProceduralTexturePtr NoiseProceduralTexture::clone()
{
  const auto textureSize = getSize();
  auto newTexture
    = NoiseProceduralTexture::New(name, textureSize.width, getScene(),
                                  _fallbackTexture ? _fallbackTexture : nullptr, _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // RenderTarget Texture
  newTexture->coordinatesMode = coordinatesMode();

  // Noise Specifics
  newTexture->brightness           = brightness;
  newTexture->octaves              = octaves;
  newTexture->persistence          = persistence;
  newTexture->animationSpeedFactor = animationSpeedFactor;
  newTexture->time                 = time;

  return newTexture;
}

NoiseProceduralTexturePtr NoiseProceduralTexture::Parse(const json& /*serializationObject*/,
                                                        Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
