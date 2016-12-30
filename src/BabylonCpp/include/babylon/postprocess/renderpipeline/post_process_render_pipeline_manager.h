#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipelineManager {

public:
  PostProcessRenderPipelineManager();
  ~PostProcessRenderPipelineManager();

  void addPipeline(PostProcessRenderPipeline* renderPipeline);
  void attachCamerasToRenderPipeline(const std::string& renderPipelineName,
                                     const std::vector<Camera*>& cameras,
                                     bool unique = false);
  void detachCamerasFromRenderPipeline(const std::string& renderPipelineName,
                                       const std::vector<Camera*>& cameras);
  void enableEffectInPipeline(const std::string& renderPipelineName,
                              const std::string& renderEffectName,
                              const std::vector<Camera*>& cameras);
  void disableEffectInPipeline(const std::string& renderPipelineName,
                               const std::string& renderEffectName,
                               const std::vector<Camera*>& cameras);
  void enableDisplayOnlyPassInPipeline(const std::string& renderPipelineName,
                                       const std::string& passName,
                                       const std::vector<Camera*>& cameras);
  void disableDisplayOnlyPassInPipeline(const std::string& renderPipelineName,
                                        const std::string& passName,
                                        const std::vector<Camera*>& cameras);
  void update();

private:
  std::unordered_map<std::string, PostProcessRenderPipeline*> _renderPipelines;

}; // end of class PostProcessRenderPipelineManager

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
