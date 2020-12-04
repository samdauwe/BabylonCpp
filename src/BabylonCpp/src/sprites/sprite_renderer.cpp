#include <babylon/sprites/sprite_renderer.h>

#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/textures/thin_texture.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/sprites/thin_sprite.h>
#include <babylon/states/depth_culling_state.h>

namespace BABYLON {

SpriteRenderer::SpriteRenderer(ThinEngine* engine, size_t capacity, float epsilon, Scene* scene)
    : texture{nullptr}
    , cellWidth{0}
    , cellHeight{0}
    , blendMode{Constants::ALPHA_COMBINE}
    , autoResetAlpha{true}
    , disableDepthWrite{false}
    , fogEnabled{true}
    , capacity{this, &SpriteRenderer::get_capacity}
    , _engine{nullptr}
    , _useVAO{false}
    , _useInstancing{false}
    , _scene{nullptr}
    , _buffer{nullptr}
    , _spriteBuffer{nullptr}
    , _indexBuffer{nullptr}
    , _effectBase{nullptr}
    , _effectFog{nullptr}
    , _vertexArrayObject{nullptr}
{
  _capacity = capacity;
  _epsilon  = epsilon;

  _engine        = engine;
  _useInstancing = engine->getCaps().instancedArrays;
  _useVAO        = engine->getCaps().vertexArrayObject && !engine->disableVertexArrayObjects;
  _scene         = scene;

  if (!_useInstancing) {
    IndicesArray indices;
    auto index = 0;
    for (unsigned int count = 0; count < capacity; ++count) {
      indices.emplace_back(index + 0);
      indices.emplace_back(index + 1);
      indices.emplace_back(index + 2);
      indices.emplace_back(index + 0);
      indices.emplace_back(index + 2);
      indices.emplace_back(index + 3);
      index += 4;
    }

    _indexBuffer = engine->createIndexBuffer(indices);
  }

  // VBO
  // 18 floats per sprite (x, y, z, angle, sizeX, sizeY, offsetX, offsetY, invertU, invertV,
  // cellLeft, cellTop, cellWidth, cellHeight, color r, color g, color b, color a)
  // 16 when using instances
  _vertexBufferSize = _useInstancing ? 16 : 18;
  _vertexData       = Float32Array(capacity * _vertexBufferSize * (_useInstancing ? 1 : 4), 0.f);
  _buffer           = std::make_unique<Buffer>(engine, _vertexData, true, _vertexBufferSize);

  auto positions = _buffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 4, _vertexBufferSize,
                                               _useInstancing);
  auto options   = _buffer->createVertexBuffer("options", 4, 2, _vertexBufferSize, _useInstancing);

  auto offset                           = 6ull;
  std::unique_ptr<VertexBuffer> offsets = nullptr;

  if (_useInstancing) {
    const Float32Array spriteData{0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};
    _spriteBuffer = std::make_unique<Buffer>(engine, spriteData, false, 2);
    offsets       = _spriteBuffer->createVertexBuffer("offsets", 0, 2);
  }
  else {
    offsets = _buffer->createVertexBuffer("offsets", offset, 2, _vertexBufferSize, _useInstancing);
    offset += 2;
  }

  auto inverts
    = _buffer->createVertexBuffer("inverts", offset, 2, _vertexBufferSize, _useInstancing);
  auto cellInfo
    = _buffer->createVertexBuffer("cellInfo", offset + 2, 4, _vertexBufferSize, _useInstancing);
  auto colors = _buffer->createVertexBuffer(VertexBuffer::ColorKind, offset + 6, 4,
                                            _vertexBufferSize, _useInstancing);

  _vertexBuffers[VertexBuffer::PositionKind] = std::move(positions);
  _vertexBuffers[VertexBuffer::OptionsKind]  = std::move(options);
  _vertexBuffers[VertexBuffer::OffsetsKind]  = std::move(offsets);
  _vertexBuffers[VertexBuffer::InvertsKind]  = std::move(inverts);
  _vertexBuffers[VertexBuffer::CellInfoKind] = std::move(cellInfo);
  _vertexBuffers[VertexBuffer::ColorKind]    = std::move(colors);

  // Effects

  {
    IEffectCreationOptions spriteOptions;
    spriteOptions.attributes
      = {VertexBuffer::PositionKind, "options", "offsets", "inverts", "cellInfo",
         VertexBuffer::ColorKind};
    spriteOptions.uniformsNames = {"view", "projection", "textureInfos", "alphaTest"};
    spriteOptions.samplers      = {"diffuseSampler"};

    _effectBase = _engine->createEffect("sprites", spriteOptions, _scene->getEngine());
  }

