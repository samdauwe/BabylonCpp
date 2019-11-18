#include <babylon/materials/image_processing_configuration.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

ImageProcessingConfiguration::ImageProcessingConfiguration()
    : colorCurvesEnabled{this,
                         &ImageProcessingConfiguration::get_colorCurvesEnabled,
                         &ImageProcessingConfiguration::set_colorCurvesEnabled}
    , colorGradingEnabled{this,
                          &ImageProcessingConfiguration::
                            get_colorGradingEnabled,
                          &ImageProcessingConfiguration::
                            set_colorGradingEnabled}
    , colorGradingWithGreenDepth{this,
                                 &ImageProcessingConfiguration::
                                   get_colorGradingWithGreenDepth,
                                 &ImageProcessingConfiguration::
                                   set_colorGradingWithGreenDepth}
    , colorGradingBGR{this, &ImageProcessingConfiguration::get_colorGradingBGR,
                      &ImageProcessingConfiguration::set_colorGradingBGR}
    , exposure{this, &ImageProcessingConfiguration::get_exposure,
               &ImageProcessingConfiguration::set_exposure}
    , toneMappingEnabled{this,
                         &ImageProcessingConfiguration::get_toneMappingEnabled,
                         &ImageProcessingConfiguration::set_toneMappingEnabled}
    , toneMappingType{this, &ImageProcessingConfiguration::get_toneMappingType,
                      &ImageProcessingConfiguration::set_toneMappingType}
    , contrast{this, &ImageProcessingConfiguration::get_contrast,
               &ImageProcessingConfiguration::set_contrast}
    , vignetteBlendMode{this,
                        &ImageProcessingConfiguration::get_vignetteBlendMode,
                        &ImageProcessingConfiguration::set_vignetteBlendMode}
    , vignetteEnabled{this, &ImageProcessingConfiguration::get_vignetteEnabled,
                      &ImageProcessingConfiguration::set_vignetteEnabled}
    , applyByPostProcess{this,
                         &ImageProcessingConfiguration::get_applyByPostProcess,
                         &ImageProcessingConfiguration::set_applyByPostProcess}
    , isEnabled{this, &ImageProcessingConfiguration::get_isEnabled,
                &ImageProcessingConfiguration::set_isEnabled}
    , colorCurves{std::make_unique<ColorCurves>()}
    , colorGradingTexture{this,
                          &ImageProcessingConfiguration::
                            get_colorGradingTexture,
                          &ImageProcessingConfiguration::
                            set_colorGradingTexture}
    , _exposure{1.f}
    , vignetteStretch{0}
    , vignetteCentreX{0}
    , vignetteCentreY{0}
    , vignetteWeight{1.5f}
    , vignetteColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , vignetteCameraFov{0.5f}
    , _contrast{1.f}
    , _colorCurvesEnabled{false}
    , _colorGradingTexture{nullptr}
    , _colorGradingEnabled{false}
    , _colorGradingWithGreenDepth{true}
    , _colorGradingBGR{true}
    , _toneMappingEnabled{false}
    , _toneMappingType{ImageProcessingConfiguration::TONEMAPPING_STANDARD}
    , _vignetteBlendMode{ImageProcessingConfiguration::VIGNETTEMODE_MULTIPLY()}
    , _vignetteEnabled{false}
    , _applyByPostProcess{false}
    , _isEnabled{true}
{
}

ImageProcessingConfiguration::~ImageProcessingConfiguration() = default;

bool ImageProcessingConfiguration::get_colorCurvesEnabled() const
{
  return _colorCurvesEnabled;
}

void ImageProcessingConfiguration::set_colorCurvesEnabled(bool value)
{
  if (_colorCurvesEnabled == value) {
    return;
  }

  _colorCurvesEnabled = value;
  _updateParameters();
}

BaseTexturePtr& ImageProcessingConfiguration::get_colorGradingTexture()
{
  return _colorGradingTexture;
}

