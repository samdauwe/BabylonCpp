#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>

namespace BABYLON {

PostProcessRenderEffect::PostProcessRenderEffect(
  Engine* engine, const string_t& name,
  const ::std::function<PostProcess*()>& getPostProcess, bool singleInstance)
    : _name{name}
    , _engine{engine}
    , _getPostProcess{getPostProcess}
    , _singleInstance{singleInstance}
{
}

PostProcessRenderEffect::~PostProcessRenderEffect()
{
}

bool PostProcessRenderEffect::isSupported() const
{
  for (auto& item : _postProcesses) {
    if (!item.second->isSupported()) {
      return false;
    }
  }
  return true;
}

void PostProcessRenderEffect::_update()
{
  for (auto& item : _renderPasses) {
    item.second->_update();
  }
}

void PostProcessRenderEffect::addPass(PostProcessRenderPass* renderPass)
{
  _renderPasses[renderPass->_name] = renderPass;

  _linkParameters();
}

void PostProcessRenderEffect::removePass(PostProcessRenderPass* renderPass)
{
  _renderPasses.erase(renderPass->_name);

  _linkParameters();
}

void PostProcessRenderEffect::addRenderEffectAsPass(
  PostProcessRenderEffect* renderEffect)
{
  _renderEffectAsPasses[renderEffect->_name] = renderEffect;

  _linkParameters();
}

PostProcessRenderPass*
PostProcessRenderEffect::getPass(const string_t& passName)
{
  if (stl_util::contains(_renderPasses, passName)) {
    return _renderPasses[passName];
  }

  return nullptr;
}

void PostProcessRenderEffect::emptyPasses()
{
  _renderPasses.clear();

  _linkParameters();
}

void PostProcessRenderEffect::_attachCameras(const vector_t<Camera*>& cameras)
{
  string_t cameraKey;

  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    auto cameraName = camera->name;

    if (_singleInstance) {
      cameraKey = "0";
    }
    else {
      cameraKey = cameraName;
    }

    if (stl_util::contains(_postProcesses, cameraKey)) {
      _postProcesses[cameraKey] = _postProcesses[cameraKey];
    }
    else {
      _postProcesses[cameraKey] = _getPostProcess();
    }

    auto index = camera->attachPostProcess(_postProcesses[cameraKey]);

    if (!stl_util::contains(_indicesForCamera, cameraName)) {
      _indicesForCamera[cameraName].clear();
    }

    _indicesForCamera[cameraName].emplace_back(index);

    if (!stl_util::contains(_cameras, camera->name)) {
      _cameras[cameraName] = camera;
    }

    for (auto& pass : _renderPasses) {
      pass.second->_incRefCount();
    }
  }

  _linkParameters();
}

void PostProcessRenderEffect::_detachCameras(const vector_t<Camera*>& cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    auto cameraName = camera->name;

    camera->detachPostProcess(
      _postProcesses[_singleInstance ? "0" : cameraName],
      _indicesForCamera[cameraName]);

    _cameras.erase(cameraName);
    _indicesForCamera.erase(cameraName);

    for (auto& item : _renderPasses) {
      item.second->_decRefCount();
    }
  }
}

void PostProcessRenderEffect::_enable(const vector_t<Camera*> cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    auto cameraName = camera->name;

    for (auto& j : _indicesForCamera[cameraName]) {
      auto index = _indicesForCamera[cameraName][j];
      if (index >= camera->_postProcesses.size()
          || camera->_postProcesses[index] == nullptr) {
        camera->attachPostProcess(
          _postProcesses[_singleInstance ? "0" : cameraName],
          static_cast<int>(_indicesForCamera[cameraName][j]));
      }
    }

    for (auto& item : _renderPasses) {
      item.second->_incRefCount();
    }
  }
}

void PostProcessRenderEffect::_disable(vector_t<Camera*> cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    auto cameraName = camera->name;

    camera->detachPostProcess(
      _postProcesses[_singleInstance ? "0" : cameraName],
      _indicesForCamera[cameraName]);

    for (auto& item : _renderPasses) {
      item.second->_decRefCount();
    }
  }
}

PostProcess* PostProcessRenderEffect::getPostProcess(Camera* camera)
{
  if (_singleInstance) {
    return _postProcesses["0"];
  }
  else {
    return camera ? _postProcesses[camera->name] : nullptr;
  }
}

void PostProcessRenderEffect::_linkParameters()
{
  for (auto& item : _postProcesses) {
    if (applyParameters) {
      applyParameters(item.second);
    }

    item.second->onBeforeRenderObservable.add(
      [this](Effect* effect) { _linkTextures(effect); });
  }
}

void PostProcessRenderEffect::_linkTextures(Effect* effect)
{
  for (auto& item : _renderPasses) {
    effect->setTexture(item.first, item.second->getRenderTexture());
  }

  for (auto& item : _renderEffectAsPasses) {
    effect->setTextureFromPostProcess(item.first + "Sampler",
                                      item.second->getPostProcess());
  }
}

} // end of namespace BABYLON
