#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

PostProcessRenderPipelineManager::PostProcessRenderPipelineManager()
{
}

PostProcessRenderPipelineManager::~PostProcessRenderPipelineManager()
{
}

void PostProcessRenderPipelineManager::addPipeline(
  PostProcessRenderPipeline* renderPipeline)
{
  _renderPipelines[renderPipeline->_name] = renderPipeline;
}

void PostProcessRenderPipelineManager::attachCamerasToRenderPipeline(
  const std::string& renderPipelineName, const std::vector<Camera*>& cameras,
  bool unique)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_attachCameras(cameras, unique);
}

void PostProcessRenderPipelineManager::detachCamerasFromRenderPipeline(
  const std::string& renderPipelineName, const std::vector<Camera*>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_detachCameras(cameras);
}

void PostProcessRenderPipelineManager::enableEffectInPipeline(
  const std::string& renderPipelineName, const std::string& renderEffectName,
  const std::vector<Camera*>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_enableEffect(renderEffectName,
                                                      cameras);
}

void PostProcessRenderPipelineManager::disableEffectInPipeline(
  const std::string& renderPipelineName, const std::string& renderEffectName,
  const std::vector<Camera*>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_disableEffect(renderEffectName,
                                                       cameras);
}

void PostProcessRenderPipelineManager::enableDisplayOnlyPassInPipeline(
  const std::string& renderPipelineName, const std::string& passName,
  const std::vector<Camera*>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_enableDisplayOnlyPass(passName,
                                                               cameras);
}

void PostProcessRenderPipelineManager::disableDisplayOnlyPassInPipeline(
  const std::string& renderPipelineName, const std::string& passName,
  const std::vector<Camera*>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)) {
    return;
  }

  _renderPipelines[renderPipelineName]->_disableDisplayOnlyPass(passName,
                                                                cameras);
}

void PostProcessRenderPipelineManager::update()
{
  std::vector<std::string> pipelinesToRemove;
  for (auto& item : _renderPipelines) {
    auto& pipeline = item.second;
    if (!pipeline->isSupported()) {
      pipeline->dispose();
      pipelinesToRemove.emplace_back(item.first);
    }
    else {
      pipeline->_update();
    }
  }

  for (auto& renderPipelineName : pipelinesToRemove) {
    _renderPipelines.erase(renderPipelineName);
  }
}

} // end of namespace BABYLON
