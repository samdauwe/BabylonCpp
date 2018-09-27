#include <babylon/materials/textures/mirror_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/structs.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/postprocess/blur_post_process.h>

namespace BABYLON {

MirrorTexture::MirrorTexture(const std::string& iName, const ISize& size,
                             Scene* iScene, bool generateMipMaps,
                             unsigned int type, unsigned int samplingMode,
                             bool generateDepthBuffer)

    : RenderTargetTexture{iName, size,  iScene,       generateMipMaps,    true,
                          type,  false, samplingMode, generateDepthBuffer}
    , mirrorPlane{Plane(0.f, 1.f, 0.f, 1.f)}
    , blurRatio{this, &MirrorTexture::get_blurRatio,
                &MirrorTexture::set_blurRatio}
    , adaptiveBlurKernel{this, &MirrorTexture::set_adaptiveBlurKernel}
    , blurKernel{this, &MirrorTexture::set_blurKernel}
    , blurKernelX{this, &MirrorTexture::get_blurKernelX,
                  &MirrorTexture::set_blurKernelX}
    , blurKernelY{this, &MirrorTexture::get_blurKernelY,
                  &MirrorTexture::set_blurKernelY}
    , scene{iScene}
    , _transformMatrix{Matrix::Zero()}
    , _mirrorMatrix{Matrix::Zero()}
    , _savedViewMatrix{Matrix::Zero()}
    , _blurX{nullptr}
    , _blurY{nullptr}
    , _adaptiveBlurKernel{0.f}
    , _blurKernelX{0.f}
    , _blurKernelY{0.f}
    , _blurRatio{1.f}

{
  ignoreCameraViewport = true;

  _updateGammaSpace();
  _imageProcessingConfigChangeObserver
    = scene->imageProcessingConfiguration()->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*ipc*/, EventState& /*es*/) {
        _updateGammaSpace();
      });

  onBeforeRenderObservable.add([this](int*, EventState&) {
    auto scene = getScene();
    Matrix::ReflectionToRef(mirrorPlane, _mirrorMatrix);
    _savedViewMatrix = scene->getViewMatrix();
    _mirrorMatrix.multiplyToRef(_savedViewMatrix, _transformMatrix);
    scene->setTransformMatrix(_transformMatrix, scene->getProjectionMatrix());
    scene->clipPlane                  = mirrorPlane;
    scene->getEngine()->cullBackFaces = false;
    scene->setMirroredCameraPosition(Vector3::TransformCoordinates(
      scene->activeCamera->globalPosition(), _mirrorMatrix));
  });

  onAfterRenderObservable.add([this](int*, EventState&) {
    auto scene = getScene();
    scene->setTransformMatrix(_savedViewMatrix, scene->getProjectionMatrix());
    scene->getEngine()->cullBackFaces = true;
    scene->_mirroredCameraPosition.reset(nullptr);
  });
}

MirrorTexture::~MirrorTexture()
{
}

void MirrorTexture::set_blurRatio(float value)
{
  if (stl_util::almost_equal(_blurRatio, value)) {
    return;
  }

  _blurRatio = value;
  _preparePostProcesses();
}

float MirrorTexture::get_blurRatio() const
{
  return _blurRatio;
}

void MirrorTexture::set_adaptiveBlurKernel(float value)
{
  _adaptiveBlurKernel = value;
  _autoComputeBlurKernel();
}

void MirrorTexture::set_blurKernel(float value)
{
  blurKernelX = value;
  blurKernelY = value;
}

void MirrorTexture::set_blurKernelX(float value)
{
  if (stl_util::almost_equal(_blurKernelX, value)) {
    return;
  }

  _blurKernelX = value;
  _preparePostProcesses();
}

float MirrorTexture::get_blurKernelX() const
{
  return _blurKernelX;
}

void MirrorTexture::set_blurKernelY(float value)
{
  if (stl_util::almost_equal(_blurKernelY, value)) {
    return;
  }

  _blurKernelY = value;
  _preparePostProcesses();
}

float MirrorTexture::get_blurKernelY() const
{
  return _blurKernelY;
}

void MirrorTexture::_autoComputeBlurKernel()
{
  auto engine = getScene()->getEngine();

  auto dw     = getRenderWidth() / engine->getRenderWidth();
  auto dh     = getRenderHeight() / engine->getRenderHeight();
  blurKernelX = _adaptiveBlurKernel * dw;
  blurKernelY = _adaptiveBlurKernel * dh;
}

void MirrorTexture::_onRatioRescale()
{
  if (_sizeRatio != 0.f) {
    resize(_initialSizeParameter);
    if (_adaptiveBlurKernel == 0.f) {
      _preparePostProcesses();
    }
  }

  if (_adaptiveBlurKernel != 0.f) {
    _autoComputeBlurKernel();
  }
}

void MirrorTexture::_updateGammaSpace()
{
  gammaSpace = !scene->imageProcessingConfiguration()->isEnabled()
               || !scene->imageProcessingConfiguration()->applyByPostProcess();
}

void MirrorTexture::_preparePostProcesses()
{
  clearPostProcesses(true);

  if (_blurKernelX != 0.f && _blurKernelY != 0.f) {
    auto engine = getScene()->getEngine();

    auto textureType = engine->getCaps().textureFloatRender ?
                         EngineConstants::TEXTURETYPE_FLOAT :
                         EngineConstants::TEXTURETYPE_HALF_FLOAT;

    _blurX = std::make_unique<BlurPostProcess>(
      "horizontal blur", Vector2(1.f, 0.f), _blurKernelX,
      ToVariant<float, PostProcessOptions>(_blurRatio), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, textureType);
    _blurX->autoClear = false;

    if (_blurRatio == 1.f && samples() < 2 && _texture) {
      _blurX->setInputTexture(_texture);
    }
    else {
      _blurX->alwaysForcePOT = true;
    }

    _blurY = std::make_unique<BlurPostProcess>(
      "vertical blur", Vector2(0.f, 1.f), _blurKernelY,
      ToVariant<float, PostProcessOptions>(_blurRatio), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, textureType);
    _blurY->autoClear      = false;
    _blurY->alwaysForcePOT = _blurRatio != 1.f;

    addPostProcess(_blurX.get());
    addPostProcess(_blurY.get());
  }
  else {
    if (_blurY) {
      removePostProcess(_blurY.get());
      _blurY->dispose();
      _blurY = nullptr;
    }
    if (_blurX) {
      removePostProcess(_blurX.get());
      _blurX->dispose();
      _blurX = nullptr;
    }
  }
}

std::unique_ptr<MirrorTexture> MirrorTexture::clone()
{
  auto scene = getScene();

  if (!scene) {
    return nullptr;
  }

  auto textureSize = getSize();
  auto newTexture  = std::make_unique<MirrorTexture>(
    name,                                        //
    Size(textureSize.width, textureSize.height), //
    scene,                                       //
    _renderTargetOptions.generateMipMaps,        //
    _renderTargetOptions.type,                   //
    _renderTargetOptions.samplingMode,           //
    _renderTargetOptions.generateDepthBuffer     //
  );

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // Mirror Texture
  newTexture->mirrorPlane = mirrorPlane;
  if (!renderList().empty()) {
    newTexture->renderList = renderList();
  }

  return newTexture;
}

Json::object MirrorTexture::serialize() const
{
  return Json::object();
}

void MirrorTexture::dispose()
{
  RenderTargetTexture::dispose();
  scene->imageProcessingConfiguration()->onUpdateParameters.remove(
    _imageProcessingConfigChangeObserver);
}

} // end of namespace BABYLON
