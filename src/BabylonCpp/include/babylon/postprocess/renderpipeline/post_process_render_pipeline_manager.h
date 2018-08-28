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
  void attachCamerasToRenderPipeline(const string_t& renderPipelineName,
                                     const vector_t<CameraPtr>& cameras,
                                     bool unique = false);
  void detachCamerasFromRenderPipeline(const string_t& renderPipelineName,
                                       const vector_t<CameraPtr>& cameras);
  void enableEffectInPipeline(const string_t& renderPipelineName,
                              const string_t& renderEffectName,
                              const vector_t<CameraPtr>& cameras);
  void disableEffectInPipeline(const string_t& renderPipelineName,
                               const string_t& renderEffectName,
                               const vector_t<CameraPtr>& cameras);
  void update();
  /** Hidden */
  void _rebuild();
  void dispose();

private:
  unordered_map_t<string_t, PostProcessRenderPipeline*> _renderPipelines;

}; // end of class PostProcessRenderPipelineManager

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
