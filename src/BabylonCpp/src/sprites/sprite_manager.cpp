#include <babylon/sprites/sprite_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/sprites/sprite_scene_component.h>

namespace BABYLON {

SpriteManager::SpriteManager(const std::string& iName, const std::string& imgUrl,
                             unsigned int capacity, const ISize& cellSize, Scene* scene,
                             float epsilon, unsigned int samplingMode, bool fromPacked,
                             const std::string& iSpriteJSON)
    : name{iName}
    , spriteJSON{iSpriteJSON}
    , fogEnabled{true}
    , onDispose{this, &SpriteManager::set_onDispose}
    , texture{this, &SpriteManager::get_texture, &SpriteManager::set_texture}
    , blendMode{this, &SpriteManager::get_blendMode, &SpriteManager::set_blendMode}
    , disableDepthWrite{false}
    , _packedAndReady{false}
    , _onDisposeObserver{nullptr}
    , _blendMode{Constants::ALPHA_COMBINE}
{
  auto component = std::static_pointer_cast<SpriteSceneComponent>(
    scene->_getComponent(SceneComponentConstants::NAME_SPRITE));
  if (!component) {
    component = SpriteSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  // ISpriteManager interface properties
  {
    layerMask        = 0x0FFFFFFF;
    isPickable       = false;
    renderingGroupId = 0;
  }

  _capacity             = capacity;
  _fromPacked           = fromPacked;
  _spriteTexture        = Texture::New(imgUrl, scene, true, false, samplingMode);
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

  _epsilon = epsilon;
  _scene   = scene;

  IndicesArray indices;
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
  // 18 floats per sprite (x, y, z, angle, sizeX, sizeY, offsetX, offsetY, invertU, invertV,
  // cellLeft, cellTop, cellWidth, cellHeight, color r, color g, color b, color a)
  _vertexData.resize(capacity * 18 * 4);
  _buffer = std::make_unique<Buffer>(scene->getEngine(), _vertexData, true, 18);

  auto positions = _buffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 4);
  auto options   = _buffer->createVertexBuffer(VertexBuffer::OptionsKind, 4, 4);
  auto inverts   = _buffer->createVertexBuffer(VertexBuffer::InvertsKind, 8, 2);
  auto cellInfo  = _buffer->createVertexBuffer(VertexBuffer::CellInfoKind, 10, 4);
  auto colors    = _buffer->createVertexBuffer(VertexBuffer::ColorKind, 14, 4);

  _vertexBuffers[VertexBuffer::PositionKind] = std::move(positions);
  _vertexBuffers[VertexBuffer::OptionsKind]  = std::move(options);
  _vertexBuffers[VertexBuffer::InvertsKind]  = std::move(inverts);
  _vertexBuffers[VertexBuffer::CellInfoKind] = std::move(cellInfo);
  _vertexBuffers[VertexBuffer::ColorKind]    = std::move(colors);

  // Effects

  {
    IEffectCreationOptions spriteOptions;
    spriteOptions.attributes
      = {VertexBuffer::PositionKind, "options", "inverts", "cellInfo", VertexBuffer::ColorKind};
    spriteOptions.uniformsNames = {"view", "projection", "textureInfos", "alphaTest"};
    spriteOptions.samplers      = {"diffuseSampler"};

    _effectBase = _scene->getEngine()->createEffect("sprites", spriteOptions, _scene->getEngine());
  }

  {
    IEffectCreationOptions spriteOptions;
    spriteOptions.attributes
      = {VertexBuffer::PositionKind, "options", "inverts", "cellInfo", VertexBuffer::ColorKind};
    spriteOptions.uniformsNames
      = {"view", "projection", "textureInfos", "alphaTest", "vFogInfos", "vFogColor"};
    spriteOptions.samplers = {"diffuseSampler"};
    spriteOptions.defines  = "#define FOG";

    _effectFog = _scene->getEngine()->createEffect("sprites", spriteOptions, _scene->getEngine());
  }

  if (_fromPacked) {
    _makePacked(imgUrl, spriteJSON);
  }
}

SpriteManager::~SpriteManager() = default;

void SpriteManager::addToScene(const SpriteManagerPtr& newSpriteManager)
{
  _scene->spriteManagers.emplace_back(newSpriteManager);
}

void SpriteManager::set_onDispose(const std::function<void(SpriteManager*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

TexturePtr& SpriteManager::get_texture()
{
  return _spriteTexture;
}

void SpriteManager::set_texture(const TexturePtr& value)
{
  _spriteTexture  = value;
  _textureContent = {};
}

unsigned int SpriteManager::get_blendMode() const
{
  return _blendMode;
}

void SpriteManager::set_blendMode(unsigned int blendMode)
{
  _blendMode = blendMode;
}

void SpriteManager::_makePacked(const std::string& /*imgUrl*/, const std::string& /*spriteJSON*/)
{
  // TODO Implement
}

void SpriteManager::_appendSpriteVertex(size_t index, Sprite& sprite, int offsetX, int offsetY,
                                        const ISize& baseSize)
{
  size_t arrayOffset = index * 18;

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

  // Positions
  _vertexData[arrayOffset + 0] = sprite.position.x;
  _vertexData[arrayOffset + 1] = sprite.position.y;
  _vertexData[arrayOffset + 2] = sprite.position.z;
  _vertexData[arrayOffset + 3] = sprite.angle;
  // Options
  _vertexData[arrayOffset + 4] = static_cast<float>(sprite.width);
  _vertexData[arrayOffset + 5] = static_cast<float>(sprite.height);
  _vertexData[arrayOffset + 6] = offsetXVal;
  _vertexData[arrayOffset + 7] = offsetYVal;
  // Inverts
  _vertexData[arrayOffset + 8] = sprite.invertU ? 1.f : 0.f;
  _vertexData[arrayOffset + 9] = sprite.invertV ? 1.f : 0.f;
  // CellIfo
  if (_packedAndReady) {
    if (sprite.cellRef.empty()) {
      sprite.cellIndex = 0;
    }
    auto num = sprite.cellIndex;
    if (StringTools::isDigit(num)) {
      sprite.cellRef = _spriteMap[static_cast<size_t>(sprite.cellIndex)];
    }
    /*
    const auto spriteCellRef = StringTools::toNumber<size_t>(sprite.cellRef);
    sprite._xOffset = _cellData[spriteCellRef].frame.x / baseSize.width;
    sprite._yOffset = _cellData[spriteCellRef].frame.y / baseSize.height;
    sprite._xSize = _cellData[spriteCellRef].frame.w;
    sprite._ySize = _cellData[spriteCellRef].frame.h;
    */
    _vertexData[arrayOffset + 10] = static_cast<float>(sprite._xOffset);
    _vertexData[arrayOffset + 11] = static_cast<float>(sprite._yOffset);
    _vertexData[arrayOffset + 12] = static_cast<float>(sprite._xSize) / baseSize.width;
    _vertexData[arrayOffset + 13] = static_cast<float>(sprite._ySize) / baseSize.height;
  }
  else {
    auto rowSize    = baseSize.width / cellWidth;
    auto offset     = (rowSize == 0) ? 0 : sprite.cellIndex / rowSize;
    sprite._xOffset = (sprite.cellIndex - offset * rowSize) * cellWidth / baseSize.width;
    sprite._yOffset = offset * cellHeight / baseSize.height;
    sprite._xSize   = cellWidth;
    sprite._ySize   = cellHeight;
    _vertexData[arrayOffset + 10] = static_cast<float>(sprite._xOffset);
    _vertexData[arrayOffset + 11] = static_cast<float>(sprite._yOffset);
    _vertexData[arrayOffset + 12] = static_cast<float>(cellWidth) / baseSize.width;
    _vertexData[arrayOffset + 13] = static_cast<float>(cellHeight) / baseSize.height;
  }
  // Color
  _vertexData[arrayOffset + 14] = sprite.color->r;
  _vertexData[arrayOffset + 15] = sprite.color->g;
  _vertexData[arrayOffset + 16] = sprite.color->b;
  _vertexData[arrayOffset + 17] = sprite.color->a;
}

bool SpriteManager::_checkTextureAlpha(Sprite& sprite, const Ray& ray, float distance,
                                       const Vector3& min, const Vector3& max)
{
  if (!sprite.useAlphaForPicking || !_spriteTexture) {
    return true;
  }

  const auto textureSize = _spriteTexture->getSize();
  if (_textureContent.empty()) {
    _textureContent = Uint8Array(static_cast<size_t>(textureSize.width * textureSize.height * 4));
    _spriteTexture->readPixels(0, 0, _textureContent);
  }

  auto& contactPoint = TmpVectors::Vector3Array[0];

  contactPoint.copyFrom(ray.direction);

  contactPoint.normalize();
  contactPoint.scaleInPlace(distance);
  contactPoint.addInPlace(ray.origin);

  const auto contactPointU = ((contactPoint.x - min.x) / (max.x - min.x)) - 0.5f;
  const auto contactPointV = (1.f - (contactPoint.y - min.y) / (max.y - min.y)) - 0.5f;

  // Rotate
  const auto angle    = sprite.angle;
  const auto rotatedU = 0.5f + (contactPointU * std::cos(angle) - contactPointV * std::sin(angle));
  const auto rotatedV = 0.5f + (contactPointU * std::sin(angle) + contactPointV * std::cos(angle));

  const auto u = sprite._xOffset * textureSize.width + rotatedU * sprite._xSize;
  const auto v = sprite._yOffset * textureSize.height + rotatedV * sprite._ySize;

  const auto alpha = _textureContent[static_cast<size_t>(u + v * textureSize.width) * 4 + 3];

  return (alpha > 0.5f);
}

std::optional<PickingInfo>
SpriteManager::intersects(const Ray& ray, const CameraPtr& camera,
                          const std::function<bool(Sprite* sprite)>& predicate, bool fastCheck)
{
  auto count                = std::min(_capacity, sprites.size());
  auto min                  = Vector3::Zero();
  auto max                  = Vector3::Zero();
  auto distance             = std::numeric_limits<float>::max();
  SpritePtr currentSprite   = nullptr;
  auto& pickedPoint         = TmpVectors::Vector3Array[0];
  auto& cameraSpacePosition = TmpVectors::Vector3Array[1];
  auto cameraView           = camera->getViewMatrix();

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

    Vector3::TransformCoordinatesToRef(sprite->position, cameraView, cameraSpacePosition);

    min.copyFromFloats(cameraSpacePosition.x - static_cast<float>(sprite->width) / 2.f,
                       cameraSpacePosition.y - static_cast<float>(sprite->height) / 2.f,
                       cameraSpacePosition.z);
    max.copyFromFloats(cameraSpacePosition.x + static_cast<float>(sprite->width) / 2.f,
                       cameraSpacePosition.y + static_cast<float>(sprite->height) / 2.f,
                       cameraSpacePosition.z);

    if (ray.intersectsBoxMinMax(min, max)) {
      auto currentDistance = Vector3::Distance(cameraSpacePosition, ray.origin);

      if (distance > currentDistance) {

        if (!_checkTextureAlpha(*sprite, ray, currentDistance, min, max)) {
          continue;
        }

        distance      = currentDistance;
        currentSprite = sprite;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  if (currentSprite) {
    PickingInfo result;

    cameraView.invertToRef(TmpVectors::MatrixArray[0]);
    result.hit          = true;
    result.pickedSprite = currentSprite;
    result.distance     = distance;

    // Get picked point
    auto& direction = TmpVectors::Vector3Array[0];
    direction.copyFrom(ray.direction);
    direction.normalize();
    direction.scaleInPlace(distance);

    ray.origin.addToRef(direction, pickedPoint);
    result.pickedPoint = Vector3::TransformCoordinates(pickedPoint, TmpVectors::MatrixArray[0]);

    return result;
  }

  return std::nullopt;
}

std::vector<PickingInfo>
SpriteManager::multiIntersects(const Ray& ray, const CameraPtr& camera,
                               const std::function<bool(Sprite* sprite)>& predicate)
{
  auto count     = std::min(_capacity, sprites.size());
  auto min       = Vector3::Zero();
  auto max       = Vector3::Zero();
  float distance = 0.f;
  std::vector<PickingInfo> results;
  auto pickedPoint         = TmpVectors::Vector3Array[0].copyFromFloats(0.f, 0.f, 0.f);
  auto cameraSpacePosition = TmpVectors::Vector3Array[1].copyFromFloats(0.f, 0.f, 0.f);
  auto cameraView          = camera->getViewMatrix();

  for (size_t index = 0; index < count; index++) {
    const auto& sprite = sprites[index];
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

    Vector3::TransformCoordinatesToRef(sprite->position, cameraView, cameraSpacePosition);

    min.copyFromFloats(cameraSpacePosition.x - sprite->width / 2.f,
                       cameraSpacePosition.y - sprite->height / 2.f, cameraSpacePosition.z);
    max.copyFromFloats(cameraSpacePosition.x + sprite->width / 2.f,
                       cameraSpacePosition.y + sprite->height / 2.f, cameraSpacePosition.z);

    if (ray.intersectsBoxMinMax(min, max)) {
      distance = Vector3::Distance(cameraSpacePosition, ray.origin);

      if (!_checkTextureAlpha(*sprite, ray, distance, min, max)) {
        continue;
      }

      PickingInfo result;
      results.emplace_back(result);

      cameraView.invertToRef(TmpVectors::MatrixArray[0]);
      result.hit          = true;
      result.pickedSprite = sprite;
      result.distance     = distance;

      // Get picked point
      auto& direction = TmpVectors::Vector3Array[2];
      direction.copyFrom(ray.direction);
      direction.normalize();
      direction.scaleInPlace(distance);

      ray.origin.addToRef(direction, pickedPoint);
      result.pickedPoint = Vector3::TransformCoordinates(pickedPoint, TmpVectors::MatrixArray[0]);
    }
  }

  return results;
}

void SpriteManager::render()
{
  // Check
  if (!_effectBase->isReady() || !_effectFog->isReady() || !_spriteTexture
      || !_spriteTexture->isReady() || sprites.empty()) {
    return;
  }

  if (_fromPacked && (!_packedAndReady || _spriteMap.empty() || _cellData.empty())) {
    return;
  }

  auto engine   = _scene->getEngine();
  auto baseSize = _spriteTexture->getBaseSize();

  // Sprites
  auto deltaTime = engine->getDeltaTime();
  auto max       = std::min(_capacity, sprites.size());

  auto offset   = 0u;
  auto noSprite = true;
  for (size_t index = 0; index < max; index++) {
    const auto& sprite = sprites[index];
    if (!sprite || !sprite->isVisible) {
      continue;
    }

    noSprite = false;
    sprite->_animate(deltaTime);

    _appendSpriteVertex(offset++, *sprite, 0, 0, baseSize);
    _appendSpriteVertex(offset++, *sprite, 1, 0, baseSize);
    _appendSpriteVertex(offset++, *sprite, 1, 1, baseSize);
    _appendSpriteVertex(offset++, *sprite, 0, 1, baseSize);
  }

  if (noSprite) {
    return;
  }

  _buffer->update(_vertexData);

  // Render
  auto effect = _effectBase;

  if (_scene->fogEnabled() && _scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    effect = _effectFog;
  }

  engine->enableEffect(effect);

  auto viewMatrix = _scene->getViewMatrix();
  effect->setTexture("diffuseSampler", _spriteTexture);
  effect->setMatrix("view", viewMatrix);
  effect->setMatrix("projection", _scene->getProjectionMatrix());

  // Fog
  if (_scene->fogEnabled() && _scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    effect->setFloat4("vFogInfos", static_cast<float>(_scene->fogMode()), _scene->fogStart,
                      _scene->fogEnd, _scene->fogDensity);
    effect->setColor3("vFogColor", _scene->fogColor);
  }

  // VBOs
  engine->bindBuffers(_vertexBuffers, _indexBuffer, effect);

  // Draw order
  engine->setDepthFunctionToLessOrEqual();
  if (!disableDepthWrite) {
    effect->setBool("alphaTest", true);
    engine->setColorWrite(false);
    engine->drawElementsType(Material::TriangleFillMode, 0, static_cast<int>((offset / 4.f) * 6));
    engine->setColorWrite(true);
    effect->setBool("alphaTest", false);
  }

  engine->setAlphaMode(_blendMode);
  engine->drawElementsType(Material::TriangleFillMode, 0, static_cast<int>((offset / 4.f) * 6));
  engine->setAlphaMode(Constants::ALPHA_DISABLE);
}

void SpriteManager::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  if (_buffer) {
    _buffer->dispose();
    _buffer = nullptr;
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer);
    _indexBuffer = nullptr;
  }

  if (_spriteTexture) {
    _spriteTexture->dispose();
    _spriteTexture = nullptr;
  }

  _textureContent = {};

  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_scene->spriteManagers, this);

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

} // end of namespace BABYLON
