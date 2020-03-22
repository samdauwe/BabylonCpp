#include <babylon/materials/effect_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_wrapper.h>
#include <babylon/materials/ieffect_renderer_options.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

IEffectRendererOptions EffectRenderer::_DefaultOptions = IEffectRendererOptions{
  {1.f, 1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f}, // positions
  {0, 1, 2, 0, 2, 3}                            // indices
};

TexturePtr EffectRenderer::_getNextFrameBuffer(bool incrementIndex)
{
  if (_ringScreenBuffer.empty()) {
    _ringScreenBuffer = {};
    for (auto i = 0; i < 2; ++i) {
      auto internalTexture = engine->createRenderTargetTexture(
        RenderTargetSize{
          engine->getRenderWidth(true),  // width
          engine->getRenderHeight(true), // height
        },
        IRenderTargetOptions{
          false,                              // generateMipMaps
          false,                              // generateDepthBuffer
          false,                              // generateStencilBuffer
          std::nullopt,                       // type
          std::nullopt,                       // format
          Constants::TEXTURE_NEAREST_NEAREST, // samplingMode
        });
      auto texture      = Texture::New("", nullptr);
      texture->_texture = internalTexture;
      _ringScreenBuffer.emplace_back(texture);
    }
  }
  auto ret = _ringScreenBuffer[_ringBufferIndex];
  if (incrementIndex) {
    _ringBufferIndex = (_ringBufferIndex + 1) % 2;
  }
  return ret;
}

EffectRenderer::EffectRenderer(ThinEngine* iEngine, const IEffectRendererOptions& options)
    : engine{iEngine}, _fullscreenViewport{std::make_unique<Viewport>(0.f, 0.f, 1.f, 1.f)}
{
  _vertexBuffers = {
    {VertexBuffer::PositionKind,
     std::make_unique<VertexBuffer>(static_cast<Engine*>(engine), options.positions,
                                    VertexBuffer::PositionKind, false, false, 2)},
  };
  _indexBuffer = engine->createIndexBuffer(options.indices);

  // No need here for full screen render.
  iEngine->depthCullingState()->depthTest = false;
  iEngine->stencilState()->stencilTest    = false;
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
  engine->enableEffect(effectWrapper->effect);
  bindBuffers(effectWrapper->effect);
  effectWrapper->onApplyObservable.notifyObservers(nullptr);
}

void EffectRenderer::draw()
{
  engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, 6);
}

void EffectRenderer::render(const std::vector<EffectWrapperPtr>& effectWrappers,
                            const TexturePtr& outputTexture)
{
  // Ensure all effects are ready
  for (const auto& wrapper : effectWrappers) {
    if (!wrapper->effect->isReady()) {
      return;
    }
  }

  for (auto i = 0ull; i < effectWrappers.size(); ++i) {
    const auto& effectWrapper = effectWrappers[i];
    auto renderTo             = outputTexture;

    // for any next effect make it's input the output of the previous effect
    if (i != 0) {
      effectWrapper->effect->onBindObservable().addOnce(
        [this, effectWrapper](Effect*, EventState & /*es*/) -> void {
          effectWrapper->effect->setTexture("textureSampler", _getNextFrameBuffer(false));
        });
    }

    // Set the output to the next screenbuffer
    if (effectWrappers.size() > 1 && i != effectWrappers.size() - 1) {
      renderTo = _getNextFrameBuffer();
    }
    else {
      renderTo = outputTexture;
    }

    // Reset state
    setViewport();
    applyEffectWrapper(effectWrapper);

    if (renderTo) {
      engine->bindFramebuffer(renderTo->getInternalTexture());
    }

    draw();

    if (renderTo) {
      engine->unBindFramebuffer(renderTo->getInternalTexture());
    }
  }
}

void EffectRenderer::dispose()
{
  if (!_ringScreenBuffer.empty()) {
    for (const auto& b : _ringScreenBuffer) {
      b->dispose();
    }
    _ringScreenBuffer.clear();
  }

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
