#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

class Camera;
class Engine;
class PostProcessRenderEffect;
class PostProcessRenderPipeline;
using CameraPtr                    = std::shared_ptr<Camera>;
using PostProcessRenderEffectPtr   = std::shared_ptr<PostProcessRenderEffect>;
using PostProcessRenderPipelinePtr = std::shared_ptr<PostProcessRenderPipeline>;

/**
 * @brief PostProcessRenderPipeline
 * @see https://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipeline : public IDisposable {

public:
  template <typename... Ts>
  static PostProcessRenderPipelinePtr New(Ts&&... args)
  {
    return std::shared_ptr<PostProcessRenderPipeline>(
      new PostProcessRenderPipeline(std::forward<Ts>(args)...));
  }
  virtual ~PostProcessRenderPipeline();

  /**
   * "PostProcessRenderPipeline"
   * @returns "PostProcessRenderPipeline"
   */
  const char* getClassName() const;

  std::vector<CameraPtr> getCameras() const;

  /**
   * @brief If all the render effects in the pipeline are support.
   */
  bool isSupported() const;

  /**
   * @brief Adds an effect to the pipeline.
   * @param renderEffect the effect to add
   */
  void addEffect(const PostProcessRenderEffectPtr& renderEffect);

  /**
   * @brief Hidden
   */
  virtual void _rebuild();

  /**
   * @brief Hidden
   */
  void _enableEffect(const std::string& renderEffectName,
                     const std::vector<CameraPtr>& cameras);

  /**
   * @brief Hidden
   */
  void _disableEffect(const std::string& renderEffectName,
                      const std::vector<CameraPtr>& cameras);

  /**
   * @brief Hidden
   */
  void _attachCameras(const std::vector<CameraPtr>& cameras, bool unique);

  /**
   * @brief Hidden
   */
  void _detachCameras(const std::vector<CameraPtr>& cameras);

  /**
   * @brief Hidden
   */
  void _update();

  /**
   * @brief Hidden
   */
  void _reset();

  /**
   * @brief Disposes of the pipeline.
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Initializes a PostProcessRenderPipeline.
   * @param engine engine to add the pipeline to
   * @param name name of the pipeline
   */
  PostProcessRenderPipeline(Engine* engine, const std::string& name);

  bool _enableMSAAOnFirstPostProcess(unsigned int sampleCount);

public:
  /**
   * Hidden
   */
  std::string _name;

protected:
  /**
   * Hidden
   */
  std::unordered_map<std::string, CameraPtr> _cameras;

private:
  Engine* engine;
  std::unordered_map<std::string, PostProcessRenderEffectPtr> _renderEffects;
  std::unordered_map<std::string, PostProcessRenderEffectPtr>
    _renderEffectsForIsolatedPass;

}; // end of class PostProcessRenderPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
