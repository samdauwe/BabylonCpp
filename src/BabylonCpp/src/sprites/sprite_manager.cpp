#include <babylon/sprites/sprite_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/sprites/sprite_renderer.h>
#include <babylon/sprites/sprite_scene_component.h>

namespace BABYLON {

SpriteManager::SpriteManager(const std::string& iName, const std::string& imgUrl,
                             unsigned int capacity, const ISize& cellSize, Scene* scene,
                             float epsilon, unsigned int samplingMode, bool fromPacked,
                             const std::string& iSpriteJSON)
    : name{iName}
    , spriteJSON{iSpriteJSON}
    , onDispose{this, &SpriteManager::set_onDispose}
    , children{this, &SpriteManager::get_children}
    , scene{this, &SpriteManager::get_scene}
    , capacity{this, &SpriteManager::get_capacity}
    , texture{this, &SpriteManager::get_texture, &SpriteManager::set_texture}
    , cellWidth{this, &SpriteManager::get_cellWidth, &SpriteManager::set_cellWidth}
    , cellHeight{this, &SpriteManager::get_cellHeight, &SpriteManager::set_cellHeight}
    , fogEnabled{this, &SpriteManager::get_fogEnabled, &SpriteManager::set_fogEnabled}
    , blendMode{this, &SpriteManager::get_blendMode, &SpriteManager::set_blendMode}
    , disableDepthWrite{false}
    , _packedAndReady{false}
    , _onDisposeObserver{nullptr}
    , _spriteRendererTexture{nullptr}
{
  if (!scene) {
    scene = Engine::LastCreatedScene();
  }

  auto component = std::static_pointer_cast<SpriteSceneComponent>(
    scene->_getComponent(SceneComponentConstants::NAME_SPRITE));
  if (!component) {
    component = SpriteSceneComponent::New(scene);
    scene->_addComponent(component);
  }
  _fromPacked = fromPacked;

  _scene            = scene;
  const auto engine = _scene->getEngine();
  _spriteRenderer   = std::make_unique<SpriteRenderer>(engine, capacity, epsilon, scene);

  if (cellSize.width && cellSize.height) {
    cellWidth  = cellSize.width;
    cellHeight = cellSize.height;
  }
  else {
    _spriteRenderer = nullptr;
    return;
  }

  uniqueId = scene->getUniqueId();

  if (!imgUrl.empty()) {
    texture = Texture::New(imgUrl, scene, true, false, samplingMode);
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

std::vector<ThinSpritePtr>& SpriteManager::get_children()
{
  return sprites;
}

Scene*& SpriteManager::get_scene()
{
  return _scene;
}

size_t SpriteManager::get_capacity() const
{
  return _spriteRenderer->capacity();
}

TexturePtr& SpriteManager::get_texture()
{
  _spriteRendererTexture = std::static_pointer_cast<Texture>(_spriteRenderer->texture);
  return _spriteRendererTexture;
}

void SpriteManager::set_texture(const TexturePtr& value)
{
  value->wrapU             = TextureConstants::CLAMP_ADDRESSMODE;
  value->wrapV             = TextureConstants::CLAMP_ADDRESSMODE;
  _spriteRenderer->texture = value;
  _textureContent          = {};
}

int SpriteManager::get_cellWidth() const
{
  return _spriteRenderer->cellWidth;
}

void SpriteManager::set_cellWidth(int value)
{
  _spriteRenderer->cellWidth = value;
}

int SpriteManager::get_cellHeight() const
{
  return _spriteRenderer->cellHeight;
}

void SpriteManager::set_cellHeight(int value)
{
  _spriteRenderer->cellHeight = value;
}

bool SpriteManager::get_fogEnabled() const
{
  return _spriteRenderer->fogEnabled;
}

void SpriteManager::set_fogEnabled(bool value)
{
  _spriteRenderer->fogEnabled = value;
}

unsigned int SpriteManager::get_blendMode() const
{
  return _spriteRenderer->blendMode;
}

void SpriteManager::set_blendMode(unsigned int iBlendMode)
{
  _spriteRenderer->blendMode = iBlendMode;
}

std::string SpriteManager::getClassName() const
{
  return "SpriteManager";
}

void SpriteManager::_makePacked(const std::string& /*imgUrl*/, const std::string& /*spriteJSON*/)
{
  // TODO Implement
}

bool SpriteManager::_checkTextureAlpha(Sprite& sprite, const Ray& ray, float distance,
                                       const Vector3& min, const Vector3& max)
{
  if (!sprite.useAlphaForPicking || !texture()) {
    return true;
  }

  const auto textureSize = texture()->getSize();
  if (_textureContent.empty()) {
    _textureContent = Uint8Array(static_cast<size_t>(textureSize.width * textureSize.height * 4));
    texture()->readPixels(0, 0, _textureContent);
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
  auto count                = std::min(capacity(), sprites.size());
  auto min                  = Vector3::Zero();
  auto max                  = Vector3::Zero();
  auto distance             = std::numeric_limits<float>::max();
  SpritePtr currentSprite   = nullptr;
  auto& pickedPoint         = TmpVectors::Vector3Array[0];
  auto& cameraSpacePosition = TmpVectors::Vector3Array[1];
  auto cameraView           = camera->getViewMatrix();

  for (unsigned int index = 0; index < count; ++index) {
    auto sprite = std::static_pointer_cast<Sprite>(sprites[index]);
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
  auto count     = std::min(capacity(), sprites.size());
  auto min       = Vector3::Zero();
  auto max       = Vector3::Zero();
  float distance = 0.f;
  std::vector<PickingInfo> results;
  auto pickedPoint         = TmpVectors::Vector3Array[0].copyFromFloats(0.f, 0.f, 0.f);
  auto cameraSpacePosition = TmpVectors::Vector3Array[1].copyFromFloats(0.f, 0.f, 0.f);
  auto cameraView          = camera->getViewMatrix();

  for (size_t index = 0; index < count; index++) {
    const auto& sprite = std::static_pointer_cast<Sprite>(sprites[index]);
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
  if (_fromPacked && (!_packedAndReady || _spriteMap.empty() || _cellData.empty())) {
    return;
  }

  auto engine    = _scene->getEngine();
  auto deltaTime = engine->getDeltaTime();
  if (_packedAndReady) {
    _spriteRenderer->render(sprites, deltaTime, _scene->getViewMatrix(),
                            _scene->getProjectionMatrix(),
                            [this](ThinSprite* sprite, const ISize& baseSize) -> void {
                              _customUpdate(sprite, baseSize);
                            });
  }
  else {
    _spriteRenderer->render(sprites, deltaTime, _scene->getViewMatrix(),
                            _scene->getProjectionMatrix());
  }
}

void SpriteManager::_customUpdate(ThinSprite* sprite, const ISize& /*baseSize*/)
{
  if (sprite->cellRef.empty()) {
    sprite->cellIndex = 0;
  }
  auto num = sprite->cellIndex;
  if (std::isfinite(num) && std::floor(num) == num) {
    sprite->cellRef = _spriteMap[sprite->cellIndex];
  }
  // sprite->_xOffset = _cellData[sprite->cellRef].frame.x / baseSize.width;
  // sprite->_yOffset = _cellData[sprite->cellRef].frame.y / baseSize.height;
  // sprite->_xSize = _cellData[sprite->cellRef].frame.w;
  // sprite->_ySize = _cellData[sprite->cellRef].frame.h;
}

void SpriteManager::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  if (_spriteRenderer) {
    _spriteRenderer->dispose();
    _spriteRenderer = nullptr;
  }

  _textureContent = {};

  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_scene->spriteManagers, this);

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

json SpriteManager::serialize(bool /*serializeTexture*/) const
{
  return nullptr;
}

SpriteManagerPtr SpriteManager::Parse(const json& /*parsedManager*/, Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
