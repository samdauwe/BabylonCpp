#include <babylon/sprites/sprite_manager.h>

#include <babylon/cameras/camera.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SpriteManager::SpriteManager(const string_t& iName, const string_t& imgUrl,
                             unsigned int capacity, const ISize& cellSize,
                             Scene* scene, float epsilon,
                             unsigned int samplingMode)
    : name{iName}
    , renderingGroupId{0}
    , layerMask{0x0FFFFFFF}
    , fogEnabled{true}
    , isPickable{false}
    , _epsilon{epsilon}
    , _capacity{capacity}
    , _scene{scene}
{
  _spriteTexture = Texture::New(imgUrl, scene, true, false, samplingMode);
  _spriteTexture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _spriteTexture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;

  if (cellSize.width != -1 && cellSize.height != -1) {
    cellWidth  = cellSize.width;
    cellHeight = cellSize.height;
  }
  else if (cellSize.width != -1 && cellSize.height == -1) {
    cellWidth  = cellSize.width;
    cellHeight = cellSize.width;
  }
  else if (cellSize.width == -1 && cellSize.height != -1) {
    cellWidth  = cellSize.height;
    cellHeight = cellSize.height;
  }
  else {
    return;
  }

  Uint32Array indices;
  int index = 0;
  for (unsigned int count = 0; count < capacity; ++count) {
    indices.emplace_back(index + 0);
    indices.emplace_back(index + 1);
    indices.emplace_back(index + 2);
    indices.emplace_back(index + 0);
    indices.emplace_back(index + 2);
    indices.emplace_back(index + 3);
    index += 4;
  }

  _indexBuffer = scene->getEngine()->createIndexBuffer(indices);

  // VBO
  // 16 floats per sprite (x, y, z, angle, sizeX, sizeY, offsetX, offsetY,
  // invertU, invertV, cellIndexX, cellIndexY, color r, color g, color b, color
  // a)
  _vertexData.resize(capacity * 16 * 4);
  _buffer
    = ::std::make_unique<Buffer>(scene->getEngine(), _vertexData, true, 16);

  auto positions
    = _buffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 4);
  auto options  = _buffer->createVertexBuffer(VertexBuffer::OptionsKind, 4, 4);
  auto cellInfo = _buffer->createVertexBuffer(VertexBuffer::CellInfoKind, 8, 4);
  auto colors   = _buffer->createVertexBuffer(VertexBuffer::ColorKind, 12, 4);

  _vertexBufferPtrs[VertexBuffer::PositionKindChars] = positions.get();
  _vertexBufferPtrs[VertexBuffer::OptionsKindChars]  = options.get();
  _vertexBufferPtrs[VertexBuffer::CellInfoKindChars] = cellInfo.get();
  _vertexBufferPtrs[VertexBuffer::ColorKindChars]    = colors.get();

  _vertexBuffers[VertexBuffer::PositionKindChars] = ::std::move(positions);
  _vertexBuffers[VertexBuffer::OptionsKindChars]  = ::std::move(options);
  _vertexBuffers[VertexBuffer::CellInfoKindChars] = ::std::move(cellInfo);
  _vertexBuffers[VertexBuffer::ColorKindChars]    = ::std::move(colors);

  // Effects

  {
    EffectCreationOptions options;
    options.attributes = {VertexBuffer::PositionKindChars, "options",
                          "cellInfo", VertexBuffer::ColorKindChars};
    options.uniformsNames = {"view", "projection", "textureInfos", "alphaTest"};
    options.samplers      = {"diffuseSampler"};

    _effectBase = _scene->getEngine()->createEffect("sprites", options,
                                                    _scene->getEngine());
  }

  {
    EffectCreationOptions options;
    options.attributes = {VertexBuffer::PositionKindChars, "options",
                          "cellInfo", VertexBuffer::ColorKindChars};
    options.uniformsNames = {"view",      "projection", "textureInfos",
                             "alphaTest", "vFogInfos",  "vFogColor"};
    options.samplers = {"diffuseSampler"};
    options.defines  = "#define FOG";

    _effectFog = _scene->getEngine()->createEffect("sprites", options,
                                                   _scene->getEngine());
  }
}

SpriteManager::~SpriteManager()
{
}

void SpriteManager::addToScene(unique_ptr_t<SpriteManager>&& newSpriteManager)
{
  _scene->spriteManagers.emplace_back(::std::move(newSpriteManager));
}

Texture* SpriteManager::texture() const
{
  return _spriteTexture;
}

void SpriteManager::texture(Texture* value)
{
  _spriteTexture = value;
}

