#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

PostProcessRenderPipelineManager::PostProcessRenderPipelineManager() = default;

PostProcessRenderPipelineManager::~PostProcessRenderPipelineManager() = default;

std::vector<PostProcessRenderPipelinePtr> PostProcessRenderPipelineManager::supportedPipelines()
{
  std::vector<PostProcessRenderPipelinePtr> result;

  for (const auto& renderPipelineItem : _renderPipelines) {
    const auto pipeline = renderPipelineItem.second;
    if (pipeline->isSupported()) {
      result.emplace_back(pipeline);
    }
  }

  return result;
}

void PostProcessRenderPipelineManager::addPipeline(
  const PostProcessRenderPipelinePtr& renderPipeline)
{
  _renderPipelines[renderPipeline->_name] = renderPipeline;
}

void PostProcessRenderPipelineManager::attachCamerasToRenderPipeline(
  const std::string& renderPipelineName, const CameraPtr& camera, bool unique)
{
  std::vector<CameraPtr> cameras{camera};
  attachCamerasToRenderPipeline(renderPipelineName, cameras, unique);
}

void PostProcessRenderPipelineManager::attachCamerasToRenderPipeline(
  const std::string& renderPipelineName, const std::vector<CameraPtr>& cameras, bool unique)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)
      || !_renderPipelines[renderPipelineName]) {
    return;
  }

  _renderPipelines[renderPipelineName]->_attachCameras(cameras, unique);
}

void PostProcessRenderPipelineManager::detachCamerasFromRenderPipeline(
  const std::string& renderPipelineName, const CameraPtr& camera)
{
  std::vector<CameraPtr> cameras{camera};
  detachCamerasFromRenderPipeline(renderPipelineName, cameras);
}

void PostProcessRenderPipelineManager::detachCamerasFromRenderPipeline(
  const std::string& renderPipelineName, const std::vector<CameraPtr>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)
      || !_renderPipelines[renderPipelineName]) {
    return;
  }

  _renderPipelines[renderPipelineName]->_detachCameras(cameras);
}

void PostProcessRenderPipelineManager::enableEffectInPipeline(const std::string& renderPipelineName,
                                                              const std::string& renderEffectName,
                                                              const CameraPtr& camera)
{
  std::vector<CameraPtr> cameras{camera};
  enableEffectInPipeline(renderPipelineName, renderEffectName, cameras);
}

void PostProcessRenderPipelineManager::enableEffectInPipeline(const std::string& renderPipelineName,
                                                              const std::string& renderEffectName,
                                                              const std::vector<CameraPtr>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)
      || !_renderPipelines[renderPipelineName]) {
    return;
  }

  _renderPipelines[renderPipelineName]->_enableEffect(renderEffectName, cameras);
}

void PostProcessRenderPipelineManager::disableEffectInPipeline(
  const std::string& renderPipelineName, const std::string& renderEffectName,
  const CameraPtr& camera)
{
  std::vector<CameraPtr> cameras{camera};
  disableEffectInPipeline(renderPipelineName, renderEffectName, cameras);
}

void PostProcessRenderPipelineManager::disableEffectInPipeline(
  const std::string& renderPipelineName, const std::string& renderEffectName,
  const std::vector<CameraPtr>& cameras)
{
  if (!stl_util::contains(_renderPipelines, renderPipelineName)
      || !_renderPipelines[renderPipelineName]) {
    return;
  }

  _renderPipelines[renderPipelineName]->_disableEffect(renderEffectName, cameras);
}

void PostProcessRenderPipelineManager::update()
{
  std::vector<std::string> pipelinesToRemove;
  for (const auto& [renderPipelineName, pipeline] : _renderPipelines) {
    if (!pipeline->isSupported()) {
      pipeline->dispose();
      pipelinesToRemove.emplace_back(renderPipelineName);
    }
    else {
      pipeline->_update();
    }
  }

  for (const auto& renderPipelineName : pipelinesToRemove) {
    _renderPipelines.erase(renderPipelineName);
  }
}

void PostProcessRenderPipelineManager::_rebuild()
{
  for (const auto& item : _renderPipelines) {
    auto& pipeline = item.second;
    pipeline->_rebuild();
  }
}

void PostProcessRenderPipelineManager::dispose()
{
  for (const auto& item : _renderPipelines) {
    auto& pipeline = item.second;
    pipeline->dispose();
  }
}

} // end of namespace BABYLON
