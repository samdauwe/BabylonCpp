#include <babylon/postprocess/refraction_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

RefractionPostProcess::RefractionPostProcess(
  const std::string& iName, const std::string& refractionTextureUrl,
  const Color3& iColor, float iDepth, int iColorLevel, float ratio,
  Camera* camera, unsigned int samplingMode, Engine* engine, bool reusable)
    : PostProcess{iName,
                  "refraction",
                  {"baseColor", "depth", "colorLevel"},
                  {"refractionSampler"},
                  ratio,
                  camera,
                  samplingMode,
                  engine,
                  reusable}
    , color{iColor}
    , depth{iDepth}
    , colorLevel{iColorLevel}
    , _refRexture{nullptr}
{
  onActivateObservable.add([&](Camera* cam) {
    _refRexture = _refRexture ? _refRexture : Texture::New(refractionTextureUrl,
                                                           cam->getScene());
  });

  onApplyObservable.add([&](Effect* effect) {
    effect->setColor3("baseColor", color);
    effect->setFloat("depth", depth);
    effect->setFloat("colorLevel", static_cast<float>(colorLevel));
    effect->setTexture("refractionSampler", _refRexture);
  });
}

RefractionPostProcess::~RefractionPostProcess()
{
}

void RefractionPostProcess::dispose(Camera* camera)
{
  if (_refRexture) {
    _refRexture->dispose();
  }

  PostProcess::dispose(camera);
}

} // end of namespace BABYLON
