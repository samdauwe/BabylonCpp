#include <babylon/materials/textures/mirror_texture.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/structs.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

MirrorTexture::MirrorTexture(const std::string& iName, const ISize& size,
                             Scene* scene, bool generateMipMaps,
                             unsigned int type, unsigned int samplingMode,
                             bool generateDepthBuffer)

    : RenderTargetTexture{iName, size,  scene,        generateMipMaps,    true,
                          type,  false, samplingMode, generateDepthBuffer}
    , mirrorPlane{Plane(0.f, 1.f, 0.f, 1.f)}
    , _transformMatrix{Matrix::Zero()}
    , _mirrorMatrix{Matrix::Zero()}
    , _savedViewMatrix{Matrix::Zero()}

{
  onBeforeRender = [this]() {
    auto scene = getScene();
    Matrix::ReflectionToRef(mirrorPlane, _mirrorMatrix);
    _savedViewMatrix = scene->getViewMatrix();
    _mirrorMatrix.multiplyToRef(_savedViewMatrix, _transformMatrix);
    scene->setTransformMatrix(_transformMatrix, scene->getProjectionMatrix());
    scene->setClipPlane(mirrorPlane);
    scene->getEngine()->cullBackFaces = false;
    scene->setMirroredCameraPosition(Vector3::TransformCoordinates(
      scene->activeCamera->position, _mirrorMatrix));
  };

  onAfterRender = [this]() {
    auto scene = getScene();
    scene->setTransformMatrix(_savedViewMatrix, scene->getProjectionMatrix());
    scene->getEngine()->cullBackFaces = true;
    scene->_mirroredCameraPosition.reset(nullptr);
    scene->resetClipPlane();
  };
}

MirrorTexture::~MirrorTexture()
{
}

std::unique_ptr<MirrorTexture> MirrorTexture::clone() const
{
  auto textureSize = getSize();
  auto newTexture  = std_util::make_unique<MirrorTexture>(
    name, Size(textureSize.width, textureSize.height), getScene(),
    _renderTargetOptions.generateMipMaps, _renderTargetOptions.type,
    _renderTargetOptions.samplingMode,
    _renderTargetOptions.generateDepthBuffer);

  // Base texture
  newTexture->setHasAlpha(hasAlpha());
  newTexture->level = level;

  // Mirror Texture
  newTexture->mirrorPlane = mirrorPlane;
  newTexture->renderList  = renderList;

  return newTexture;
}

Json::object MirrorTexture::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
