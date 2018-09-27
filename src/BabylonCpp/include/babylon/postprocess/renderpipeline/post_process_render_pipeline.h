#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

class Camera;
class Engine;
class PostProcessRenderEffect;
using CameraPtr = std::shared_ptr<Camera>;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipeline : public IDisposable {

public:
  PostProcessRenderPipeline(Engine* engine, const std::string& name);
  virtual ~PostProcessRenderPipeline();

  /**
   * Returns the string "PostProcessRenderPipeline"
   */
  const char* getClassName() const;

  std::vector<CameraPtr> getCameras() const;
  bool isSupported() const;
  void addEffect(PostProcessRenderEffect* renderEffect);
  /** Hidden */
  virtual void _rebuild();
  /** Hidden */
  void _enableEffect(const std::string& renderEffectName,
                     const std::vector<CameraPtr>& cameras);
  void _disableEffect(const std::string& renderEffectName,
                      const std::vector<CameraPtr>& cameras);
  /** Hidden */
  void _attachCameras(const std::vector<CameraPtr>& cameras, bool unique);
  /** Hidden */
  void _detachCameras(const std::vector<CameraPtr>& cameras);
  /** Hidden */
  void _update();
  /** Hidden */
  void _reset();
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

protected:
  bool _enableMSAAOnFirstPostProcess(unsigned int sampleCount);

public:
  /** Hidden */
  std::string _name;

protected:
  std::unordered_map<std::string, CameraPtr> _cameras;

private:
  Engine* engine;
  std::unordered_map<std::string, PostProcessRenderEffect*> _renderEffects;
  std::unordered_map<std::string, PostProcessRenderEffect*>
    _renderEffectsForIsolatedPass;

}; // end of class PostProcessRenderPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
