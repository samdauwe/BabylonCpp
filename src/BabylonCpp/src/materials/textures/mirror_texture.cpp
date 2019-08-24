#include <babylon/materials/textures/mirror_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/structs.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/postprocesses/blur_post_process.h>

namespace BABYLON {

MirrorTexture::MirrorTexture(const std::string& iName,
                             const std::variant<ISize, float>& size,
                             Scene* iScene, bool generateMipMaps,
                             unsigned int type, unsigned int iSamplingMode,
                             bool generateDepthBuffer)

    : RenderTargetTexture{iName, size,  iScene,        generateMipMaps,    true,
                          type,  false, iSamplingMode, generateDepthBuffer}
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
    , _imageProcessingConfigChangeObserver{nullptr}
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
      scene->activeCamera()->globalPosition(), _mirrorMatrix));
  });

  onAfterRenderObservable.add([this](int*, EventState&) {
    auto scene = getScene();
    scene->setTransformMatrix(_savedViewMatrix, scene->getProjectionMatrix());
    scene->getEngine()->cullBackFaces = true;
    scene->_mirroredCameraPosition.reset(nullptr);

    scene->clipPlane = std::nullopt;
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

  auto dw = static_cast<float>(getRenderWidth())
            / static_cast<float>(engine->getRenderWidth());
  auto dh = static_cast<float>(getRenderHeight())
            / static_cast<float>(engine->getRenderHeight());
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

    auto iTextureType = engine->getCaps().textureFloatRender ?
                          Constants::TEXTURETYPE_FLOAT :
                          Constants::TEXTURETYPE_HALF_FLOAT;

    _blurX = BlurPostProcess::New(
      "horizontal blur", Vector2(1.f, 0.f), _blurKernelX, _blurRatio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, iTextureType);
    _blurX->autoClear = false;

    if (_blurRatio == 1.f && samples() < 2 && _texture) {
      _blurX->inputTexture = _texture;
    }
    else {
      _blurX->alwaysForcePOT = true;
    }

    _blurY = BlurPostProcess::New(
      "vertical blur", Vector2(0.f, 1.f), _blurKernelY, _blurRatio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, iTextureType);
    _blurY->autoClear      = false;
    _blurY->alwaysForcePOT = _blurRatio != 1.f;

    addPostProcess(_blurX);
    addPostProcess(_blurY);
  }
  else {
    if (_blurY) {
      removePostProcess(_blurY);
      _blurY->dispose();
      _blurY = nullptr;
    }
    if (_blurX) {
      removePostProcess(_blurX);
      _blurX->dispose();
      _blurX = nullptr;
    }
  }
}

MirrorTexturePtr MirrorTexture::clone()
{
  auto iScene = getScene();

  if (!iScene) {
    return nullptr;
  }

  auto textureSize = getSize();
  auto newTexture
    = MirrorTexture::New(name,                                            //
                         Size(textureSize.width, textureSize.height),     //
                         iScene,                                          //
                         _renderTargetOptions.generateMipMaps.value(),    //
                         _renderTargetOptions.type.value(),               //
                         _renderTargetOptions.samplingMode.value(),       //
                         _renderTargetOptions.generateDepthBuffer.value() //
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

json MirrorTexture::serialize() const
{
  return nullptr;
}

void MirrorTexture::dispose()
{
  RenderTargetTexture::dispose();
  scene->imageProcessingConfiguration()->onUpdateParameters.remove(
    _imageProcessingConfigChangeObserver);
}

} // end of namespace BABYLON
