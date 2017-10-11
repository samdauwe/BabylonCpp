#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/postprocess/display_pass_post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>

namespace BABYLON {

PostProcessRenderPipeline::PostProcessRenderPipeline(Engine* engine,
                                                     const string_t& name)

    : _name{name}, _engine{engine}
{
}

vector_t<Camera*> PostProcessRenderPipeline::getCameras() const
{
  vector_t<Camera*> cameras;
  cameras.reserve(_cameras.size());
  for (auto& item : _cameras) {
    cameras.emplace_back(item.second);
  }
  return cameras;
}

bool PostProcessRenderPipeline::isSupported() const
{
  for (auto& item : _renderEffects) {
    if (!item.second->isSupported()) {
      return false;
    }
  }
  return true;
}

PostProcessRenderPipeline::~PostProcessRenderPipeline()
{
}

const char* PostProcessRenderPipeline::getClassName() const
{
  return "PostProcessRenderPipeline";
}

void PostProcessRenderPipeline::addEffect(PostProcessRenderEffect* renderEffect)
{
  _renderEffects[renderEffect->_name] = renderEffect;
}

void PostProcessRenderPipeline::_rebuild()
{
}

void PostProcessRenderPipeline::_enableEffect(const string_t& renderEffectName,
                                              const vector_t<Camera*>& cameras)
{
  auto& renderEffects = _renderEffects[renderEffectName];

  if (!renderEffects) {
    return;
  }

  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;
  renderEffects->_enable(_cam);
}

void PostProcessRenderPipeline::_disableEffect(const string_t& renderEffectName,
                                               const vector_t<Camera*>& cameras)
{
  if (!stl_util::contains(_renderEffects, renderEffectName)) {
    return;
  }

  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;
  _renderEffects[renderEffectName]->_disable(_cam);
}

void PostProcessRenderPipeline::_attachCameras(const vector_t<Camera*>& cameras,
                                               bool unique)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  vector_t<Camera*> camerasToDelete;
  for (auto& camera : _cam) {
    const auto& cameraName = camera->name;

    if (stl_util::contains(_cameras, camera->name)) {
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

  for (auto& item : _renderEffects) {
    item.second->_attachCameras(_cam);
  }
}

void PostProcessRenderPipeline::_detachCameras(const vector_t<Camera*>& cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& item : _renderEffects) {
    item.second->_detachCameras(_cam);
  }

  for (auto& camera : _cam) {
    auto it = _cameras.find(camera->name);
    if (it != _cameras.end())
      _cameras.erase(it);
  }
}

void PostProcessRenderPipeline::_enableDisplayOnlyPass(
  const string_t& passName, const vector_t<Camera*>& cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  PostProcessRenderPass* pass = nullptr;
  for (auto& item : _renderEffects) {
    pass = item.second->getPass(passName);

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

  pass->_name = string_t(PostProcessRenderPipeline::PASS_SAMPLER_NAME);

  for (auto& camera : _cam) {
    const auto& cameraName = camera->name;
    if (!stl_util::contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName] = new PostProcessRenderEffect(
        _engine, PostProcessRenderPipeline::PASS_EFFECT_NAME, [this]() {
          return new DisplayPassPostProcess(
            PostProcessRenderPipeline::PASS_EFFECT_NAME, 1.f, nullptr, 0,
            _engine, true);
        });
    }
    _renderEffectsForIsolatedPass[cameraName]->emptyPasses();
    _renderEffectsForIsolatedPass[cameraName]->addPass(pass);
    _renderEffectsForIsolatedPass[cameraName]->_attachCameras({camera});
  }
}

void PostProcessRenderPipeline::_disableDisplayOnlyPass(
  const string_t& /*passName*/, const vector_t<Camera*>& cameras)
{
  auto _cam = cameras.empty() ? stl_util::extract_values(_cameras) : cameras;

  for (auto& camera : _cam) {
    const auto& cameraName = camera->name;
    if (!stl_util::contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName] = new PostProcessRenderEffect(
        _engine, PostProcessRenderPipeline::PASS_EFFECT_NAME, [this]() {
          return new DisplayPassPostProcess(
            PostProcessRenderPipeline::PASS_EFFECT_NAME, 1.f, nullptr, 0,
            _engine, true);
        });
    }
    _renderEffectsForIsolatedPass[cameraName]->_disable({camera});
  }

  for (auto& item : _renderEffects) {
    item.second->_enable(_cam);
  }
}

void PostProcessRenderPipeline::_update()
{
  for (auto& item : _renderEffects) {
    item.second->_update();
  }

  for (auto& item : _cameras) {
    const auto& cameraName = item.first;
    if (stl_util::contains(_renderEffectsForIsolatedPass, cameraName)) {
      _renderEffectsForIsolatedPass[cameraName]->_update();
    }
  }
}

void PostProcessRenderPipeline::_reset()
{
  _renderEffects.clear();
  _renderEffectsForIsolatedPass.clear();
}

void PostProcessRenderPipeline::dispose(bool /*doNotRecurse*/)
{
  // Must be implemented by children
}

} // end of namespace BABYLON
