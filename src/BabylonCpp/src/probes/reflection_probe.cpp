#include <babylon/probes/reflection_probe.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ReflectionProbe::ReflectionProbe(const string_t& name, const ISize& size,
                                 Scene* scene, bool generateMipMaps)
    : position{Vector3::Zero()}
    , samples{this, &ReflectionProbe::get_samples,
              &ReflectionProbe::set_samples}
    , refreshRate{this, &ReflectionProbe::get_refreshRate,
                  &ReflectionProbe::set_refreshRate}
    , renderList{this, &ReflectionProbe::get_renderList}
    , _scene{scene}
    , _viewMatrix{Matrix::Identity()}
    , _target{Vector3::Zero()}
    , _add{Vector3::Zero()}
    , _attachedMesh{nullptr}
    , _invertYAxis{false}
{
  _renderTargetTexture = ::std::make_unique<RenderTargetTexture>(
    name, size, scene, generateMipMaps, true,
    EngineConstants::TEXTURETYPE_UNSIGNED_INT, true);

  _renderTargetTexture->onBeforeRenderObservable.add(
    [this](int* faceIndex, EventState&) {
      switch (*faceIndex) {
        case 0:
          _add.copyFromFloats(1.f, 0.f, 0.f);
          break;
        case 1:
          _add.copyFromFloats(-1.f, 0.f, 0.f);
          break;
        case 2:
          _add.copyFromFloats(0.f, _invertYAxis ? 1.f : -1.f, 0.f);
          break;
        case 3:
          _add.copyFromFloats(0.f, _invertYAxis ? -1.f : 1.f, 0.f);
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
        position.copyFrom(_attachedMesh->getAbsolutePosition());
      }

      position.addToRef(_add, _target);

      Matrix::LookAtLHToRef(position, _target, Vector3::Up(), _viewMatrix);

      _scene->setTransformMatrix(_viewMatrix, _projectionMatrix);

      _scene->_forcedViewPosition = ::std::make_unique<Vector3>(position);
    });

  _renderTargetTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture*, EventState&) {
      _scene->_forcedViewPosition = nullptr;
      _scene->updateTransformMatrix(true);
    });

  if (scene->activeCamera) {
    _projectionMatrix = Matrix::PerspectiveFovLH(
      Math::PI_2, 1.f, scene->activeCamera->minZ, scene->activeCamera->maxZ);
  }
}

ReflectionProbe::~ReflectionProbe()
{
}

void ReflectionProbe::addToScene(
  unique_ptr_t<ReflectionProbe>&& newReflectionProbe)
{
  _scene->reflectionProbes.emplace_back(::std::move(newReflectionProbe));
}

unsigned int ReflectionProbe::get_samples() const
{
  return _renderTargetTexture->samples();
}

void ReflectionProbe::set_samples(unsigned int value)
{
  _renderTargetTexture->setSamples(value);
}

int ReflectionProbe::get_refreshRate() const
{
  return _renderTargetTexture->refreshRate();
}

void ReflectionProbe::set_refreshRate(int value)
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

vector_t<AbstractMesh*>& ReflectionProbe::get_renderList()
{
  return _renderTargetTexture->renderList;
}

void ReflectionProbe::attachToMesh(AbstractMesh* mesh)
{
  _attachedMesh = mesh;
}

void ReflectionProbe::setRenderingAutoClearDepthStencil(
  unsigned int renderingGroupId, bool autoClearDepthStencil)
{
  _renderTargetTexture->setRenderingAutoClearDepthStencil(
    renderingGroupId, autoClearDepthStencil);
}

void ReflectionProbe::dispose(bool /*doNotRecurse*/)
{
  // Remove from the scene if found
  _scene->reflectionProbes.erase(
    ::std::remove_if(
      _scene->reflectionProbes.begin(), _scene->reflectionProbes.end(),
      [this](const unique_ptr_t<ReflectionProbe>& reflectionProbe) {
        return reflectionProbe.get() == this;
      }),
    _scene->reflectionProbes.end());

  if (_renderTargetTexture) {
    _renderTargetTexture->dispose();
    _renderTargetTexture.reset(nullptr);
  }
}

} // end of namespace BABYLON
