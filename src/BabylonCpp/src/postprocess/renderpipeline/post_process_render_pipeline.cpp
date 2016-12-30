#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

#include <babylon/cameras/camera.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>
#include <babylon/postprocess/display_pass_post_process.h>

namespace BABYLON {

PostProcessRenderPipeline::PostProcessRenderPipeline(Engine* engine,
                                                     const std::string& name)

    : _name{name}, _engine{engine}
{
}

PostProcessRenderPipeline::~PostProcessRenderPipeline()
{
}

void PostProcessRenderPipeline::addEffect(PostProcessRenderEffect* renderEffect)
{
  _renderEffects[renderEffect->_name] = renderEffect;
}

void PostProcessRenderPipeline::_enableEffect(
  const std::string& renderEffectName, const std::vector<Camera*>& cameras)
{
  PostProcessRenderEffect* renderEffects = _renderEffects[renderEffectName];

  if (!renderEffects) {
    return;
  }

  renderEffects->_enable(cameras);
}

void PostProcessRenderPipeline::_disableEffect(
  const std::string& renderEffectName, const std::vector<Camera*>& cameras)
{
  if (!std_util::contains(_renderEffects, renderEffectName)) {
    return;
  }

  _renderEffects[renderEffectName]->_disable(cameras);
}

void PostProcessRenderPipeline::_attachCameras(
  const std::vector<Camera*>& cameras, bool unique)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  std::vector<Camera*> camerasToDelete;

  for (auto& camera : _cam) {
    std::string cameraName = camera->name;

    if (_cameras.find(camera->name) == _cameras.end()) {
      _cameras[cameraName] = camera;
    }
    else if (unique) {
      camerasToDelete.emplace_back(camera);
    }
  }

  for (auto& camera : camerasToDelete) {
    auto it = _cameras.find(camera->name);
    if (it != _cameras.end())
      _cameras.erase(it);
  }

  for (auto renderEffect : _renderEffects) {
    renderEffect.second->_attachCameras(_cam);
  }
}

void PostProcessRenderPipeline::_detachCameras(
  const std::vector<Camera*>& cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  for (auto& renderEffect : _renderEffects) {
    renderEffect.second->_detachCameras(_cam);
  }

  for (auto& camera : _cam) {
    auto it = _cameras.find(camera->name);
    if (it != _cameras.end())
      _cameras.erase(it);
  }
}

void PostProcessRenderPipeline::_enableDisplayOnlyPass(
  const std::string& passName, const std::vector<Camera*>& cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  PostProcessRenderPass* pass = nullptr;

  for (auto& renderEffect : _renderEffects) {
    pass = renderEffect.second->getPass(passName);

    if (pass != nullptr) {
      break;
    }
  }

  if (pass == nullptr) {
    return;
  }

  for (auto& renderEffect : _renderEffects) {
    renderEffect.second->_disable(_cam);
  }

  pass->_name = std::string(PostProcessRenderPipeline::PASS_SAMPLER_NAME);

  //for (auto& camera : _cam) {
    //std::string cameraName = camera->name;

    // TODO FIXME
    //std::cout << cameraName << std::endl;
    /*if (!contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName] = new PostProcessRenderEffect(
        _engine, PostProcessRenderPipeline::PASS_EFFECT_NAME, [this]() {
          return new DisplayPassPostProcess(
            PostProcessRenderPipeline::PASS_EFFECT_NAME, 1.0, nullptr, nullptr,
            _engine, true);
        });
    }
    _renderEffectsForIsolatedPass[cameraName]->emptyPasses();
    _renderEffectsForIsolatedPass[cameraName]->addPass(pass);
    _renderEffectsForIsolatedPass[cameraName]->_attachCameras(camera);*/
  //}
}

void PostProcessRenderPipeline::_disableDisplayOnlyPass(
  const std::string& /*passName*/, const std::vector<Camera*>& cameras)
{
  std::vector<Camera*> _cam
    = cameras.empty() ? std_util::extract_values(_cameras) : cameras;

  //for (auto& camera : _cam) {
    //std::string cameraName = camera->name;

    // TODO FIXME
    //std::cout << cameraName << std::endl;
    /*if (!contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName] = new PostProcessRenderEffect(
        _engine, PostProcessRenderPipeline::PASS_EFFECT_NAME, [this]() {
          return new DisplayPassPostProcess(
            PostProcessRenderPipeline::PASS_EFFECT_NAME, 1.0, nullptr, nullptr,
            _engine, true);
        });
    }
    _renderEffectsForIsolatedPass[cameraName]->_disable(camera);*/
  //}

  for (auto& element : _renderEffects) {
    element.second->_enable(_cam);
  }
}

void PostProcessRenderPipeline::_update()
{
  for (auto& element : _renderEffects) {
    element.second->_update();
  }

  for (auto& element : _cameras) {
    std::string cameraName = element.first;
    if (std_util::contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName]->_update();
    }
  }
}

} // end of namespace BABYLON
