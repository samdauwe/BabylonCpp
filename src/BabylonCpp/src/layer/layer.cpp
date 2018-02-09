#include <babylon/layer/layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

Layer::Layer(const string_t& name, const string_t& imgUrl, Scene* scene,
             bool iIsBackground, const Color4& iColor)
    : isBackground{iIsBackground}
    , color{iColor}
    , scale{Vector2(1.f, 1.f)}
    , offset{Vector2(0.f, 0.f)}
    , alphaBlendingMode{EngineConstants::ALPHA_COMBINE}
    , layerMask{0x0FFFFFFF}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _name{name}
{
  texture = !imgUrl.empty() ? Texture::New(imgUrl, scene, true) : nullptr;

  _scene = scene ? scene : Engine::LastCreatedScene();
  _scene->layers.emplace_back(this);

  auto engine = scene->getEngine();

  // VBO
  Float32Array vertices{
    1.f,  1.f,  //
    -1.f, 1.f,  //
    -1.f, -1.f, //
    1.f,  -1.f  //
  };

  _vertexBuffers[VertexBuffer::PositionKindChars]
    = ::std::make_unique<VertexBuffer>(
      engine, vertices, VertexBuffer::PositionKind, false, false, 2);

  _createIndexBuffer();

  // Effects
  {
    EffectCreationOptions options;
    options.attributes    = {VertexBuffer::PositionKindChars};
    options.uniformsNames = {"textureMatrix", "color", "scale", "offset"};
    options.samplers      = {"textureSampler"};
    options.defines       = "";

    _effect = engine->createEffect("layer", options, _scene->getEngine());
  }

  {
    EffectCreationOptions options;
    options.attributes    = {VertexBuffer::PositionKindChars};
    options.uniformsNames = {"textureMatrix", "color", "scale", "offset"};
    options.samplers      = {"textureSampler"};
    options.defines       = "#define ALPHATEST";

    _alphaTestEffect
      = engine->createEffect("layer", options, _scene->getEngine());
  }
}

Layer::~Layer()
{
}

// Events
void Layer::setOnDispose(
  const ::std::function<void(Layer*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Layer::setOnBeforeRender(
  const ::std::function<void(Layer*, EventState&)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void Layer::setOnAfterRender(
  const ::std::function<void(Layer*, EventState&)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

void Layer::_createIndexBuffer()
{
  auto engine = _scene->getEngine();

  // Indices
  Uint32Array indices{
    0, //
    1, //
    2, //
    0, //
    2, //
    3  //
  };
  _indexBuffer = engine->createIndexBuffer(indices);
}

void Layer::_rebuild()
{
  auto& vb = _vertexBuffers[VertexBuffer::PositionKindChars];

  if (vb) {
    vb->_rebuild();
  }

  _createIndexBuffer();
}

void Layer::render()
{
  auto currentEffect = alphaTest ? _alphaTestEffect : _effect;

  // Check
  if (!currentEffect->isReady() || !texture || !texture->isReady()) {
    return;
  }

  auto engine = _scene->getEngine();

  onBeforeRenderObservable.notifyObservers(this);

  // Render
  engine->enableEffect(currentEffect);
  engine->setState(false);

  // Texture
  currentEffect->setTexture("textureSampler", texture);
  currentEffect->setMatrix("textureMatrix", *texture->getTextureMatrix());

  // Color
  currentEffect->setFloat4("color", color.r, color.g, color.b, color.a);

  // Scale / offset
  currentEffect->setVector2("offset", offset);
  currentEffect->setVector2("scale", scale);

  // VBOs
  engine->bindBuffers(stl_util::to_raw_ptr_map(_vertexBuffers),
                      _indexBuffer.get(), currentEffect);

  // Draw order
  if (!alphaTest) {
    engine->setAlphaMode(alphaBlendingMode);
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
    engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
  }
  else {
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
  }

  onAfterRenderObservable.notifyObservers(this);
}

void Layer::dispose(bool /*doNotRecurse*/)
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)) {
    auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKindChars];
    if (vertexBuffer) {
      vertexBuffer->dispose();
      _vertexBuffers.erase(VertexBuffer::PositionKindChars);
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer = nullptr;
  }

  if (texture) {
    texture->dispose();
    texture = nullptr;
  }

  // Remove from scene
  auto& layers = _scene->layers;
  layers.erase(std::remove(layers.begin(), layers.end(), this), layers.end());

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onAfterRenderObservable.clear();
  onBeforeRenderObservable.clear();
}

} // end of namespace BABYLON
