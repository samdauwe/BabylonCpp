#include <babylon/postprocess/image_processing_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/image_processing_configuration.h>

namespace BABYLON {

ImageProcessingPostProcess::ImageProcessingPostProcess(
  const string_t& iName, float renderRatio, Camera* camera,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType)
    : PostProcess{iName,       "imageProcessing", {},     {},       renderRatio,
                  camera,      samplingMode,      engine, reusable, "",
                  textureType, "postprocess",     {},     true}
    , _fromLinearSpace{false}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr, true);

  _imageProcessingConfiguration->setApplyByPostProcess(true);

  setOnApply([&](Effect* effect, EventState& /*es*/) {
    _imageProcessingConfiguration->bind(effect, aspectRatio());
  });
}

ImageProcessingPostProcess::~ImageProcessingPostProcess()
{
}

const char* ImageProcessingPostProcess::getClassName() const
{
  return "ImageProcessingPostProcess";
}

ImageProcessingConfiguration*
ImageProcessingPostProcess::imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void ImageProcessingPostProcess::setImageProcessingConfiguration(
  ImageProcessingConfiguration* value)
{
  _attachImageProcessingConfiguration(value);
}

void ImageProcessingPostProcess::_attachImageProcessingConfiguration(
  ImageProcessingConfiguration* configuration, bool doNotBuild)
{
  if (configuration == _imageProcessingConfiguration) {
    return;
  }

  // Detaches observer.
  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(
      _imageProcessingObserver);
  }

  // Pick the scene configuration if needed.
  if (!configuration) {
    auto camera = getCamera();
    auto scene  = camera ? camera->getScene() : Engine::LastCreatedScene();
    _imageProcessingConfiguration = scene->imageProcessingConfiguration();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  _imageProcessingObserver
    = _imageProcessingConfiguration->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*conf*/, EventState& /*es*/) {
        _updateParameters();
      });

  // Ensure the effect will be rebuilt.
  if (!doNotBuild) {
    _updateParameters();
  }
}

ColorCurves* ImageProcessingPostProcess::colorCurves() const
{
  return _imageProcessingConfiguration->colorCurves.get();
}

void ImageProcessingPostProcess::setColorCurves(ColorCurves* value)
{
  _imageProcessingConfiguration->colorCurves.reset(value);
}

bool ImageProcessingPostProcess::colorCurvesEnabled() const
{
  return _imageProcessingConfiguration->colorCurvesEnabled();
}

void ImageProcessingPostProcess::setColorCurvesEnabled(bool value)
{
  _imageProcessingConfiguration->setColorCurvesEnabled(value);
}

BaseTexture* ImageProcessingPostProcess::colorGradingTexture() const
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void ImageProcessingPostProcess::setColorGradingTexture(BaseTexture* value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

bool ImageProcessingPostProcess::colorGradingEnabled() const
{
  return _imageProcessingConfiguration->colorGradingEnabled();
}

void ImageProcessingPostProcess::setColorGradingEnabled(bool value)
{
  _imageProcessingConfiguration->setColorGradingEnabled(value);
}

float ImageProcessingPostProcess::exposure() const
{
  return _imageProcessingConfiguration->exposure();
}

void ImageProcessingPostProcess::setExposure(float value)
{
  _imageProcessingConfiguration->setExposure(value);
}

bool ImageProcessingPostProcess::toneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void ImageProcessingPostProcess::setToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->setToneMappingEnabled(value);
}

float ImageProcessingPostProcess::contrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void ImageProcessingPostProcess::setContrast(float value)
{
  _imageProcessingConfiguration->setContrast(value);
}

float ImageProcessingPostProcess::vignetteStretch() const
{
  return _imageProcessingConfiguration->vignetteStretch;
}

void ImageProcessingPostProcess::setVignetteStretch(float value)
{
  _imageProcessingConfiguration->vignetteStretch = value;
}

float ImageProcessingPostProcess::vignetteCentreX() const
{
  return _imageProcessingConfiguration->vignetteCentreX;
}

void ImageProcessingPostProcess::setVignetteCentreX(float value)
{
  _imageProcessingConfiguration->vignetteCentreX = value;
}

