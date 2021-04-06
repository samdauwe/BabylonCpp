#include <babylon/materials/textures/procedurals/procedural_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/materials/draw_wrapper.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/procedurals/procedural_texture_scene_component.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ProceduralTexture::ProceduralTexture(const std::string& iName, const RenderTargetTextureSize& size,
                                     const std::unordered_map<std::string, std::string>& fragment,
                                     Scene* scene, Texture* fallbackTexture, bool generateMipMaps,
                                     bool iIsCube, unsigned int textureType)
    : Texture("", scene, !generateMipMaps)
    , isEnabled{true}
    , autoClear{true}
    , onGenerated{nullptr}
    , _isCube{iIsCube}
    , refreshRate{this, &ProceduralTexture::get_refreshRate, &ProceduralTexture::set_refreshRate}
    , _fallbackTexture{nullptr}
    , _textureType{0}
    , _currentRefreshId{-1}
    , _frameId{-1}
    , _refreshRate{1}
    , _fallbackTextureUsed{false}
    , _cachedDefines{""}
    , _contentUpdateId{-1}
{
  scene = getScene() ? getScene() : EngineStore::LastCreatedScene();

  auto component = scene->_getComponent(SceneComponentConstants::NAME_PROCEDURALTEXTURE);
  if (!component) {
    component = ProceduralTextureSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  _fullEngine = scene->getEngine();

  name             = iName;
  isRenderTarget   = true;
  _size            = size;
  _textureType     = textureType;
  _generateMipMaps = generateMipMaps;
  _drawWrapper     = std::make_shared<DrawWrapper>(_fullEngine);

  setFragment(fragment);

  _fallbackTexture = fallbackTexture;

  const auto width  = std::holds_alternative<int>(size) ?
                        std::get<int>(size) :
                      std::holds_alternative<RenderTargetSize>(size) ?
                        std::get<RenderTargetSize>(size).width :
                        static_cast<int>(std::holds_alternative<float>(size));
  const auto height = std::holds_alternative<int>(size) ?
                        std::get<int>(size) :
                      std::holds_alternative<RenderTargetSize>(size) ?
                        std::get<RenderTargetSize>(size).height :
                        static_cast<int>(std::holds_alternative<float>(size));

  if (iIsCube) {
    IRenderTargetOptions options;
    options.generateMipMaps = generateMipMaps;
    options.type            = textureType;
    _texture = _fullEngine->createRenderTargetCubeTexture(ISize{width, height}, options);
    setFloat("face", 0);
  }
  else {
    IRenderTargetOptions options;
    options.generateMipMaps = generateMipMaps;
    options.type            = textureType;
    _texture                = _fullEngine->createRenderTargetTexture(width, options);
  }

  // VBO
  Float32Array vertices{1.f,  1.f,  //
                        -1.f, 1.f,  //
                        -1.f, -1.f, //
                        1.f,  -1.f};

  _vertexBuffers[VertexBuffer::PositionKind] = std::make_unique<VertexBuffer>(
    _fullEngine, vertices, VertexBuffer::PositionKind, false, false, 2);

  // Indices
  Uint32Array indices{0, 1, 2, 0, 2, 3};

  _indexBuffer = _fullEngine->createIndexBuffer(indices);
}

ProceduralTexture::ProceduralTexture(const std::string& iName, const RenderTargetTextureSize& size,
                                     const std::string& fragment, Scene* scene,
                                     Texture* fallbackTexture, bool generateMipMaps, bool iIsCube,
                                     unsigned int textureType)
    : Texture("", scene, !generateMipMaps)
    , isEnabled{true}
    , autoClear{true}
    , onGenerated{nullptr}
    , _isCube{iIsCube}
    , refreshRate{this, &ProceduralTexture::get_refreshRate, &ProceduralTexture::set_refreshRate}
    , _fallbackTexture{nullptr}
    , _textureType{0}
    , _currentRefreshId{-1}
    , _frameId{-1}
    , _refreshRate{1}
    , _fallbackTextureUsed{false}
    , _cachedDefines{""}
    , _contentUpdateId{-1}
{
  scene          = getScene() ? getScene() : EngineStore::LastCreatedScene();
  auto component = scene->_getComponent(SceneComponentConstants::NAME_PROCEDURALTEXTURE);
  if (!component) {
    component = ProceduralTextureSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  _fullEngine = scene->getEngine();

  name             = iName;
  isRenderTarget   = true;
  _size            = size;
  _textureType     = textureType;
  _generateMipMaps = generateMipMaps;
  _drawWrapper     = std::make_shared<DrawWrapper>(_fullEngine);

  setFragment(fragment);

  _fallbackTexture = fallbackTexture;

  const auto width  = std::holds_alternative<int>(size) ?
                        std::get<int>(size) :
                      std::holds_alternative<RenderTargetSize>(size) ?
                        std::get<RenderTargetSize>(size).width :
                        static_cast<int>(std::holds_alternative<float>(size));
  const auto height = std::holds_alternative<int>(size) ?
                        std::get<int>(size) :
                      std::holds_alternative<RenderTargetSize>(size) ?
                        std::get<RenderTargetSize>(size).height :
                        static_cast<int>(std::holds_alternative<float>(size));

  if (iIsCube) {
    IRenderTargetOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.generateDepthBuffer   = false;
    options.generateStencilBuffer = false;
    options.type                  = textureType;
    _texture = _fullEngine->createRenderTargetCubeTexture(ISize{width, height}, options);
    setFloat("face", 0);
  }
  else {
    IRenderTargetOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.generateDepthBuffer   = false;
    options.generateStencilBuffer = false;
    options.type                  = textureType;
    _texture                      = _fullEngine->createRenderTargetTexture(width, options);
  }

  // VBO
  Float32Array vertices{1.f,  1.f,  //
                        -1.f, 1.f,  //
                        -1.f, -1.f, //
                        1.f,  -1.f};

  _vertexBuffers[VertexBuffer::PositionKind] = std::make_unique<VertexBuffer>(
    _fullEngine, vertices, VertexBuffer::PositionKind, false, false, 2);

  _createIndexBuffer();
}

ProceduralTexture::~ProceduralTexture() = default;

void ProceduralTexture::addToScene(const ProceduralTexturePtr& newTexture)
{
  BaseTexture::addToScene(newTexture);
  getScene()->proceduralTextures.emplace_back(newTexture);
}

EffectPtr& ProceduralTexture::getEffect()
{
  return _drawWrapper->effect;
}

void ProceduralTexture::_setEffect(const EffectPtr& effect)
{
  _drawWrapper->effect = effect;
}

ArrayBufferView& ProceduralTexture::getContent()
{
  if (_contentData && _frameId == _contentUpdateId) {
    return _contentData;
  }

  if (_contentData) {
    _contentData     = readPixels(0, 0, _contentData);
    _contentUpdateId = _frameId;
  }
  else {
    _contentData     = readPixels(0, 0);
    _contentUpdateId = _frameId;
  }

  return _contentData;
}

void ProceduralTexture::_createIndexBuffer()
{
  // Indices
  Uint32Array indices{
    0, //
    1, //
    2, //
    0, //
    2, //
    3  //
  };

  _indexBuffer = _fullEngine->createIndexBuffer(indices);
}

void ProceduralTexture::_rebuild(bool /*forceFullRebuild*/)
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& vb = _vertexBuffers[VertexBuffer::PositionKind];
    if (vb) {
      vb->_rebuild();
    }
  }

  _createIndexBuffer();

  if (static_cast<unsigned>(refreshRate()) == RenderTargetTexture::REFRESHRATE_RENDER_ONCE) {
    refreshRate = RenderTargetTexture::REFRESHRATE_RENDER_ONCE;
  }
}

