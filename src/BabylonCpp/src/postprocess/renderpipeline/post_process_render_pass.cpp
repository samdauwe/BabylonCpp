#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

PostProcessRenderPass::PostProcessRenderPass(
  Scene* scene, const std::string& name, ISize size,
  const std::vector<Mesh*>& renderList,
  const std::function<void(int faceIndex)>& beforeRender,
  const std::function<void(int faceIndex)>& afterRender)
    : _name{name}
    , _enabled{true}
    , _renderList{renderList}
    , _scene{scene}
    , _refCount{0}
{
  _renderTexture = new RenderTargetTexture(name, size, scene);
  setRenderList(renderList);

  _renderTexture->onBeforeRenderObservable.add(beforeRender);
  _renderTexture->onAfterRenderObservable.add(afterRender);
}

PostProcessRenderPass::~PostProcessRenderPass()
{
}

int PostProcessRenderPass::_incRefCount()
{
  if (_refCount == 0) {
    _scene->customRenderTargets.emplace_back(_renderTexture);
  }

  return ++_refCount;
}

int PostProcessRenderPass::_decRefCount()
{
  --_refCount;

  if (_refCount <= 0) {
    auto it = std::find(_scene->customRenderTargets.begin(),
                        _scene->customRenderTargets.end(), _renderTexture);
    if (it != _scene->customRenderTargets.end()) {
      _scene->customRenderTargets.erase(it);
    }
  }

  return _refCount;
}

void PostProcessRenderPass::_update()
{
  setRenderList(_renderList);
}

void PostProcessRenderPass::setRenderList(const std::vector<Mesh*>& renderList)
{
  std::vector<AbstractMesh*> tmp;
  for (auto& mesh : renderList) {
    tmp.emplace_back(mesh);
  }
  _renderTexture->renderList = tmp;
}

RenderTargetTexture* PostProcessRenderPass::getRenderTexture()
{
  return _renderTexture;
}

} // end of namespace BABYLON