float ImageProcessingPostProcess::vignetteCentreY() const
{
  return _imageProcessingConfiguration->vignetteCentreY;
}

void ImageProcessingPostProcess::setVignetteCentreY(float value)
{
  _imageProcessingConfiguration->vignetteCentreY = value;
}

float ImageProcessingPostProcess::vignetteWeight() const
{
  return _imageProcessingConfiguration->vignetteWeight;
}

void ImageProcessingPostProcess::setVignetteWeight(float value)
{
  _imageProcessingConfiguration->vignetteWeight = value;
}

Color4& ImageProcessingPostProcess::vignetteColor()
{
  return _imageProcessingConfiguration->vignetteColor;
}

void ImageProcessingPostProcess::setVignetteColor(const Color4& value)
{
  _imageProcessingConfiguration->vignetteColor = value;
}

float ImageProcessingPostProcess::vignetteCameraFov() const
{
  return _imageProcessingConfiguration->vignetteCameraFov;
}

void ImageProcessingPostProcess::setVignetteCameraFov(float value)
{
  _imageProcessingConfiguration->vignetteCameraFov = value;
}

unsigned int ImageProcessingPostProcess::vignetteBlendMode() const
{
  return _imageProcessingConfiguration->vignetteBlendMode();
}

void ImageProcessingPostProcess::setVignetteBlendMode(unsigned int value)
{
  _imageProcessingConfiguration->setVignetteBlendMode(value);
}

bool ImageProcessingPostProcess::vignetteEnabled() const
{
  return _imageProcessingConfiguration->vignetteEnabled();
}

void ImageProcessingPostProcess::setVignetteEnabled(bool value)
{
  _imageProcessingConfiguration->setVignetteEnabled(value);
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

void ImageProcessingPostProcess::_updateParameters()
{
  _defines.FROMLINEARSPACE = _fromLinearSpace;
  _imageProcessingConfiguration->prepareDefines(_defines, true);

  string_t defines = "";
  if (_defines.IMAGEPROCESSING) {
    defines += "#define IMAGEPROCESSING;\r\n";
  }
  else if (_defines.VIGNETTE) {
    defines += "#define VIGNETTE;\r\n";
  }
  else if (_defines.VIGNETTEBLENDMODEMULTIPLY) {
    defines += "#define VIGNETTEBLENDMODEMULTIPLY;\r\n";
  }
  else if (_defines.VIGNETTEBLENDMODEOPAQUE) {
    defines += "#define VIGNETTEBLENDMODEOPAQUE;\r\n";
  }
  else if (_defines.TONEMAPPING) {
    defines += "#define TONEMAPPING;\r\n";
  }
  else if (_defines.CONTRAST) {
    defines += "#define CONTRAST;\r\n";
  }
  else if (_defines.EXPOSURE) {
    defines += "#define EXPOSURE;\r\n";
  }
  else if (_defines.COLORCURVES) {
    defines += "#define COLORCURVES;\r\n";
  }
  else if (_defines.COLORGRADING) {
    defines += "#define COLORGRADING;\r\n";
  }
  else if (_defines.FROMLINEARSPACE) {
    defines += "#define FROMLINEARSPACE;\r\n";
  }
  else if (_defines.SAMPLER3DGREENDEPTH) {
    defines += "#define SAMPLER3DGREENDEPTH;\r\n";
  }
  else if (_defines.SAMPLER3DBGRMAP) {
    defines += "#define SAMPLER3DBGRMAP;\r\n";
  }
  else if (_defines.IMAGEPROCESSINGPOSTPROCESS) {
    defines += "#define IMAGEPROCESSINGPOSTPROCESS;\r\n";
  }

  vector_t<string_t> samplers{"textureSampler"};
  ImageProcessingConfiguration::PrepareSamplers(samplers, _defines);

  vector_t<string_t> uniforms{"scale"};
  ImageProcessingConfiguration::PrepareUniforms(uniforms, _defines);

  updateEffect(defines, uniforms, samplers);
}

void ImageProcessingPostProcess::dispose(Camera* camera)
{
  PostProcess::dispose(camera);

  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(
      _imageProcessingObserver);
  }

  _imageProcessingConfiguration->setApplyByPostProcess(false);
}

} // end of namespace BABYLON