  if (_scene) {
    IEffectCreationOptions spriteOptions;
    spriteOptions.attributes
      = {VertexBuffer::PositionKind, "options", "offsets", "inverts", "cellInfo",
         VertexBuffer::ColorKind};
    spriteOptions.uniformsNames
      = {"view", "projection", "textureInfos", "alphaTest", "vFogInfos", "vFogColor"};
    spriteOptions.samplers = {"diffuseSampler"};
    spriteOptions.defines  = "#define FOG";

    _effectFog = _scene->getEngine()->createEffect("sprites", spriteOptions, _scene->getEngine());
  }
}

SpriteRenderer::~SpriteRenderer() = default;

size_t SpriteRenderer::get_capacity() const
{
  return _capacity;
}

void SpriteRenderer::render(
  const std::vector<ThinSpritePtr>& sprites, float deltaTime, const Matrix& viewMatrix,
  const Matrix& projectionMatrix,
  const std::function<void(ThinSprite* sprite, const ISize& baseSize)>& customSpriteUpdate)
{
  if (!texture || !texture->isReady() || sprites.empty()) {
    return;
  }

  auto effect          = _effectBase;
  auto shouldRenderFog = false;
  if (fogEnabled && _scene && _scene->fogEnabled() && _scene->fogMode() != 0) {
    effect          = _effectFog;
    shouldRenderFog = true;
  }

  // Check
  if (!effect->isReady()) {
    return;
  }
  auto engine                     = _scene->getEngine();
  const auto useRightHandedSystem = !!(_scene && _scene->useRightHandedSystem());
  const auto baseSize             = texture->getBaseSize();

  // Sprites
  auto max = std::min(_capacity, sprites.size());

  auto offset   = 0u;
  auto noSprite = true;
  for (size_t index = 0; index < max; index++) {
    const auto& sprite = sprites[index];
    if (!sprite || !sprite->isVisible) {
      continue;
    }

    noSprite = false;
    sprite->_animate(deltaTime);

    _appendSpriteVertex(offset++, sprite, 0, 0, baseSize, useRightHandedSystem, customSpriteUpdate);
    if (!_useInstancing) {
      _appendSpriteVertex(offset++, sprite, 1, 0, baseSize, useRightHandedSystem,
                          customSpriteUpdate);
      _appendSpriteVertex(offset++, sprite, 1, 1, baseSize, useRightHandedSystem,
                          customSpriteUpdate);
      _appendSpriteVertex(offset++, sprite, 0, 1, baseSize, useRightHandedSystem,
                          customSpriteUpdate);
    }
  }

  if (noSprite) {
    return;
  }

  _buffer->update(_vertexData);

  const auto culling = engine->depthCullingState()->cull().value_or(true);
  const auto zOffset = engine->depthCullingState()->zOffset();

  // Handle Right Handed
  if (useRightHandedSystem && _scene) {
    _scene->getEngine()->setState(culling, zOffset, false, false);
  }

  // Render
  engine->enableEffect(effect);

  effect->setTexture("diffuseSampler", texture);
  effect->setMatrix("view", viewMatrix);
  effect->setMatrix("projection", projectionMatrix);

  // Scene Info
  if (shouldRenderFog && _scene) {
    const auto scene = _scene;

    // Fog
    effect->setFloat4("vFogInfos", scene->fogMode(), scene->fogStart, scene->fogEnd,
                      scene->fogDensity);
    effect->setColor3("vFogColor", scene->fogColor);
  }

  if (_useVAO) {
    if (!_vertexArrayObject) {
      _vertexArrayObject = engine->recordVertexArrayObject(_vertexBuffers, _indexBuffer, effect);
    }
    engine->bindVertexArrayObject(_vertexArrayObject, _indexBuffer);
  }
  else {
    // VBOs
    engine->bindBuffers(_vertexBuffers, _indexBuffer, effect);
  }

  // Draw order
  engine->depthCullingState()->depthFunc = Constants::LEQUAL;
  if (!disableDepthWrite) {
    effect->setBool("alphaTest", true);
    engine->setColorWrite(false);
    if (_useInstancing) {
      engine->drawArraysType(Constants::MATERIAL_TriangleFanDrawMode, 0, 4, offset);
    }
    else {
      engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, (offset / 4) * 6);
    }
    engine->setColorWrite(true);
    effect->setBool("alphaTest", false);
  }

  engine->setAlphaMode(blendMode);
  if (_useInstancing) {
    engine->drawArraysType(Constants::MATERIAL_TriangleFanDrawMode, 0, 4, offset);
  }
  else {
    engine->drawElementsType(Constants::MATERIAL_TriangleFillMode, 0, (offset / 4) * 6);
  }

  if (autoResetAlpha) {
    engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }

  // Restore Right Handed
  if (useRightHandedSystem && _scene) {
    _scene->getEngine()->setState(culling, zOffset, false, true);
  }

  engine->unbindInstanceAttributes();
}