void ProceduralTexture::reset()
{
  if (_drawWrapper->effect == nullptr) {
    return;
  }

  _drawWrapper->effect->dispose();
}

std::string ProceduralTexture::_getDefines() const
{
  return "";
}

bool ProceduralTexture::isReady()
{
  auto engine = _fullEngine;
  std::unordered_map<std::string, std::string> shaders;

  if (nodeMaterialSource) {
    return _drawWrapper->effect->isReady();
  }

  if (_fragment.empty()) {
    return false;
  }

  if (_fallbackTextureUsed) {
    return true;
  }

  auto defines = _getDefines();
  if (_drawWrapper->effect && defines == _cachedDefines && _drawWrapper->effect->isReady()) {
    return true;
  }

  if (stl_util::contains(_fragment, "fragmentElement")) {
    shaders["vertex"]          = "procedural";
    shaders["fragmentElement"] = _fragment["fragmentElement"];
  }
  else {
    return false;
  }

  if (_cachedDefines != defines) {
    _cachedDefines = defines;

    IEffectCreationOptions options;
    options.attributes    = {VertexBuffer::PositionKind};
    options.uniformsNames = _uniforms;
    options.samplers      = _samplers;
    options.defines       = defines;
    options.onError       = [this](const Effect* /*effect*/, const std::string& /*errors*/) {
      releaseInternalTexture();

      if (_fallbackTexture) {
        _texture = _fallbackTexture->_texture;

        if (_texture) {
          _texture->incrementReferences();
        }
      }

      _fallbackTextureUsed = true;
    };

    _drawWrapper->effect = engine->createEffect(shaders, options, getScene()->getEngine());
  }

  return _drawWrapper->effect->isReady();
}

