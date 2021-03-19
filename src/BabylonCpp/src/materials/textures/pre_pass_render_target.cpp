#include <babylon/materials/textures/pre_pass_render_target.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/size.h>
#include <babylon/postprocesses/image_processing_post_process.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

PrePassRenderTarget::PrePassRenderTarget(const std::string& iName,
                                         const RenderTargetTexturePtr& iRenderTargetTexture,
                                         const std::variant<int, RenderTargetSize, float>& size,
                                         size_t count, Scene* scene,
                                         const std::optional<IMultiRenderTargetOptions>& options)
    : MultiRenderTarget{iName, size, count, scene, options}
    , imageProcessingPostProcess{nullptr}
    , _engine{nullptr}
    , _scene{nullptr}
    , _outputPostProcess{nullptr}
    , _internalTextureDirty{false}
    , enabled{false}
    , renderTargetTexture{nullptr}
{
  renderTargetTexture = iRenderTargetTexture;
}

PrePassRenderTarget::~PrePassRenderTarget() = default;

void PrePassRenderTarget::_createCompositionEffect()
{
  imageProcessingPostProcess
    = ImageProcessingPostProcess::New("prePassComposition", 1.f, nullptr, std::nullopt, _engine);
  imageProcessingPostProcess->_updateParameters();
}

void PrePassRenderTarget::_checkSize()
{
  const auto requiredWidth  = _engine->getRenderWidth(true);
  const auto requiredHeight = _engine->getRenderHeight(true);

  const auto width  = getRenderWidth();
  const auto height = getRenderHeight();

  if (width != requiredWidth || height != requiredHeight) {
    resize(Size{
      requiredWidth, // width,
      requiredHeight // height
    });

    _internalTextureDirty = true;
  }
}

void PrePassRenderTarget::updateCount(size_t iCount,
                                      const std::optional<IMultiRenderTargetOptions>& options)
{
  MultiRenderTarget::updateCount(iCount, options);
  _internalTextureDirty = true;
}

void PrePassRenderTarget::_resetPostProcessChain()
{
  _beforeCompositionPostProcesses = {};
}

void PrePassRenderTarget::dispose()
{
  const auto& scene = _scene;

  MultiRenderTarget::dispose();

  if (scene && scene->prePassRenderer()) {
    auto it = std::find_if(scene->prePassRenderer()->renderTargets.begin(),
                           scene->prePassRenderer()->renderTargets.end(),
                           [this](const PrePassRenderTargetPtr& rt) { return rt.get() == this; });

    if (it != scene->prePassRenderer()->renderTargets.end()) {
      scene->prePassRenderer()->renderTargets.erase(it);
    }
  }

  if (imageProcessingPostProcess) {
    imageProcessingPostProcess->dispose();
  }
}

} // end of namespace BABYLON
