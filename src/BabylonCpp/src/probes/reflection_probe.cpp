#include <babylon/probes/reflection_probe.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ReflectionProbe::ReflectionProbe(const std::string& name, const ISize& size,
                                 Scene* scene, bool generateMipMaps)
    : invertYAxis{false}
    , position{Vector3::Zero()}
    , _scene{scene}
    , _viewMatrix{Matrix::Identity()}
    , _target{Vector3::Zero()}
    , _add{Vector3::Zero()}
{
  _renderTargetTexture = std_util::make_unique<RenderTargetTexture>(
    name, size, scene, generateMipMaps, true, Engine::TEXTURETYPE_UNSIGNED_INT,
    true);

  _renderTargetTexture->onBeforeRenderObservable.add([&](int faceIndex) {
    switch (faceIndex) {
      case 0:
        _add.copyFromFloats(1.f, 0.f, 0.f);
        break;
      case 1:
        _add.copyFromFloats(-1.f, 0.f, 0.f);
        break;
      case 2:
        _add.copyFromFloats(0.f, invertYAxis ? 1.f : -1.f, 0.f);
        break;
      case 3:
        _add.copyFromFloats(0.f, invertYAxis ? -1.f : 1.f, 0.f);
        break;
      case 4:
        _add.copyFromFloats(0.f, 0.f, 1.f);
        break;
      case 5:
        _add.copyFromFloats(0.f, 0.f, -1.f);
        break;
      default:
        break;
    }

    if (_attachedMesh) {
      position.copyFrom(*_attachedMesh->getAbsolutePosition());
    }

    position.addToRef(_add, _target);

    Matrix::LookAtLHToRef(position, _target, Vector3::Up(), _viewMatrix);

    scene->setTransformMatrix(_viewMatrix, _projectionMatrix);
  });

  _renderTargetTexture->onAfterUnbindObservable.add(
    [&]() { scene->updateTransformMatrix(true); });

  _projectionMatrix = Matrix::PerspectiveFovLH(
    Math::PI_2, 1.f, scene->activeCamera->minZ, scene->activeCamera->maxZ);
}

ReflectionProbe::~ReflectionProbe()
{
}

void ReflectionProbe::addToScene(
  std::unique_ptr<ReflectionProbe>&& newReflectionProbe)
{
  _scene->reflectionProbes.emplace_back(std::move(newReflectionProbe));
}

int ReflectionProbe::refreshRate() const
{
  return _renderTargetTexture->refreshRate();
}

void ReflectionProbe::setRefreshRate(int value)
{
  _renderTargetTexture->setRefreshRate(value);
}

Scene* ReflectionProbe::getScene() const
{
  return _scene;
}

RenderTargetTexture* ReflectionProbe::cubeTexture()
{
  return _renderTargetTexture.get();
}

std::vector<AbstractMesh*>& ReflectionProbe::renderList()
{
  return _renderTargetTexture->renderList;
}

void ReflectionProbe::attachToMesh(AbstractMesh* mesh)
{
  _attachedMesh = mesh;
}

void ReflectionProbe::dispose(bool /*doNotRecurse*/)
{
  // Remove from the scene if found
  _scene->reflectionProbes.erase(
    std::remove_if(
      _scene->reflectionProbes.begin(), _scene->reflectionProbes.end(),
      [this](const std::unique_ptr<ReflectionProbe>& reflectionProbe) {
        return reflectionProbe.get() == this;
      }),
    _scene->reflectionProbes.end());

  if (_renderTargetTexture) {
    _renderTargetTexture->dispose();
    _renderTargetTexture.reset(nullptr);
  }
}

} // end of namespace BABYLON
