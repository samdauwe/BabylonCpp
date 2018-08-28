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
  PostProcessRenderPipeline(Engine* engine, const string_t& name);
  virtual ~PostProcessRenderPipeline();

  /**
   * Returns the string "PostProcessRenderPipeline"
   */
  const char* getClassName() const;

  vector_t<CameraPtr> getCameras() const;
  bool isSupported() const;
  void addEffect(PostProcessRenderEffect* renderEffect);
  /** Hidden */
  virtual void _rebuild();
  /** Hidden */
  void _enableEffect(const string_t& renderEffectName,
                     const vector_t<CameraPtr>& cameras);
  void _disableEffect(const string_t& renderEffectName,
                      const vector_t<CameraPtr>& cameras);
  /** Hidden */
  void _attachCameras(const vector_t<CameraPtr>& cameras, bool unique);
  /** Hidden */
  void _detachCameras(const vector_t<CameraPtr>& cameras);
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
  string_t _name;

protected:
  unordered_map_t<string_t, CameraPtr> _cameras;

private:
  Engine* engine;
  unordered_map_t<string_t, PostProcessRenderEffect*> _renderEffects;
  unordered_map_t<string_t, PostProcessRenderEffect*>
    _renderEffectsForIsolatedPass;

}; // end of class PostProcessRenderPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PIPELINE_H
