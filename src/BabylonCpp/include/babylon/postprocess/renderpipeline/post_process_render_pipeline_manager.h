#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Camera;
class PostProcessRenderPipeline;
using CameraPtr                    = std::shared_ptr<Camera>;
using PostProcessRenderPipelinePtr = std::shared_ptr<PostProcessRenderPipeline>;

/**
 * @brief PostProcessRenderPipelineManager class
 * @see https://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipelineManager {

public:
  /**
   * Initializes a PostProcessRenderPipelineManager.
   * @see https://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
   */
  PostProcessRenderPipelineManager();
  ~PostProcessRenderPipelineManager();

  /**
   * @brief Adds a pipeline to the manager.
   * @param renderPipeline The pipeline to add
   */
  void addPipeline(const PostProcessRenderPipelinePtr& renderPipeline);

  /**
   * @brief Attaches a camera to the pipeline.
   * @param renderPipelineName The name of the pipeline to attach to
   * @param cameras the camera to attach
   * @param unique if the camera can be attached multiple times to the pipeline
   */
  void attachCamerasToRenderPipeline(const std::string& renderPipelineName,
                                     const CameraPtr& camera,
                                     bool unique = false);

  /**
   * @brief Attaches a camera to the pipeline.
   * @param renderPipelineName The name of the pipeline to attach to
   * @param cameras the camera to attach
   * @param unique if the camera can be attached multiple times to the pipeline
   */
  void attachCamerasToRenderPipeline(const std::string& renderPipelineName,
                                     const std::vector<CameraPtr>& cameras,
                                     bool unique = false);

  /**
   * @brief Detaches a camera from the pipeline.
   * @param renderPipelineName The name of the pipeline to detach from
   * @param cameras the camera to detach
   */
  void detachCamerasFromRenderPipeline(const std::string& renderPipelineName,
                                       const CameraPtr& camera);

  /**
   * @brief Detaches a camera from the pipeline.
   * @param renderPipelineName The name of the pipeline to detach from
   * @param cameras the camera to detach
   */
  void detachCamerasFromRenderPipeline(const std::string& renderPipelineName,
                                       const std::vector<CameraPtr>& cameras);

  /**
   * @brief Enables an effect by name on a pipeline.
   * @param renderPipelineName the name of the pipeline to enable the effect in
   * @param renderEffectName the name of the effect to enable
   * @param cameras the cameras that the effect should be enabled on
   */
  void enableEffectInPipeline(const std::string& renderPipelineName,
                              const std::string& renderEffectName,
                              const CameraPtr& camera);

  /**
   * @brief Enables an effect by name on a pipeline.
   * @param renderPipelineName the name of the pipeline to enable the effect in
   * @param renderEffectName the name of the effect to enable
   * @param cameras the cameras that the effect should be enabled on
   */
  void enableEffectInPipeline(const std::string& renderPipelineName,
                              const std::string& renderEffectName,
                              const std::vector<CameraPtr>& cameras);

  /**
   * @brief Disables an effect by name on a pipeline.
   * @param renderPipelineName the name of the pipeline to disable the effect in
   * @param renderEffectName the name of the effect to disable
   * @param cameras the cameras that the effect should be disabled on
   */
  void disableEffectInPipeline(const std::string& renderPipelineName,
                               const std::string& renderEffectName,
                               const CameraPtr& camera);

  /**
   * @brief Disables an effect by name on a pipeline.
   * @param renderPipelineName the name of the pipeline to disable the effect in
   * @param renderEffectName the name of the effect to disable
   * @param cameras the cameras that the effect should be disabled on
   */
  void disableEffectInPipeline(const std::string& renderPipelineName,
                               const std::string& renderEffectName,
                               const std::vector<CameraPtr>& cameras);

  /**
   * @brief Updates the state of all contained render pipelines and disposes of
   * any non supported pipelines.
   */
  void update();

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Disposes of the manager and pipelines.
   */
  void dispose();

private:
  std::unordered_map<std::string, PostProcessRenderPipelinePtr>
    _renderPipelines;

}; // end of class PostProcessRenderPipelineManager

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_MANAGER_H
