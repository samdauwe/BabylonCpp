#include <babylon/materials/textures/procedurals/noise_procedural_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

NoiseProceduralTexture::NoiseProceduralTexture(const std::string& name,
                                               int size, Scene* scene,
                                               Texture* fallbackTexture,
                                               bool generateMipMaps)
    : ProceduralTexture(name, size, "noise",
                        scene ? scene : Engine::LastCreatedScene(),
                        fallbackTexture, generateMipMaps)
    , brightness{0.2f}
    , octaves{3}
    , persistence{0.8f}
    , animationSpeedFactor{1.f}
    , _time{0.f}
{
  _updateShaderUniforms();
}

NoiseProceduralTexture::~NoiseProceduralTexture()
{
}

void NoiseProceduralTexture::_updateShaderUniforms()
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  _time += scene->getAnimationRatio() * animationSpeedFactor * 0.01f;

  setFloat("brightness", brightness);
  setInt("octaves", octaves);
  setFloat("persistence", persistence);
  setFloat("timeScale", _time);
}

void NoiseProceduralTexture::render(bool useCameraPostProcess)
{
  _updateShaderUniforms();
  ProceduralTexture::render(useCameraPostProcess);
}

Json::object NoiseProceduralTexture::serialize() const
{
  return Json::object();
}

void NoiseProceduralTexture::Parse(const Json::value& /*serializationObject*/,
                                   Scene* /*scene*/,
                                   const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
