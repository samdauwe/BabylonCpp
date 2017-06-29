#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderEffect {

public:
  PostProcessRenderEffect(Engine* engine, const std::string& name,
                          const std::function<PostProcess*()>& getPostProcess,
                          bool singleInstance = true);
  ~PostProcessRenderEffect();

  bool isSupported() const;
  void _update();
  void addPass(PostProcessRenderPass* renderPass);
  void removePass(PostProcessRenderPass* renderPass);
  void addRenderEffectAsPass(PostProcessRenderEffect* renderEffect);
  PostProcessRenderPass* getPass(const std::string& passName);
  void emptyPasses();
  void _attachCameras(const std::vector<Camera*>& cameras);
  void _detachCameras(const std::vector<Camera*>& cameras);
  void _enable(const std::vector<Camera*> cameras);
  void _disable(std::vector<Camera*> cameras);

private:
  PostProcess* getPostProcess(Camera* camera = nullptr);
  void _linkParameters();
  void _linkTextures(Effect* effect);

public:
  std::string _name;
  std::function<void(PostProcess* postProcess)> applyParameters;
  Engine* _engine;

private:
  std::unordered_map<std::string, PostProcess*> _postProcesses;
  std::function<PostProcess*()> _getPostProcess;
  bool _singleInstance;
  std::unordered_map<std::string, Camera*> _cameras;
  std::unordered_map<std::string, IndicesArray> _indicesForCamera;
  std::unordered_map<std::string, PostProcessRenderPass*> _renderPasses;
  std::unordered_map<std::string, PostProcessRenderEffect*>
    _renderEffectAsPasses;

}; // end of class PostProcessRenderEffect

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_EFFECT_H
