#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderEffect {

public:
  PostProcessRenderEffect(Engine* engine, const string_t& name,
                          const ::std::function<PostProcess*()>& getPostProcess,
                          bool singleInstance = true);
  ~PostProcessRenderEffect();

  bool isSupported() const;
  void _update();
  void addPass(PostProcessRenderPass* renderPass);
  void removePass(PostProcessRenderPass* renderPass);
  void addRenderEffectAsPass(PostProcessRenderEffect* renderEffect);
  PostProcessRenderPass* getPass(const string_t& passName);
  void emptyPasses();
  void _attachCameras(const vector_t<Camera*>& cameras);
  void _detachCameras(const vector_t<Camera*>& cameras);
  void _enable(const vector_t<Camera*> cameras);
  void _disable(vector_t<Camera*> cameras);

private:
  PostProcess* getPostProcess(Camera* camera = nullptr);
  void _linkParameters();
  void _linkTextures(Effect* effect);

public:
  string_t _name;
  ::std::function<void(PostProcess* postProcess)> applyParameters;
  Engine* _engine;

private:
  unordered_map_t<string_t, PostProcess*> _postProcesses;
  ::std::function<PostProcess*()> _getPostProcess;
  bool _singleInstance;
  unordered_map_t<string_t, Camera*> _cameras;
  unordered_map_t<string_t, IndicesArray> _indicesForCamera;
  unordered_map_t<string_t, PostProcessRenderPass*> _renderPasses;
  unordered_map_t<string_t, PostProcessRenderEffect*> _renderEffectAsPasses;

}; // end of class PostProcessRenderEffect

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H
