#include <babylon/postprocesses/color_correction_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

ColorCorrectionPostProcess::ColorCorrectionPostProcess(
  const std::string& iName, const std::string& colorTableUrl, float ratio,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable)
    : PostProcess{iName,  "colorCorrection", {},     {"colorTable"}, ratio,
                  camera, samplingMode,      engine, reusable}
{
  _colorTableTexture
    = Texture::New(colorTableUrl, camera->getScene(), true, false,
                   TextureConstants::TRILINEAR_SAMPLINGMODE);
  _colorTableTexture->anisotropicFilteringLevel = 1;
  _colorTableTexture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _colorTableTexture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;

  onApply = [&](Effect* effect, EventState&) {
    effect->setTexture("colorTable", _colorTableTexture);
  };
}

ColorCorrectionPostProcess::~ColorCorrectionPostProcess() = default;

} // end of namespace BABYLON