void SpriteRenderer::_appendSpriteVertex(
  size_t index, const ThinSpritePtr& sprite, int offsetX, int offsetY, const ISize& baseSize,
  bool useRightHandedSystem,
  const std::function<void(ThinSprite* sprite, const ISize& baseSize)>& customSpriteUpdate)
{
  size_t arrayOffset = index * _vertexBufferSize;

  auto offsetXVal = static_cast<float>(offsetX);
  auto offsetYVal = static_cast<float>(offsetY);

  if (offsetX == 0) {
    offsetXVal = _epsilon;
  }
  else if (offsetX == 1) {
    offsetXVal = 1.f - _epsilon;
  }

  if (offsetY == 0) {
    offsetYVal = _epsilon;
  }
  else if (offsetY == 1) {
    offsetYVal = 1.f - _epsilon;
  }

  if (customSpriteUpdate) {
    customSpriteUpdate(sprite.get(), baseSize);
  }
  else {
    if (!sprite->cellIndex) {
      sprite->cellIndex = 0;
    }

    const auto rowSize = baseSize.width / cellWidth;
    const auto offset  = (sprite->cellIndex / rowSize) >> 0;
    sprite->_xOffset   = (sprite->cellIndex - offset * rowSize) * cellWidth / baseSize.width;
    sprite->_yOffset   = offset * cellHeight / baseSize.height;
    sprite->_xSize     = cellWidth;
    sprite->_ySize     = cellHeight;
  }

  // Positions
  _vertexData[arrayOffset + 0] = sprite->position.x;
  _vertexData[arrayOffset + 1] = sprite->position.y;
  _vertexData[arrayOffset + 2] = sprite->position.z;
  _vertexData[arrayOffset + 3] = sprite->angle;
  // Options
  _vertexData[arrayOffset + 4] = static_cast<float>(sprite->width);
  _vertexData[arrayOffset + 5] = static_cast<float>(sprite->height);

  if (!_useInstancing) {
    _vertexData[arrayOffset + 6] = offsetXVal;
    _vertexData[arrayOffset + 7] = offsetYVal;
  }
  else {
    arrayOffset -= 2;
  }

  // Inverts according to Right Handed
  if (useRightHandedSystem) {
    _vertexData[arrayOffset + 8] = sprite->invertU ? 0.f : 1.f;
  }
  else {
    _vertexData[arrayOffset + 8] = sprite->invertU ? 1.f : 0.f;
  }

  _vertexData[arrayOffset + 9] = sprite->invertV ? 1.f : 0.f;

  _vertexData[arrayOffset + 10] = static_cast<float>(sprite->_xOffset);
  _vertexData[arrayOffset + 11] = static_cast<float>(sprite->_yOffset);
  _vertexData[arrayOffset + 12] = static_cast<float>(sprite->_xSize) / baseSize.width;
  _vertexData[arrayOffset + 13] = static_cast<float>(sprite->_ySize) / baseSize.height;

  // Color
  _vertexData[arrayOffset + 14] = sprite->color.r;
  _vertexData[arrayOffset + 15] = sprite->color.g;
  _vertexData[arrayOffset + 16] = sprite->color.b;
  _vertexData[arrayOffset + 17] = sprite->color.a;
}

void SpriteRenderer::dispose()
{
  if (_buffer) {
    _buffer->dispose();
    _buffer = nullptr;
  }

  if (_spriteBuffer) {
    _spriteBuffer->dispose();
    _spriteBuffer = nullptr;
  }

  if (_indexBuffer) {
    _engine->_releaseBuffer(_indexBuffer);
    _indexBuffer = nullptr;
  }

  if (_vertexArrayObject) {
    _engine->releaseVertexArrayObject(_vertexArrayObject);
    _vertexArrayObject = nullptr;
  }

  if (texture) {
    texture->dispose();
    texture = nullptr;
  }
}

} // end of namespace BABYLON
