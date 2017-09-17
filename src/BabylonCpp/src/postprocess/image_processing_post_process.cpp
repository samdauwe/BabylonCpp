#include <babylon/postprocess/image_processing_post_process.h>

#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ImageProcessingPostProcess::ImageProcessingPostProcess(
  const string_t& iName, float renderRatio, Camera* camera,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType)
    : PostProcess{iName,
                  "imageProcessing",
                  {"contrast", "vignetteSettings1", "vignetteSettings2",
                   "cameraExposureLinear", "vCameraColorCurveNegative",
                   "vCameraColorCurveNeutral", "vCameraColorCurvePositive",
                   "colorTransformSettings"},
                  {"txColorTransform"},
                  renderRatio,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType,
                  "postprocess",
                  {},
                  true}
    , colorGradingWeight{1.f}
    , colorCurves{new ColorCurves()}
    , cameraFov{0.5f}
    , vignetteStretch{0}
    , vignetteCentreX{0}
    , vignetteCentreY{0}
    , vignetteWeight{1.5f}
    , vignetteColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , cameraContrast{1.f}
    , cameraExposure{1.68f}
    , _colorGradingTexture{nullptr}
    , _colorCurvesEnabled{true}
    , _vignetteBlendMode{ImageProcessingPostProcess::VIGNETTEMODE_MULTIPLY}
    , _vignetteEnabled{true}
    , _cameraToneMappingEnabled{true}
    , _fromLinearSpace{false}
{
  _updateParameters();

  setOnApply([this](Effect* effect, const EventState&) {
    // Color
    if (_colorCurvesEnabled) {
      ColorCurves::Bind(*colorCurves, effect);
    }

    if (_vignetteEnabled) {
      // Vignette
      float vignetteScaleY = ::std::tan(cameraFov * 0.5f);
      float vignetteScaleX = vignetteScaleY * aspectRatio();

      float vignetteScaleGeometricMean
        = ::std::sqrt(vignetteScaleX * vignetteScaleY);
      vignetteScaleX = Tools::Mix(vignetteScaleX, vignetteScaleGeometricMean,
                                  vignetteStretch);
      vignetteScaleY = Tools::Mix(vignetteScaleY, vignetteScaleGeometricMean,
                                  vignetteStretch);

      effect->setFloat4("vignetteSettings1", vignetteScaleX, vignetteScaleY,
                        -vignetteScaleX * vignetteCentreX,
                        -vignetteScaleY * vignetteCentreY);

      float vignettePower = -2.f * vignetteWeight;
      effect->setFloat4("vignetteSettings2", vignetteColor.r, vignetteColor.g,
                        vignetteColor.b, vignettePower);
    }

    // Contrast and exposure
    effect->setFloat("contrast", cameraContrast);
    effect->setFloat("cameraExposureLinear",
                     ::std::pow(2.f, -cameraExposure) * Math::PI);

    // Color transform settings
    if (_colorGradingTexture) {
      effect->setTexture("txColorTransform", colorGradingTexture());
      float textureSize
        = static_cast<float>(colorGradingTexture()->getSize().height);

      effect->setFloat4("colorTransformSettings",
                        (textureSize - 1.f) / textureSize, // textureScale
                        0.5f / textureSize,                // textureOffset
                        textureSize,                       // textureSize
                        colorGradingWeight                 // weight
                        );
    }
  });
}

ImageProcessingPostProcess::~ImageProcessingPostProcess()
{
}

const char* ImageProcessingPostProcess::getClassName() const
{
  return "ImageProcessingPostProcess";
}

BaseTexture* ImageProcessingPostProcess::colorGradingTexture()
{
  return _colorGradingTexture;
}

void ImageProcessingPostProcess::setColorGradingTexture(BaseTexture* value)
{
  if (_colorGradingTexture == value) {
    return;
  }

  _colorGradingTexture = value;
  _updateParameters();
}

unsigned int ImageProcessingPostProcess::vignetteBlendMode() const
{
  return _vignetteBlendMode;
}

void ImageProcessingPostProcess::setVignetteBlendMode(unsigned int value)
{
  if (_vignetteBlendMode == value) {
    return;
  }

  _vignetteBlendMode = value;
  _updateParameters();
}

bool ImageProcessingPostProcess::colorCurvesEnabled() const
{
  return _colorCurvesEnabled;
}

void ImageProcessingPostProcess::setColorCurvesEnabled(bool value)
{
  if (_colorCurvesEnabled == value) {
    return;
  }

  _colorCurvesEnabled = value;
  _updateParameters();
}

bool ImageProcessingPostProcess::vignetteEnabled() const
{
  return _vignetteEnabled;
}

void ImageProcessingPostProcess::setVignetteEnabled(bool value)
{
  if (_vignetteEnabled == value) {
    return;
  }

  _vignetteEnabled = value;
  _updateParameters();
}

bool ImageProcessingPostProcess::fromLinearSpace() const
{
  return _fromLinearSpace;
}

void ImageProcessingPostProcess::setFromLinearSpace(bool value)
{
  if (_fromLinearSpace == value) {
    return;
  }

  _fromLinearSpace = value;
  _updateParameters();
}

bool ImageProcessingPostProcess::cameraToneMappingEnabled() const
{
  return _cameraToneMappingEnabled;
}

void ImageProcessingPostProcess::setCameraToneMappingEnabled(bool value)
{
  if (_cameraToneMappingEnabled == value) {
    return;
  }

  _cameraToneMappingEnabled = value;
  _updateParameters();
}

void ImageProcessingPostProcess::_updateParameters()
{
  std::ostringstream defines;
  vector_t<string_t> samplers = {"textureSampler"};

  if (colorGradingTexture()) {
    defines << "#define COLORGRADING\r\n";
    samplers.emplace_back("txColorTransform");
  }

  if (_vignetteEnabled) {
    defines << "#define VIGNETTE\r\n";

    if (vignetteBlendMode()
        == ImageProcessingPostProcess::VIGNETTEMODE_MULTIPLY) {
      defines << "#define VIGNETTEBLENDMODEMULTIPLY\r\n";
    }
    else {
      defines << "#define VIGNETTEBLENDMODEOPAQUE\r\n";
    }
  }

  if (cameraToneMappingEnabled()) {
    defines << "#define TONEMAPPING\r\n";
  }

  if (_colorCurvesEnabled && colorCurves) {
    defines << "#define COLORCURVES\r\n";
  }

  if (_fromLinearSpace) {
    defines << "#define FROMLINEARSPACE\r\n";
  }

  updateEffect(defines.str(), {}, samplers);
}

} // end of namespace BABYLON
