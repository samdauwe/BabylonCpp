#include <babylon/materials/image_processing_configuration.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ImageProcessingConfiguration::ImageProcessingConfiguration()
    : colorCurves{::std::make_unique<ColorCurves>()}
    , colorGradingTexture{nullptr}
    , _exposure{1.f}
    , vignetteStretch{0}
    , vignetteCentreX{0}
    , vignetteCentreY{0}
    , vignetteWeight{1.5f}
    , vignetteColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , vignetteCameraFov{0.5f}
    , _contrast{1.f}
    , _colorCurvesEnabled{false}
    , _colorGradingEnabled{false}
    , _colorGradingWithGreenDepth{true}
    , _colorGradingBGR{true}
    , _toneMappingEnabled{false}
    , _vignetteBlendMode{ImageProcessingConfiguration::VIGNETTEMODE_MULTIPLY()}
    , _vignetteEnabled{false}
    , _applyByPostProcess{false}
    , _isEnabled{true}
{
}

ImageProcessingConfiguration::~ImageProcessingConfiguration()
{
}

bool ImageProcessingConfiguration::colorCurvesEnabled() const
{
  return _colorCurvesEnabled;
}

void ImageProcessingConfiguration::setColorCurvesEnabled(bool value)
{
  if (_colorCurvesEnabled == value) {
    return;
  }

  _colorCurvesEnabled = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::colorGradingEnabled() const
{
  return _colorGradingEnabled;
}

void ImageProcessingConfiguration::setColorGradingEnabled(bool value)
{
  if (_colorGradingEnabled == value) {
    return;
  }

  _colorGradingEnabled = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::colorGradingWithGreenDepth() const
{
  return _colorGradingWithGreenDepth;
}

void ImageProcessingConfiguration::setColorGradingWithGreenDepth(bool value)
{
  if (_colorGradingWithGreenDepth == value) {
    return;
  }

  _colorGradingWithGreenDepth = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::colorGradingBGR() const
{
  return _colorGradingBGR;
}

void ImageProcessingConfiguration::setColorGradingBGR(bool value)
{
  if (_colorGradingWithGreenDepth == value) {
    return;
  }

  _colorGradingWithGreenDepth = value;
  _updateParameters();
}

float ImageProcessingConfiguration::exposure() const
{
  return _exposure;
}

void ImageProcessingConfiguration::setExposure(float value)
{
  if (stl_util::almost_equal(_exposure, value)) {
    return;
  }

  _exposure = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::toneMappingEnabled() const
{
  return _toneMappingEnabled;
}

void ImageProcessingConfiguration::setToneMappingEnabled(bool value)
{
  if (_toneMappingEnabled == value) {
    return;
  }

  _toneMappingEnabled = value;
  _updateParameters();
}

float ImageProcessingConfiguration::contrast() const
{
  return _contrast;
}

void ImageProcessingConfiguration::setContrast(float value)
{
  if (stl_util::almost_equal(_contrast, value)) {
    return;
  }

  _contrast = value;
  _updateParameters();
}

unsigned int ImageProcessingConfiguration::vignetteBlendMode() const
{
  return _vignetteBlendMode;
}

void ImageProcessingConfiguration::setVignetteBlendMode(unsigned int value)
{
  if (_vignetteBlendMode == value) {
    return;
  }

  _vignetteBlendMode = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::vignetteEnabled() const
{
  return _vignetteEnabled;
}

void ImageProcessingConfiguration::setVignetteEnabled(bool value)
{
  if (_vignetteEnabled == value) {
    return;
  }

  _vignetteEnabled = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::applyByPostProcess() const
{
  return _applyByPostProcess;
}

void ImageProcessingConfiguration::setApplyByPostProcess(bool value)
{
  if (_applyByPostProcess == value) {
    return;
  }

  _applyByPostProcess = value;
  _updateParameters();
}

bool ImageProcessingConfiguration::isEnabled() const
{
  return _isEnabled;
}

void ImageProcessingConfiguration::setIsEnabled(bool value)
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

const char* ImageProcessingConfiguration::getClassName() const
{
  return "ImageProcessingConfiguration";
}

void ImageProcessingConfiguration::PrepareUniforms(
  vector_t<string_t>& uniforms,
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
  vector_t<string_t>& samplersList,
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
  defines.TONEMAPPING             = toneMappingEnabled();
  defines.CONTRAST                = (contrast() != 1.f);
  defines.EXPOSURE                = (exposure() != 1.f);
  defines.COLORCURVES             = (colorCurvesEnabled() && !colorCurves);
  defines.COLORGRADING = (colorGradingEnabled() && !colorGradingTexture);
  if (defines.COLORGRADING) {
    defines.COLORGRADING3D = colorGradingTexture->is3D;
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
}

bool ImageProcessingConfiguration::isReady() const
{
  // Color Grading texure can not be none blocking.
  return !colorGradingEnabled() || !colorGradingTexture
         || colorGradingTexture->isReady();
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

    auto vignetteScaleY = ::std::tan(vignetteCameraFov * 0.5f);
    auto vignetteScaleX = vignetteScaleY * aspectRatio;

    auto vignetteScaleGeometricMean
      = ::std::sqrt(vignetteScaleX * vignetteScaleY);
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
  if (colorGradingTexture) {
    effect->setTexture("txColorTransform", colorGradingTexture);
    const auto textureSize
      = static_cast<float>(colorGradingTexture->getSize().height);

    const auto weight = static_cast<float>(colorGradingTexture->level);
    effect->setFloat4("colorTransformSettings",
                      (textureSize - 1) / textureSize, // textureScale
                      0.5f / textureSize,              // textureOffset
                      textureSize,                     // textureSize
                      weight                           // weight
    );
  }
}

unique_ptr_t<ImageProcessingConfiguration> ImageProcessingConfiguration::clone()
{
  return nullptr;
}

Json::object ImageProcessingConfiguration::serialize() const
{
  return Json::object();
}

void ImageProcessingConfiguration::Parse(const Json::value& /*source*/)
{
}

} // end of namespace BABYLON