void ImageProcessingConfiguration::set_colorGradingTexture(
  const BaseTexturePtr& value)
{
  if (_colorGradingTexture == value) {
    return;
  }

  _colorGradingTexture = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_colorGradingEnabled() const
{
  return _colorGradingEnabled;
}

void ImageProcessingConfiguration::set_colorGradingEnabled(bool value)
{
  if (_colorGradingEnabled == value) {
    return;
  }

  _colorGradingEnabled = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_colorGradingWithGreenDepth() const
{
  return _colorGradingWithGreenDepth;
}

void ImageProcessingConfiguration::set_colorGradingWithGreenDepth(bool value)
{
  if (_colorGradingWithGreenDepth == value) {
    return;
  }

  _colorGradingWithGreenDepth = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_colorGradingBGR() const
{
  return _colorGradingBGR;
}

void ImageProcessingConfiguration::set_colorGradingBGR(bool value)
{
  if (_colorGradingWithGreenDepth == value) {
    return;
  }

  _colorGradingWithGreenDepth = value;
  _updateParameters();
}

float ImageProcessingConfiguration::get_exposure() const
{
  return _exposure;
}

void ImageProcessingConfiguration::set_exposure(float value)
{
  if (stl_util::almost_equal(_exposure, value)) {
    return;
  }

  _exposure = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_toneMappingEnabled() const
{
  return _toneMappingEnabled;
}

void ImageProcessingConfiguration::set_toneMappingEnabled(bool value)
{
  if (_toneMappingEnabled == value) {
    return;
  }

  _toneMappingEnabled = value;
  _updateParameters();
}

unsigned int ImageProcessingConfiguration::get_toneMappingType() const
{
  return _toneMappingType;
}

void ImageProcessingConfiguration::set_toneMappingType(unsigned int value)
{
  if (_toneMappingType == value) {
    return;
  }

  _toneMappingType = value;
  _updateParameters();
}

float ImageProcessingConfiguration::get_contrast() const
{
  return _contrast;
}

void ImageProcessingConfiguration::set_contrast(float value)
{
  if (stl_util::almost_equal(_contrast, value)) {
    return;
  }

  _contrast = value;
  _updateParameters();
}

unsigned int ImageProcessingConfiguration::get_vignetteBlendMode() const
{
  return _vignetteBlendMode;
}

void ImageProcessingConfiguration::set_vignetteBlendMode(unsigned int value)
{
  if (_vignetteBlendMode == value) {
    return;
  }

  _vignetteBlendMode = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_vignetteEnabled() const
{
  return _vignetteEnabled;
}

void ImageProcessingConfiguration::set_vignetteEnabled(bool value)
{
  if (_vignetteEnabled == value) {
    return;
  }

  _vignetteEnabled = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_applyByPostProcess() const
{
  return _applyByPostProcess;
}

void ImageProcessingConfiguration::set_applyByPostProcess(bool value)
{
  if (_applyByPostProcess == value) {
    return;
  }

  _applyByPostProcess = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::get_isEnabled() const
{
  return _isEnabled;
}

void ImageProcessingConfiguration::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;
  _updateParameters();
}

void ImageProcessingConfiguration::_updateParameters()
{
  onUpdateParameters.notifyObservers(this);
}

std::string ImageProcessingConfiguration::getClassName() const
{
  return "ImageProcessingConfiguration";
}

void ImageProcessingConfiguration::PrepareUniforms(
  std::vector<std::string>& uniforms,
  const IImageProcessingConfigurationDefines& defines)
{
  if (defines.EXPOSURE) {
    uniforms.emplace_back("exposureLinear");
  }
  if (defines.CONTRAST) {
    uniforms.emplace_back("contrast");
  }
  if (defines.COLORGRADING) {
    uniforms.emplace_back("colorTransformSettings");
  }
  if (defines.VIGNETTE) {
    uniforms.emplace_back("vInverseScreenSize");
    uniforms.emplace_back("vignetteSettings1");
    uniforms.emplace_back("vignetteSettings2");
  }
  if (defines.COLORCURVES) {
    ColorCurves::PrepareUniforms(uniforms);
  }
}

void ImageProcessingConfiguration::PrepareSamplers(
  std::vector<std::string>& samplersList,
  const IImageProcessingConfigurationDefines& defines)
{
  if (defines.COLORGRADING) {
    samplersList.emplace_back("txColorTransform");
  }
}

void ImageProcessingConfiguration::prepareDefines(
  IImageProcessingConfigurationDefines& defines, bool forPostProcess)
{
  if (forPostProcess != applyByPostProcess() || !_isEnabled) {
    defines.VIGNETTE                   = false;
    defines.TONEMAPPING                = false;
    defines.TONEMAPPING_ACES           = false;
    defines.CONTRAST                   = false;
    defines.EXPOSURE                   = false;
    defines.COLORCURVES                = false;
    defines.COLORGRADING               = false;
    defines.COLORGRADING3D             = false;
    defines.IMAGEPROCESSING            = false;
    defines.IMAGEPROCESSINGPOSTPROCESS = applyByPostProcess() && _isEnabled;
    return;
  }
  defines.VIGNETTE = vignetteEnabled();
  defines.VIGNETTEBLENDMODEMULTIPLY
    = (vignetteBlendMode()
       == ImageProcessingConfiguration::VIGNETTEMODE_MULTIPLY());
  defines.VIGNETTEBLENDMODEOPAQUE = !defines.VIGNETTEBLENDMODEMULTIPLY;

  defines.TONEMAPPING = toneMappingEnabled();
  switch (_toneMappingType) {
    case ImageProcessingConfiguration::TONEMAPPING_ACES:
      defines.TONEMAPPING_ACES = true;
      break;
    default:
      defines.TONEMAPPING_ACES = false;
      break;
  }

  defines.CONTRAST     = (contrast() != 1.f);
  defines.EXPOSURE     = (exposure() != 1.f);
  defines.COLORCURVES  = (colorCurvesEnabled() && !colorCurves);
  defines.COLORGRADING = (colorGradingEnabled() && !_colorGradingTexture);
  if (defines.COLORGRADING) {
    defines.COLORGRADING3D = _colorGradingTexture->is3D;
  }
  else {
    defines.COLORGRADING3D = false;
  }
  defines.SAMPLER3DGREENDEPTH        = colorGradingWithGreenDepth();
  defines.SAMPLER3DBGRMAP            = colorGradingBGR();
  defines.IMAGEPROCESSINGPOSTPROCESS = applyByPostProcess();
  defines.IMAGEPROCESSING            = defines.VIGNETTE || defines.TONEMAPPING
                            || defines.CONTRAST || defines.EXPOSURE
                            || defines.COLORCURVES || defines.COLORGRADING;
  defines.IMAGEPROCESSING = defines.VIGNETTE || defines.TONEMAPPING
                            || defines.CONTRAST || defines.EXPOSURE
                            || defines.COLORCURVES || defines.COLORGRADING;
}

bool ImageProcessingConfiguration::isReady() const
{
  // Color Grading texure can not be none blocking.
  return !colorGradingEnabled() || !_colorGradingTexture
         || _colorGradingTexture->isReady();
}

void ImageProcessingConfiguration::bind(Effect* effect, float aspectRatio)
{
  // Color Curves
  if (_colorCurvesEnabled && colorCurves) {
    ColorCurves::Bind(*colorCurves, effect);
  }

  // Vignette
  if (_vignetteEnabled) {
    auto inverseWidth  = 1.f / effect->getEngine()->getRenderWidth();
    auto inverseHeight = 1.f / effect->getEngine()->getRenderHeight();
    effect->setFloat2("vInverseScreenSize", inverseWidth, inverseHeight);

    auto vignetteScaleY = std::tan(vignetteCameraFov * 0.5f);
    auto vignetteScaleX = vignetteScaleY * aspectRatio;

    auto vignetteScaleGeometricMean
      = std::sqrt(vignetteScaleX * vignetteScaleY);
    vignetteScaleX
      = Tools::Mix(vignetteScaleX, vignetteScaleGeometricMean, vignetteStretch);
    vignetteScaleY
      = Tools::Mix(vignetteScaleY, vignetteScaleGeometricMean, vignetteStretch);

    effect->setFloat4("vignetteSettings1", vignetteScaleX, vignetteScaleY,
                      -vignetteScaleX * vignetteCentreX,
                      -vignetteScaleY * vignetteCentreY);

    auto vignettePower = -2.f * vignetteWeight;
    effect->setFloat4("vignetteSettings2", vignetteColor.r, vignetteColor.g,
                      vignetteColor.b, vignettePower);
  }

  // Exposure
  effect->setFloat("exposureLinear", exposure());

  // Contrast
  effect->setFloat("contrast", contrast());

  // Color transform settings
  if (_colorGradingTexture) {
    effect->setTexture("txColorTransform", colorGradingTexture);
    const auto textureSize
      = static_cast<float>(_colorGradingTexture->getSize().height);

    const auto weight = static_cast<float>(_colorGradingTexture->level);
    effect->setFloat4("colorTransformSettings",
                      (textureSize - 1) / textureSize, // textureScale
                      0.5f / textureSize,              // textureOffset
                      textureSize,                     // textureSize
                      weight                           // weight
    );
  }
}

std::unique_ptr<ImageProcessingConfiguration>
ImageProcessingConfiguration::clone()
{
  return nullptr;
}

json ImageProcessingConfiguration::serialize() const
{
  return nullptr;
}

void ImageProcessingConfiguration::Parse(const json& /*source*/)
{
}

} // end of namespace BABYLON
