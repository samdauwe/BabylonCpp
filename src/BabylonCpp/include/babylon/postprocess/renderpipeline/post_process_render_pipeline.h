#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPipeline : public IDisposable {

public:
  static constexpr const char* PASS_EFFECT_NAME  = "passEffect";
  static constexpr const char* PASS_SAMPLER_NAME = "passSampler";

public:
  PostProcessRenderPipeline(Engine* engine, const std::string& name);
  virtual ~PostProcessRenderPipeline();

  std::vector<Camera*> getCameras() const;
  bool isSupported() const;
  void addEffect(PostProcessRenderEffect* renderEffect);
  void _enableEffect(const std::string& renderEffectName,
                     const std::vector<Camera*>& cameras);
  void _disableEffect(const std::string& renderEffectName,
                      const std::vector<Camera*>& cameras);
  void _attachCameras(const std::vector<Camera*>& cameras, bool unique);
  void _detachCameras(const std::vector<Camera*>& cameras);
  void _enableDisplayOnlyPass(const std::string& passName,
                              const std::vector<Camera*>& cameras);
  void _disableDisplayOnlyPass(const std::string& passName,
                               const std::vector<Camera*>& cameras);
  void _update();
  void _reset();
  virtual void dispose(bool doNotRecurse = false) override;

public:
  std::string _name;
  Engine* _engine;

protected:
  std::unordered_map<std::string, Camera*> _cameras;

private:
  std::unordered_map<std::string, PostProcessRenderEffect*> _renderEffects;
  std::unordered_map<std::string, PostProcessRenderEffect*>
    _renderEffectsForIsolatedPass;

}; // end of class PostProcessRenderPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
