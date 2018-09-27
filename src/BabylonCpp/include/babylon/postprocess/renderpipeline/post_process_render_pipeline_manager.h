#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Camera;
class PostProcessRenderPipeline;
using CameraPtr = std::shared_ptr<Camera>;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipelineManager {

public:
  PostProcessRenderPipelineManager();
  ~PostProcessRenderPipelineManager();

  void addPipeline(PostProcessRenderPipeline* renderPipeline);
  void attachCamerasToRenderPipeline(const std::string& renderPipelineName,
                                     const std::vector<CameraPtr>& cameras,
                                     bool unique = false);
  void detachCamerasFromRenderPipeline(const std::string& renderPipelineName,
                                       const std::vector<CameraPtr>& cameras);
  void enableEffectInPipeline(const std::string& renderPipelineName,
                              const std::string& renderEffectName,
                              const std::vector<CameraPtr>& cameras);
  void disableEffectInPipeline(const std::string& renderPipelineName,
                               const std::string& renderEffectName,
                               const std::vector<CameraPtr>& cameras);
  void update();
  /** Hidden */
  void _rebuild();
  void dispose();

private:
  std::unordered_map<std::string, PostProcessRenderPipeline*> _renderPipelines;

}; // end of class PostProcessRenderPipelineManager

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
