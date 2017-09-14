#ifndef BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PASS_H
#define BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PASS_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessRenderPass {

public:
  PostProcessRenderPass(
    Scene* scene, const string_t& name, ISize size,
    const vector_t<Mesh*>& renderList,
    const ::std::function<void(int faceIndex)>& beforeRender,
    const ::std::function<void(int faceIndex)>& afterRender);
  ~PostProcessRenderPass();

  int _incRefCount();
  int _decRefCount();
  void _update();
  void setRenderList(const vector_t<Mesh*>& renderList);
  RenderTargetTexture* getRenderTexture();

public:
  string_t _name;
  bool _enabled;

private:
  vector_t<Mesh*> _renderList;
  std::unique_ptr<RenderTargetTexture> _renderTexture;
  Scene* _scene;
  int _refCount;

}; // end of class PostProcessRenderPass

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_RENDERPIPELINE_POST_PROCESS_RENDER_PASS_H
