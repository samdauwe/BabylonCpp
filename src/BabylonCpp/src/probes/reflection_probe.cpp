#include <babylon/probes/reflection_probe.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ReflectionProbe::ReflectionProbe(const std::string& iName, const ISize& size, Scene* scene,
                                 bool generateMipMaps, bool useFloat)
    : name{iName}
    , position{Vector3::Zero()}
    , samples{this, &ReflectionProbe::get_samples, &ReflectionProbe::set_samples}
    , refreshRate{this, &ReflectionProbe::get_refreshRate, &ReflectionProbe::set_refreshRate}
    , cubeTexture{this, &ReflectionProbe::get_cubeTexture}
    , renderList{this, &ReflectionProbe::get_renderList}
    , _scene{scene}
    , _renderTargetTexture{nullptr}
    , _viewMatrix{Matrix::Identity()}
    , _target{Vector3::Zero()}
    , _add{Vector3::Zero()}
    , _attachedMesh{nullptr}
    , _invertYAxis{false}
{
  auto textureType = Constants::TEXTURETYPE_UNSIGNED_BYTE;
  if (useFloat) {
    const auto caps = _scene->getEngine()->getCaps();
    if (caps.textureHalfFloatRender) {
      textureType = Constants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (caps.textureFloatRender) {
      textureType = Constants::TEXTURETYPE_FLOAT;
    }
  }
  _renderTargetTexture = RenderTargetTexture::New(iName, RenderTargetSize{size.width, size.height},
                                                  scene, generateMipMaps, true, textureType, true);

  _renderTargetTexture->onBeforeRenderObservable.add([scene, this](const int* faceIndex,
                                                                   EventState&) {
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
        _add.copyFromFloats(0.f, 0.f, scene->useRightHandedSystem() ? -1.f : 1.f);
        break;
      case 5:
        _add.copyFromFloats(0.f, 0.f, scene->useRightHandedSystem() ? 1.f : -1.f);
        break;
      default:
        break;
    }

    if (_attachedMesh) {
      position.copyFrom(_attachedMesh->getAbsolutePosition());
    }

    position.addToRef(_add, _target);

    if (scene->useRightHandedSystem()) {
      Matrix::LookAtRHToRef(position, _target, Vector3::Up(), _viewMatrix);

      if (scene->activeCamera()) {
        _projectionMatrix = Matrix::PerspectiveFovRH(
          Math::PI / 2.f, 1.f, scene->activeCamera()->minZ, scene->activeCamera()->maxZ);
        scene->setTransformMatrix(_viewMatrix, _projectionMatrix);
      }
    }
    else {
      Matrix::LookAtLHToRef(position, _target, Vector3::Up(), _viewMatrix);

      if (_scene->activeCamera()) {
        _projectionMatrix = Matrix::PerspectiveFovLH(Math::PI_2, 1.f, _scene->activeCamera()->minZ,
                                                     _scene->activeCamera()->maxZ);
        _scene->setTransformMatrix(_viewMatrix, _projectionMatrix);
      }
    }

    _scene->_forcedViewPosition = std::make_unique<Vector3>(position);
  });

  _renderTargetTexture->onBeforeBindObservable.add(
    [this](RenderTargetTexture* /*texture*/, EventState& /*es*/) -> void {
      _scene->getEngine()->_debugPushGroup(
        StringTools::printf("reflection probe generation for %s", name.c_str()), 1);
    });

  _renderTargetTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture* /*texture*/, EventState& /*es*/) -> void {
      _scene->_forcedViewPosition = nullptr;
      _scene->updateTransformMatrix(true);
      _scene->getEngine()->_debugPopGroup(1);
    });
}

ReflectionProbe::~ReflectionProbe() = default;

void ReflectionProbe::addToScene(const ReflectionProbePtr& newReflectionProbe)
{
  _scene->reflectionProbes.emplace_back(newReflectionProbe);
}

unsigned int ReflectionProbe::get_samples() const
{
  return _renderTargetTexture->samples();
}

void ReflectionProbe::set_samples(unsigned int value)
{
  _renderTargetTexture->samples = value;
}

int ReflectionProbe::get_refreshRate() const
{
  return _renderTargetTexture->refreshRate();
}

void ReflectionProbe::set_refreshRate(int value)
{
  _renderTargetTexture->refreshRate = value;
}

Scene* ReflectionProbe::getScene() const
{
  return _scene;
}

RenderTargetTexturePtr& ReflectionProbe::get_cubeTexture()
{
  return _renderTargetTexture;
}

std::vector<AbstractMesh*>& ReflectionProbe::get_renderList()
{
  return _renderTargetTexture->renderList;
}

void ReflectionProbe::attachToMesh(AbstractMesh* mesh)
{
  _attachedMesh = mesh;
}

void ReflectionProbe::setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                                        bool autoClearDepthStencil)
{
  _renderTargetTexture->setRenderingAutoClearDepthStencil(renderingGroupId, autoClearDepthStencil);
}

void ReflectionProbe::dispose()
{
  // Remove from the scene if found
  stl_util::remove_vector_elements_equal_sharedptr(_scene->reflectionProbes, this);

  if (_renderTargetTexture) {
    _renderTargetTexture->dispose();
    _renderTargetTexture = nullptr;
  }
}

std::string ReflectionProbe::toString(bool fullDetails)
{
  std::ostringstream ret;
  ret << "Name: " << name;

  if (fullDetails) {
    ret << ", position: " << position.toString();

    if (_attachedMesh) {
      ret << ", attached mesh: " << _attachedMesh->name;
    }
  }

  return ret.str();
}

std::string ReflectionProbe::getClassName() const
{
  return "ReflectionProbe";
}

void ReflectionProbe::serialize(json& /*serializationObject*/)
{
}

ReflectionProbePtr ReflectionProbe::Parse(const json& /*parsedReflectionProbe*/, Scene* /*scene*/,
                                          const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
