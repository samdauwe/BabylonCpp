#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>

namespace BABYLON {

PostProcessRenderEffect::PostProcessRenderEffect(
  Engine* engine, const std::string& name,
  const std::function<PostProcess*()>& getPostProcess, bool singleInstance)
    : _name{name}
    , _engine{engine}
    , _getPostProcess{getPostProcess}
    , _singleInstance{singleInstance}
{
}

PostProcessRenderEffect::~PostProcessRenderEffect()
{
}

void PostProcessRenderEffect::_update()
{
  for (auto& element : _renderPasses) {
    element.second->_update();
  }
}

void PostProcessRenderEffect::addPass(PostProcessRenderPass* renderPass)
{
  _renderPasses[renderPass->_name] = renderPass;

  _linkParameters();
}

void PostProcessRenderEffect::removePass(PostProcessRenderPass* /*renderPass*/)
{
  // TODO FIXME
  // delete _renderPasses[renderPass->_name];

  _linkParameters();
}

void PostProcessRenderEffect::addRenderEffectAsPass(
  PostProcessRenderEffect* renderEffect)
{
  _renderEffectAsPasses[renderEffect->_name] = renderEffect;

  _linkParameters();
}

PostProcessRenderPass*
PostProcessRenderEffect::getPass(const std::string& passName)
{
  if (std_util::contains(_renderPasses, passName))
    return _renderPasses[passName];

  return nullptr;
}

void PostProcessRenderEffect::emptyPasses()
{
  _renderPasses.clear();

  _linkParameters();
}

void PostProcessRenderEffect::_attachCameras(std::vector<Camera*>& cameras)
{
  std::string cameraKey;

  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    std::string cameraName = camera->name;

    if (_singleInstance) {
      cameraKey = "0";
    }
    else {
      cameraKey = cameraName;
    }

    if (std_util::contains(_postProcesses, cameraKey))
      _postProcesses[cameraKey] = _postProcesses[cameraKey];
    else
      _postProcesses[cameraKey] = _getPostProcess();

    int index = camera->attachPostProcess(_postProcesses[cameraKey]);

    if (!std_util::contains(_indicesForCamera, cameraName)) {
      _indicesForCamera[cameraName].clear();
    }

    _indicesForCamera[cameraName].emplace_back(index);

    if (!std_util::contains(_cameras, camera->name)) {
      _cameras[cameraName] = camera;
    }

    for (auto& pass : _renderPasses) {
      pass.second->_incRefCount();
    }
  }

  _linkParameters();
}

void PostProcessRenderEffect::_detachCameras(std::vector<Camera*>& cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    std::string cameraName = camera->name;

    camera->detachPostProcess(_postProcesses[_singleInstance ? 0 : cameraName],
                              _indicesForCamera[cameraName]);

    _cameras.erase(cameraName);
    _indicesForCamera.erase(cameraName);

    for (auto& pass : _renderPasses) {
      pass.second->_decRefCount();
    }
  }
}

void PostProcessRenderEffect::_enable(const std::vector<Camera*> cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    std::string cameraName = camera->name;

    for (auto& j : _indicesForCamera[cameraName]) {
      if (camera->_postProcesses[_indicesForCamera[cameraName][j]] == nullptr) {
        camera->attachPostProcess(
          _postProcesses[_singleInstance ? "0" : cameraName],
          _indicesForCamera[cameraName][j]);
      }
    }

    for (auto& pass : _renderPasses) {
      pass.second->_incRefCount();
    }
  }
}

void PostProcessRenderEffect::_disable(std::vector<Camera*> cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    std::string cameraName = camera->name;

    camera->detachPostProcess(
      _postProcesses[_singleInstance ? "0" : cameraName],
      _indicesForCamera[cameraName]);

    for (auto& pass : _renderPasses) {
      pass.second->_decRefCount();
    }
  }
}

PostProcess* PostProcessRenderEffect::getPostProcess(Camera* camera)
{
  if (_singleInstance) {
    return _postProcesses["0"];
  }
  else {
    return _postProcesses[camera->name];
  }
}

void PostProcessRenderEffect::_linkParameters()
{
  for (auto& element : _postProcesses) {
    if (applyParameters) {
      applyParameters(element.second);
    }

    // element.second->onBeforeRender
    //  = [this](Effect* effect) { _linkTextures(effect); };
  }
}

void PostProcessRenderEffect::_linkTextures(Effect* /*effect*/)
{
  // TODO FIXME
  //std::cout << (effect == nullptr) << std::endl;
  /*for (auto& element : _renderPasses) {
    effect->setTexture(element.first, static_cast<BaseTexture*>(
                                        element.second->getRenderTexture()));
  }

  for (auto& element : _renderEffectAsPasses) {
    effect->setTextureFromPostProcess(concatString(element.first, "Sampler"),
                                      element.second->getPostProcess());
  }*/
}

} // end of namespace BABYLON