void ProceduralTexture::resetRefreshCounter()
{
  _currentRefreshId = -1;
}

void ProceduralTexture::setFragment(const std::unordered_map<std::string, std::string>& fragment)
{
  _fragment = fragment;
}

void ProceduralTexture::setFragment(const std::string& fragment)
{
  _fragment["fragmentElement"] = fragment;
}

float ProceduralTexture::get_refreshRate() const
{
  return _refreshRate;
}

void ProceduralTexture::set_refreshRate(float value)
{
  _refreshRate = value;
  resetRefreshCounter();
}

bool ProceduralTexture::_shouldRender()
{
  if (!isEnabled || !isReady() || !_texture) {
    if (_texture) {
      _texture->isReady = false;
    }
    return false;
  }

  if (_fallbackTextureUsed) {
    return false;
  }

  if (_currentRefreshId == -1) { // At least render once
    _currentRefreshId = 1;
    _frameId++;
    return true;
  }

  if (static_cast<int>(refreshRate()) == _currentRefreshId) {
    _currentRefreshId = 1;
    _frameId++;
    return true;
  }

  ++_currentRefreshId;
  return false;
}

ProceduralTexture::RenderTargetTextureSize& ProceduralTexture::getRenderSize()
{
  return _size;
}

const ProceduralTexture::RenderTargetTextureSize& ProceduralTexture::getRenderSize() const
{
  return _size;
}

void ProceduralTexture::resize(const Size& size, bool generateMipMaps)
{
  if (_fallbackTextureUsed) {
    return;
  }

  releaseInternalTexture();
  IRenderTargetOptions options;
  options.generateMipMaps       = generateMipMaps;
  options.generateDepthBuffer   = false;
  options.generateStencilBuffer = false;
  options.type                  = _textureType;
  _texture
    = _fullEngine->createRenderTargetTexture(RenderTargetSize{size.width, size.height}, options);

  // Update properties
  _size            = RenderTargetSize{size.width, size.height};
  _generateMipMaps = generateMipMaps;
}

void ProceduralTexture::_checkUniform(const std::string& uniformName)
{
  auto it = std::find(_uniforms.begin(), _uniforms.end(), uniformName);
  if (it == _uniforms.end()) {
    _uniforms.emplace_back(uniformName);
  }
}

ProceduralTexture& ProceduralTexture::setTexture(const std::string& iName,
                                                 const TexturePtr& texture)
{
  auto it = std::find(_samplers.begin(), _samplers.end(), iName);
  if (it == _samplers.end()) {
    _samplers.emplace_back(iName);
  }
  _textures[iName] = texture;

  return *this;
}

