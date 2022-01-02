#include <babylon/materials/effect_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_wrapper.h>
#include <babylon/materials/ieffect_renderer_options.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

IEffectRendererOptions EffectRenderer::_DefaultOptions = IEffectRendererOptions{
  {1.f, 1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f}, // positions
  {0, 1, 2, 0, 2, 3},                           // indices
};

EffectRenderer::EffectRenderer(ThinEngine* iEngine, const IEffectRendererOptions& options)
    : engine{iEngine}, _fullscreenViewport{std::make_unique<Viewport>(0.f, 0.f, 1.f, 1.f)}
{
  _vertexBuffers = {
    {VertexBuffer::PositionKind,
     std::make_unique<VertexBuffer>(static_cast<Engine*>(engine), options.positions,
                                    VertexBuffer::PositionKind, false, false, 2)},
  };
  _indexBuffer = engine->createIndexBuffer(options.indices);
}

EffectRenderer::~EffectRenderer() = default;

void EffectRenderer::setViewport()
{
  engine->setViewport(*_fullscreenViewport);
}

void EffectRenderer::setViewport(Viewport& viewport)
{
  engine->setViewport(viewport);
}

void EffectRenderer::bindBuffers(const EffectPtr& effect)
{
  engine->bindBuffers(_vertexBuffers, _indexBuffer, effect);
}

void EffectRenderer::applyEffectWrapper(const EffectWrapperPtr& effectWrapper)
{
  engine->depthCullingState()->depthTest = false;
  engine->stencilState()->stencilTest    = false;
  engine->enableEffect(effectWrapper->effect);
  bindBuffers(effectWrapper->effect);
  effectWrapper->onApplyObservable.notifyObservers(nullptr);
}

void EffectRenderer::restoreStates()
{
  engine->depthCullingState()->depthTest = true;
  engine->stencilState()->stencilTest    = true;
}

void EffectRenderer::draw()
{
  engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, 6);
}

bool EffectRenderer::isRenderTargetTexture(
  const std::variant<InternalTexturePtr, RenderTargetTexturePtr>& texture)
{
  return std::holds_alternative<RenderTargetTexturePtr>(texture);
}

void EffectRenderer::render(
  const EffectWrapperPtr& effectWrapper,
  const std::optional<std::variant<InternalTexturePtr, RenderTargetTexturePtr>>& outputTexture)
{
  // Ensure effect is ready
  if (!effectWrapper->effect->isReady()) {
    return;
  }

  // Reset state
  setViewport();

  const auto out = !outputTexture.has_value() ?
                     nullptr :
                   isRenderTargetTexture(*outputTexture) ?
                     std::get<RenderTargetTexturePtr>(*outputTexture)->getInternalTexture() :
                     std::get<InternalTexturePtr>(*outputTexture);

  if (out) {
    engine->bindFramebuffer(out);
  }

  applyEffectWrapper(effectWrapper);

  draw();

  if (out) {
    engine->unBindFramebuffer(out);
  }

  restoreStates();
}

void EffectRenderer::dispose()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto vertexBuffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (vertexBuffer) {
      vertexBuffer->dispose();
    }
    _vertexBuffers.erase(VertexBuffer::PositionKind);
  }

  if (_indexBuffer) {
    engine->_releaseBuffer(_indexBuffer);
  }
}

} // end of namespace BABYLON