void SpriteManager::setOnDispose(
  const ::std::function<void(SpriteManager*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void SpriteManager::_appendSpriteVertex(size_t index, Sprite* sprite,
                                        int offsetX, int offsetY, int rowSize)
{
  size_t arrayOffset = index * 16;

  float offsetXVal = static_cast<float>(offsetX);
  float offsetYVal = static_cast<float>(offsetY);

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

  _vertexData[arrayOffset + 0] = sprite->position.x;
  _vertexData[arrayOffset + 1] = sprite->position.y;
  _vertexData[arrayOffset + 2] = sprite->position.z;
  _vertexData[arrayOffset + 3] = sprite->angle;
  _vertexData[arrayOffset + 4] = static_cast<float>(sprite->width);
  _vertexData[arrayOffset + 5] = static_cast<float>(sprite->height);
  _vertexData[arrayOffset + 6] = offsetXVal;
  _vertexData[arrayOffset + 7] = offsetYVal;
  _vertexData[arrayOffset + 8] = sprite->invertU ? 1.f : 0.f;
  _vertexData[arrayOffset + 9] = sprite->invertV ? 1.f : 0.f;
  float offset = static_cast<float>((sprite->cellIndex / rowSize) >> 0);
  _vertexData[arrayOffset + 10]
    = static_cast<float>(static_cast<float>(sprite->cellIndex)
                         - offset * static_cast<float>(rowSize));
  _vertexData[arrayOffset + 11] = offset;
  // Color
  _vertexData[arrayOffset + 12] = sprite->color->r;
  _vertexData[arrayOffset + 13] = sprite->color->g;
  _vertexData[arrayOffset + 14] = sprite->color->b;
  _vertexData[arrayOffset + 15] = sprite->color->a;
}

PickingInfo*
SpriteManager::intersects(const Ray ray, Camera* camera,
                          ::std::function<bool(Sprite* sprite)> predicate,
                          bool fastCheck)
{
  auto count               = ::std::min(_capacity, sprites.size());
  auto min                 = Vector3::Zero();
  auto max                 = Vector3::Zero();
  auto distance            = numeric_limits_t<float>::max();
  Sprite* currentSprite    = nullptr;
  auto cameraSpacePosition = Vector3::Zero();
  auto cameraView          = camera->getViewMatrix();

  for (unsigned int index = 0; index < count; ++index) {
    auto& sprite = sprites[index];
    if (!sprite) {
      continue;
    }

    if (predicate) {
      if (!predicate(sprite.get())) {
        continue;
      }
    }
    else if (!sprite->isPickable) {
      continue;
    }

    Vector3::TransformCoordinatesToRef(sprite->position, cameraView,
                                       cameraSpacePosition);

    min.copyFromFloats(
      cameraSpacePosition.x - static_cast<float>(sprite->width) / 2.f,
      cameraSpacePosition.y - static_cast<float>(sprite->height) / 2.f,
      cameraSpacePosition.z);
    max.copyFromFloats(
      cameraSpacePosition.x + static_cast<float>(sprite->width) / 2.f,
      cameraSpacePosition.y + static_cast<float>(sprite->height) / 2.f,
      cameraSpacePosition.z);

    if (ray.intersectsBoxMinMax(min, max)) {
      auto currentDistance = Vector3::Distance(cameraSpacePosition, ray.origin);

      if (distance > currentDistance) {
        distance      = currentDistance;
        currentSprite = sprite.get();

        if (fastCheck) {
          break;
        }
      }
    }
  }

  if (currentSprite) {
    PickingInfo* result = new PickingInfo();

    result->hit          = true;
    result->pickedSprite = currentSprite;
    result->distance     = distance;

    return result;
  }

  return nullptr;
}

void SpriteManager::render()
{
  // Check
  if (!_effectBase->isReady() || !_effectFog->isReady() || !_spriteTexture
      || !_spriteTexture->isReady())
    return;

  auto engine   = _scene->getEngine();
  auto baseSize = _spriteTexture->getBaseSize();

  // Sprites
  auto deltaTime = engine->getDeltaTime();
  size_t max     = ::std::min(_capacity, sprites.size());
  int rowSize    = baseSize.width / cellWidth;

  unsigned int offset = 0;
  for (auto& sprite : sprites) {
    if (!sprite) {
      continue;
    }

    sprite->_animate(deltaTime);

    _appendSpriteVertex(offset++, sprite.get(), 0, 0, rowSize);
    _appendSpriteVertex(offset++, sprite.get(), 1, 0, rowSize);
    _appendSpriteVertex(offset++, sprite.get(), 1, 1, rowSize);
    _appendSpriteVertex(offset++, sprite.get(), 0, 1, rowSize);
  }
  _buffer->update(_vertexData);

  // Render
  auto effect = _effectBase;

  if (_scene->fogEnabled() && _scene->fogMode() != Scene::FOGMODE_NONE
      && fogEnabled) {
    effect = _effectFog;
  }

  engine->enableEffect(effect);

  auto viewMatrix = _scene->getViewMatrix();
  effect->setTexture("diffuseSampler", _spriteTexture);
  effect->setMatrix("view", viewMatrix);
  effect->setMatrix("projection", _scene->getProjectionMatrix());
  effect->setFloat2(
    "textureInfos",
    static_cast<float>(cellWidth) / static_cast<float>(baseSize.width),
    static_cast<float>(cellWidth) / static_cast<float>(baseSize.height));

  // Fog
  if (_scene->fogEnabled() && _scene->fogMode() != Scene::FOGMODE_NONE
      && fogEnabled) {
    effect->setFloat4("vFogInfos", static_cast<float>(_scene->fogMode()),
                      _scene->fogStart, _scene->fogEnd, _scene->fogDensity);
    effect->setColor3("vFogColor", _scene->fogColor);
  }

  // VBOs
  engine->bindBuffers(_vertexBufferPtrs, _indexBuffer.get(), effect);

  // Draw order
  engine->setDepthFunctionToLessOrEqual();
  effect->setBool("alphaTest", true);
  engine->setColorWrite(false);
  engine->draw(true, 0, static_cast<int>(max * 6));
  engine->setColorWrite(true);
  effect->setBool("alphaTest", false);

  engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
  engine->draw(true, 0, static_cast<int>(max * 6));
  engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
}

void SpriteManager::dispose(bool /*doNotRecurse*/)
{
  if (_buffer) {
    _buffer->dispose();
    _buffer.reset(nullptr);
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }

  if (_spriteTexture) {
    _spriteTexture->dispose();
    _spriteTexture = nullptr;
  }

  // Remove from scene
  _scene->spriteManagers.erase(
    ::std::remove_if(_scene->spriteManagers.begin(),
                     _scene->spriteManagers.end(),
                     [this](const unique_ptr_t<SpriteManager>& spriteManager) {
                       return spriteManager.get() == this;
                     }),
    _scene->spriteManagers.end());

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

} // end of namespace BABYLON
