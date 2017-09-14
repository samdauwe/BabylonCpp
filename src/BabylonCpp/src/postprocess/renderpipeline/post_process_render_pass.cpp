#include <babylon/postprocess/renderpipeline/post_process_render_pass.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

PostProcessRenderPass::PostProcessRenderPass(
  Scene* scene, const string_t& name, ISize size,
  const vector_t<Mesh*>& renderList,
  const ::std::function<void(int faceIndex)>& beforeRender,
  const ::std::function<void(int faceIndex)>& afterRender)
    : _name{name}
    , _enabled{true}
    , _renderList{renderList}
    , _scene{scene}
    , _refCount{0}
{
  _renderTexture = ::std::make_unique<RenderTargetTexture>(name, size, scene);
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
    _scene->customRenderTargets.emplace_back(_renderTexture.get());
  }

  return ++_refCount;
}

int PostProcessRenderPass::_decRefCount()
{
  --_refCount;

  if (_refCount <= 0) {
    _scene->customRenderTargets.erase(::std::remove_if(
      _scene->customRenderTargets.begin(), _scene->customRenderTargets.end(),
      [this](const RenderTargetTexture* renderTargetTexture) {
        return _renderTexture.get() == renderTargetTexture;
      }));
  }

  return _refCount;
}

void PostProcessRenderPass::_update()
{
  setRenderList(_renderList);
}

void PostProcessRenderPass::setRenderList(const vector_t<Mesh*>& renderList)
{
  _renderTexture->renderList.clear();
  _renderTexture->renderList.reserve(renderList.size());
  ::std::copy(renderList.begin(), renderList.end(),
              ::std::back_inserter(_renderTexture->renderList));
}

RenderTargetTexture* PostProcessRenderPass::getRenderTexture()
{
  return _renderTexture.get();
}

} // end of namespace BABYLON
