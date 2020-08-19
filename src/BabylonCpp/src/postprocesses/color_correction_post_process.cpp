#include <babylon/postprocesses/color_correction_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

ColorCorrectionPostProcess::ColorCorrectionPostProcess(
  const std::string& iName, const std::string& iColorTableUrl,
  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable)
    : PostProcess{iName,  "colorCorrection", {},     {"colorTable"}, options,
                  camera, samplingMode,      engine, reusable}
{
  _colorTableTexture = Texture::New(colorTableUrl, camera->getScene(), true, false,
                                    TextureConstants::TRILINEAR_SAMPLINGMODE);
  _colorTableTexture->anisotropicFilteringLevel = 1;
  _colorTableTexture->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _colorTableTexture->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;

  colorTableUrl = iColorTableUrl;

  onApply
    = [=](Effect* effect, EventState&) { effect->setTexture("colorTable", _colorTableTexture); };
}

ColorCorrectionPostProcess::~ColorCorrectionPostProcess() = default;

std::string ColorCorrectionPostProcess::getClassName() const
{
  return "ColorCorrectionPostProcess";
}

ColorCorrectionPostProcessPtr ColorCorrectionPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                                 const CameraPtr& /*targetCamera*/,
                                                                 Scene* /*scene*/,
                                                                 const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
