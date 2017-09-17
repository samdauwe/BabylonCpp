#include <babylon/postprocess/color_correction_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

ColorCorrectionPostProcess::ColorCorrectionPostProcess(
  const string_t& iName, const string_t& colorTableUrl, float ratio,
  Camera* camera, unsigned int samplingMode, Engine* engine, bool reusable)
    : PostProcess{iName,  "colorCorrection", {},     {"colorTable"}, ratio,
                  camera, samplingMode,      engine, reusable}
{
  _colorTableTexture
    = Texture::New(colorTableUrl, camera->getScene(), true, false,
                   TextureConstants::TRILINEAR_SAMPLINGMODE);
  _colorTableTexture->anisotropicFilteringLevel = 1;
  _colorTableTexture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _colorTableTexture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;

  setOnApply([&](Effect* effect, const EventState&) {
    effect->setTexture("colorTable", _colorTableTexture);
  });
}

ColorCorrectionPostProcess::~ColorCorrectionPostProcess()
{
}

} // end of namespace BABYLON
