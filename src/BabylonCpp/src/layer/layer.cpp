#include <babylon/layer/layer.h>

#include <babylon/engine/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

Layer::Layer(const std::string& name, const std::string& imgUrl, Scene* scene,
             bool _isBackground, const Color4& _color)
    : isBackground{_isBackground}
    , color{_color}
    , scale{Vector2(1.f, 1.f)}
    , offset{Vector2(0.f, 0.f)}
    , alphaBlendingMode{Engine::ALPHA_COMBINE}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _name{name}
    , _scene{scene}
{
  texture = !imgUrl.empty() ? Texture::New(imgUrl, scene, true) : nullptr;

  _scene->layers.emplace_back(this);

  Engine* engine = scene->getEngine();

  // VBO
  Float32Array vertices{1.f, 1.f, -1.f, 1.f, -1.f, -1.f, 1.f, -1.f};

  VertexBuffer* vertexBuffer = new VertexBuffer(
    engine, vertices, VertexBuffer::PositionKind, false, false, 2);
  _vertexBuffers[VertexBuffer::PositionKindChars] = vertexBuffer;

  // Indices
  Uint32Array indices{0, 1, 2, 0, 2, 3};
  _indexBuffer = engine->createIndexBuffer(indices);

  // Effects
  _effect
    = engine->createEffect("layer",                                       //
                           {VertexBuffer::PositionKindChars},             //
                           {"textureMatrix", "color", "scale", "offset"}, //
                           {"textureSampler"}, "");

  _alphaTestEffect
    = engine->createEffect("layer",                                       //
                           {VertexBuffer::PositionKindChars},             //
                           {"textureMatrix", "color", "scale", "offset"}, //
                           {"textureSampler"}, "#define ALPHATEST");
}

Layer::~Layer()
{
}

// Events
void Layer::setOnDispose(const std::function<void()>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Layer::setOnBeforeRender(const std::function<void()>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void Layer::setOnAfterRender(const std::function<void()>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

void Layer::render()
{
  Effect* currentEffect = alphaTest ? _alphaTestEffect : _effect;

  // Check
  if (!currentEffect->isReady() || !texture || !texture->isReady()) {
    return;
  }

  Engine* engine = _scene->getEngine();

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
  engine->bindBuffers(_vertexBuffers, _indexBuffer.get(), currentEffect);

  // Draw order
  if (!_alphaTestEffect) {
    engine->setAlphaMode(alphaBlendingMode);
    engine->draw(true, 0, 6);
    engine->setAlphaMode(Engine::ALPHA_DISABLE);
  }
  else {
    engine->draw(true, 0, 6);
  }

  onAfterRenderObservable.notifyObservers(this);
}

void Layer::dispose(bool /*doNotRecurse*/)
{
  VertexBuffer* vertexBuffer = _vertexBuffers[VertexBuffer::PositionKindChars];
  if (vertexBuffer) {
    vertexBuffer->dispose();
    _vertexBuffers.erase(VertexBuffer::PositionKindChars);
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }

  if (texture) {
    texture->dispose();
    texture = nullptr;
  }

  // Remove from scene
  auto& layers = _scene->layers;
  auto it      = std::remove(layers.begin(), layers.end(), this);
  if (it != layers.end()) {
    layers.erase(it);
  }

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onAfterRenderObservable.clear();
  onBeforeRenderObservable.clear();
}

} // end of namespace BABYLON
