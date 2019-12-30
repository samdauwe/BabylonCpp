#include <babylon/materials/textures/procedurals/procedural_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/procedurals/procedural_texture_scene_component.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

ProceduralTexture::ProceduralTexture(const std::string& iName, const Size& size,
                                     const std::unordered_map<std::string, std::string>& fragment,
                                     Scene* scene, Texture* fallbackTexture, bool generateMipMaps,
                                     bool iIsCube)
    : Texture("", scene, !generateMipMaps)
    , isEnabled{true}
    , autoClear{true}
    , _generateMipMaps{generateMipMaps}
    , _isCube{iIsCube}
    , _effect{nullptr}
    , refreshRate{this, &ProceduralTexture::get_refreshRate, &ProceduralTexture::set_refreshRate}
    , _currentRefreshId{-1}
    , _frameId{-1}
    , _refreshRate{1}
    , _fallbackTextureUsed{false}
    , _cachedDefines{""}
    , _contentUpdateId{-1}
{
  scene = getScene();

  auto component = scene->_getComponent(SceneComponentConstants::NAME_PROCEDURALTEXTURE);
  if (!component) {
    component = ProceduralTextureSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  _engine = scene->getEngine();

  name           = iName;
  isRenderTarget = true;
  _size          = size;
  setFragment(fragment);

  _fallbackTexture = fallbackTexture;

  if (iIsCube) {
    IRenderTargetOptions options;
    options.generateMipMaps = generateMipMaps;
    _texture                = _engine->createRenderTargetCubeTexture(size, options);
    setFloat("face", 0);
  }
  else {
    IRenderTargetOptions options;
    options.generateMipMaps = generateMipMaps;
    _texture                = _engine->createRenderTargetTexture(size, options);
  }

  // VBO
  Float32Array vertices{1.f,  1.f,  //
                        -1.f, 1.f,  //
                        -1.f, -1.f, //
                        1.f,  -1.f};

  _vertexBuffers[VertexBuffer::PositionKind] = std::make_unique<VertexBuffer>(
    _engine, vertices, VertexBuffer::PositionKind, false, false, 2);

  // Indices
  Uint32Array indices{0, 1, 2, 0, 2, 3};

  _indexBuffer = _engine->createIndexBuffer(indices);
}

ProceduralTexture::ProceduralTexture(const std::string& iName, const Size& size,
                                     const std::string& fragment, Scene* scene,
                                     Texture* fallbackTexture, bool generateMipMaps, bool iIsCube)
    : Texture("", scene, !generateMipMaps)
    , isEnabled{true}
    , autoClear{true}
    , _generateMipMaps{generateMipMaps}
    , _isCube{iIsCube}
    , refreshRate{this, &ProceduralTexture::get_refreshRate, &ProceduralTexture::set_refreshRate}
    , _currentRefreshId{-1}
    , _refreshRate{1}
    , _fallbackTextureUsed{false}
    , _cachedDefines{""}
    , _contentUpdateId{-1}
{
  scene          = getScene();
  auto component = scene->_getComponent(SceneComponentConstants::NAME_PROCEDURALTEXTURE);
  if (!component) {
    component = ProceduralTextureSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  _engine = scene->getEngine();

  name           = iName;
  isRenderTarget = true;
  _size          = size;

  setFragment(fragment);

  _fallbackTexture = fallbackTexture;

  if (iIsCube) {
    IRenderTargetOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.generateDepthBuffer   = false;
    options.generateStencilBuffer = false;
    _texture                      = _engine->createRenderTargetCubeTexture(size, options);
    setFloat("face", 0);
  }
  else {
    IRenderTargetOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.generateDepthBuffer   = false;
    options.generateStencilBuffer = false;
    _texture                      = _engine->createRenderTargetTexture(size, options);
  }

  // VBO
  Float32Array vertices{1.f,  1.f,  //
                        -1.f, 1.f,  //
                        -1.f, -1.f, //
                        1.f,  -1.f};

  _vertexBuffers[VertexBuffer::PositionKind] = std::make_unique<VertexBuffer>(
    _engine, vertices, VertexBuffer::PositionKind, false, false, 2);

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
  return _effect;
}

ArrayBufferView& ProceduralTexture::getContent()
{
  if (_contentData && _frameId == _contentUpdateId) {
    return _contentData;
  }

  _contentData     = readPixels(0, 0, _contentData);
  _contentUpdateId = _frameId;

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

  _indexBuffer = _engine->createIndexBuffer(indices);
}

void ProceduralTexture::_rebuild()
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
  if (_effect == nullptr) {
    return;
  }

  _effect->dispose();
}

std::string ProceduralTexture::_getDefines() const
{
  return "";
}

bool ProceduralTexture::isReady()
{
  auto engine = _engine;
  std::unordered_map<std::string, std::string> shaders;

  if (_fragment.empty()) {
    return false;
  }

  if (_fallbackTextureUsed) {
    return true;
  }

  auto defines = _getDefines();
  if (_effect && defines == _cachedDefines && _effect->isReady()) {
    return true;
  }

  if (stl_util::contains(_fragment, "fragmentElement")) {
    shaders["vertex"]          = "procedural";
    shaders["fragmentElement"] = _fragment["fragmentElement"];
  }
  else {
    return false;
  }

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

  _effect = engine->createEffect(shaders, options, getScene()->getEngine());

  return _effect->isReady();
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

Size& ProceduralTexture::getRenderSize()
{
  return _size;
}

const Size& ProceduralTexture::getRenderSize() const
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
  options.generateMipMaps = generateMipMaps;
  _texture                = _engine->createRenderTargetTexture(size, options);

  // Update properties
  _size            = size;
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

  auto engine = _engine;

  // Render
  engine->enableEffect(_effect);
  engine->setState(false);

  // Texture
  for (auto& item : _textures) {
    _effect->setTexture(item.first, item.second);
  }

  // Int
  for (auto& item : _ints) {
    _effect->setInt(item.first, item.second);
  }

  // Float
  for (auto& item : _floats) {
    _effect->setFloat(item.first, item.second);
  }

  // Floats
  for (auto& item : _floatsArrays) {
    _effect->setArray(item.first, item.second);
  }

  // Color3
  for (auto& item : _colors3) {
    _effect->setColor3(item.first, item.second);
  }

  // Color4
  for (auto& item : _colors4) {
    const auto& color = item.second;
    _effect->setFloat4(item.first, color.r, color.g, color.b, color.a);
  }

  // Vector2
  for (auto& item : _vectors2) {
    _effect->setVector2(item.first, item.second);
  }

  // Vector3
  for (auto& item : _vectors3) {
    _effect->setVector3(item.first, item.second);
  }

  // Matrix
  for (auto& item : _matrices) {
    _effect->setMatrix(item.first, item.second);
  }

  if (!_texture) {
    return;
  }

  if (isCube) {
    for (unsigned int face = 0; face < 6; ++face) {
      engine->bindFramebuffer(_texture, face, std::nullopt, std::nullopt, true);

      // VBOs
      engine->bindBuffers(_vertexBuffers, _indexBuffer, _effect);

      _effect->setFloat("face", static_cast<float>(face));

      // Clear
      if (autoClear) {
        engine->clear(scene->clearColor, true, false, false);
      }

      // Draw order
      engine->drawElementsType(Material::TriangleFillMode, 0, 6);

      // Mipmaps
      if (face == 5) {
        engine->generateMipMapsForCubemap(_texture);
      }
    }
  }
  else {
    engine->bindFramebuffer(_texture, 0u, std::nullopt, std::nullopt, true);

    // VBOs
    engine->bindBuffers(_vertexBuffers, _indexBuffer, _effect);

    // Clear
    if (autoClear) {
      engine->clear(scene->clearColor, true, false, false);
    }

    // Draw order
    engine->drawElementsType(Material::TriangleFillMode, 0, 6);
  }

  // Unbind
  engine->unBindFramebuffer(_texture, isCube);

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

  if (_indexBuffer && _engine->_releaseBuffer(_indexBuffer)) {
    _indexBuffer = nullptr;
  }

  Texture::dispose();
}

} // namespace BABYLON
