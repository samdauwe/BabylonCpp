#include <babylon/layers/layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/layers/layer_scene_component.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/matrix.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

Layer::Layer(const std::string& name, const std::string& imgUrl, Scene* scene, bool iIsBackground,
             const Color4& iColor)
    : isBackground{iIsBackground}
    , color{iColor}
    , scale{Vector2(1.f, 1.f)}
    , offset{Vector2(0.f, 0.f)}
    , alphaBlendingMode{Constants::ALPHA_COMBINE}
    , layerMask{0x0FFFFFFF}
    , renderOnlyInRenderTargetTextures{false}
    , onDispose{this, &Layer::set_onDispose}
    , onBeforeRender{this, &Layer::set_onBeforeRender}
    , onAfterRender{this, &Layer::set_onAfterRender}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _name{name}
{
  texture = !imgUrl.empty() ? Texture::New(imgUrl, scene, true) : nullptr;

  _scene              = scene ? scene : Engine::LastCreatedScene();
  auto layerComponent = std::static_pointer_cast<LayerSceneComponent>(
    _scene->_getComponent(SceneComponentConstants::NAME_LAYER));
  if (!layerComponent) {
    layerComponent = LayerSceneComponent::New(_scene);
    _scene->_addComponent(layerComponent);
  }
  _scene->layers.emplace_back(shared_from_this());

  auto engine = _scene->getEngine();

  // VBO
  const Float32Array vertices{
    1.f,  1.f,  //
    -1.f, 1.f,  //
    -1.f, -1.f, //
    1.f,  -1.f  //
  };

  _vertexBuffers[VertexBuffer::PositionKind]
    = std::make_shared<VertexBuffer>(engine, vertices, VertexBuffer::PositionKind, false, false, 2);

  _createIndexBuffer();
}

Layer::~Layer() = default;

void Layer::set_onDispose(const LayerCallbackType& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Layer::set_onBeforeRender(const LayerCallbackType& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void Layer::set_onAfterRender(const LayerCallbackType& callback)
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
  auto& vb = _vertexBuffers[VertexBuffer::PositionKind];

  if (vb) {
    vb->_rebuild();
  }

  _createIndexBuffer();
}

void Layer::render()
{
  auto engine = _scene->getEngine();

  std::string defines = "";

  if (alphaTest) {
    defines = "#define ALPHATEST";
  }

  if (texture && !texture->gammaSpace) {
    defines += "\r\n#define LINEAR";
  }

  if (_previousDefines != defines) {
    _previousDefines = defines;

    IEffectCreationOptions options;
    options.attributes    = {VertexBuffer::PositionKind};
    options.uniformsNames = {"textureMatrix", "color", "scale", "offset"};
    options.samplers      = {"textureSampler"};
    options.defines       = defines;

    _effect = engine->createEffect("layer", options, engine);
  }
  auto currentEffect = _effect;

  // Check
  if (!currentEffect || !currentEffect->isReady() || !texture || !texture->isReady()) {
    return;
  }

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
  engine->bindBuffers(_vertexBuffers, _indexBuffer, currentEffect);

  // Draw order
  if (!alphaTest) {
    engine->setAlphaMode(alphaBlendingMode);
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
    engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }
  else {
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
  }

  onAfterRenderObservable.notifyObservers(this);
}

void Layer::dispose()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (vertexBuffer) {
      vertexBuffer->dispose();
      _vertexBuffers.erase(VertexBuffer::PositionKind);
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer);
    _indexBuffer = nullptr;
  }

  if (texture) {
    texture->dispose();
    texture = nullptr;
  }

  // Clean RTT list
  renderTargetTextures.clear();

  // Remove from scene
  auto& layers = _scene->layers;
  stl_util::remove_vector_elements_equal_sharedptr(layers, this);

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onAfterRenderObservable.clear();
  onBeforeRenderObservable.clear();
}

} // end of namespace BABYLON