ProceduralTexture& ProceduralTexture::setFloat(const std::string& _name, float value)
{
  _checkUniform(_name);
  _floats[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setInt(const std::string& _name, int value)
{
  _checkUniform(_name);
  _ints[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setFloats(const std::string& _name, Float32Array value)
{
  _checkUniform(_name);
  _floatsArrays[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setColor3(const std::string& _name, const Color3& value)
{
  _checkUniform(_name);
  _colors3[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setColor4(const std::string& _name, const Color4& value)
{
  _checkUniform(_name);
  _colors4[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setVector2(const std::string& _name, const Vector2& value)
{
  _checkUniform(_name);
  _vectors2[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setVector3(const std::string& _name, const Vector3& value)
{
  _checkUniform(_name);
  _vectors3[_name] = value;

  return *this;
}

ProceduralTexture& ProceduralTexture::setMatrix(const std::string& _name, const Matrix& value)
{
  _checkUniform(_name);
  _matrices[_name] = value;

  return *this;
}

void ProceduralTexture::render(bool /*useCameraPostProcess*/)
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  auto engine = _fullEngine;

  // Render
  engine->enableEffect(_drawWrapper);
  onBeforeGenerationObservable.notifyObservers(this);
  engine->setState(false);

  if (!nodeMaterialSource) {
    // Texture
    for (const auto& item : _textures) {
      _drawWrapper->effect->setTexture(item.first, item.second);
    }

    // Int
    for (const auto& item : _ints) {
      _drawWrapper->effect->setInt(item.first, item.second);
    }

    // Float
    for (const auto& item : _floats) {
      _drawWrapper->effect->setFloat(item.first, item.second);
    }

    // Floats
    for (const auto& item : _floatsArrays) {
      _drawWrapper->effect->setArray(item.first, item.second);
    }

    // Color3
    for (const auto& item : _colors3) {
      _drawWrapper->effect->setColor3(item.first, item.second);
    }

    // Color4
    for (const auto& item : _colors4) {
      const auto& color = item.second;
      _drawWrapper->effect->setFloat4(item.first, color.r, color.g, color.b, color.a);
    }

    // Vector2
    for (const auto& item : _vectors2) {
      _drawWrapper->effect->setVector2(item.first, item.second);
    }

    // Vector3
    for (const auto& item : _vectors3) {
      _drawWrapper->effect->setVector3(item.first, item.second);
    }

    // Matrix
    for (const auto& item : _matrices) {
      _drawWrapper->effect->setMatrix(item.first, item.second);
    }
  }

  if (!_texture) {
    return;
  }

  engine->_debugPushGroup(StringTools::printf("procedural texture generation for %s", name.c_str()),
                          1);

  if (isCube) {
    for (unsigned int face = 0; face < 6; ++face) {
      engine->bindFramebuffer(_texture, face, std::nullopt, std::nullopt, true);

      // VBOs
      engine->bindBuffers(_vertexBuffers, _indexBuffer, _drawWrapper->effect);

      _drawWrapper->effect->setFloat("face", static_cast<float>(face));

      // Clear
      if (autoClear) {
        engine->clear(scene->clearColor, true, false, false);
      }

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode, 0, 6);
    }
  }
  else {
    engine->bindFramebuffer(_texture, 0u, std::nullopt, std::nullopt, true);

    // VBOs
    engine->bindBuffers(_vertexBuffers, _indexBuffer, _drawWrapper->effect);

    // Clear
    if (autoClear) {
      engine->clear(scene->clearColor, true, false, false);
    }

    // Draw order
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
  }

  // Unbind
  engine->unBindFramebuffer(_texture, isCube);

  // Mipmaps
  if (isCube) {
    engine->generateMipMapsForCubemap(_texture);
  }

  engine->_debugPopGroup(1);

  if (onGenerated) {
    onGenerated();
  }

  onGeneratedObservable.notifyObservers(this);
}

ProceduralTexturePtr ProceduralTexture::clone()
{
  auto textureSize = getSize();
  auto newTexture  = ProceduralTexture::New(name, textureSize.width, _fragment, getScene(),
                                           _fallbackTexture, _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // RenderTarget Texture
  newTexture->coordinatesMode = coordinatesMode();

  return newTexture;
}

void ProceduralTexture::dispose()
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  stl_util::remove_vector_elements_equal_sharedptr(scene->proceduralTextures, this);

  auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKind];
  if (vertexBuffer) {
    vertexBuffer->dispose();
    vertexBuffer = nullptr;
    _vertexBuffers.erase(VertexBuffer::PositionKind);
  }

  if (_indexBuffer && _fullEngine->_releaseBuffer(_indexBuffer)) {
    _indexBuffer = nullptr;
  }

  onGeneratedObservable.clear();
  onBeforeGenerationObservable.clear();

  Texture::dispose();
}

} // namespace BABYLON
