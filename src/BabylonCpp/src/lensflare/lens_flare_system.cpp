#include <babylon/lensflare/lens_flare_system.h>

#include <babylon/cameras/camera.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/lensflare/lens_flare.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

LensFlareSystem::LensFlareSystem(const std::string iName, Mesh* emitter,
                                 Scene* scene)
    : id{iName}
    , name{iName}
    , borderLimit{300}
    , viewportBorder{0.f}
    , layerMask{0x0FFFFFFF}
    , _scene{scene}
    , _emitter{emitter}
    , _isEnabled{true}
{
  meshesSelectionPredicate = [this](Mesh* m) {
    return m->material() && m->isVisible && m->isEnabled() && m->isBlocker
           && ((m->layerMask & _scene->activeCamera->layerMask) != 0);
  };

  auto engine = scene->getEngine();

  // VBO
  Float32Array vertices = {1.f,  1.f,  //
                           -1.f, 1.f,  //
                           -1.f, -1.f, //
                           1.f,  -1.f};

  _vertexBuffers[VertexBuffer::PositionKindChars]
    = std_util::make_unique<VertexBuffer>(
      engine, vertices, VertexBuffer::PositionKind, false, false, 2);

  // Indices
  Uint32Array indices = {0, 1, 2, 0, 2, 3};
  _indexBuffer        = engine->createIndexBuffer(indices);

  // Effects
  EffectCreationOptions effectCreationOptions;
  effectCreationOptions.attributes    = {VertexBuffer::PositionKindChars};
  effectCreationOptions.uniformsNames = {"color", "viewportMatrix"};
  effectCreationOptions.samplers      = {"textureSampler"};

  _effect = _scene->getEngine()->createEffect(
    "lensFlare", effectCreationOptions, _scene->getEngine());
}

LensFlareSystem::~LensFlareSystem()
{
}

void LensFlareSystem::addToScene(
  std::unique_ptr<LensFlareSystem>&& lensFlareSystem)
{
  _scene->lensFlareSystems.emplace_back(std::move(lensFlareSystem));
}

bool LensFlareSystem::isEnabled() const
{
  return _isEnabled;
}

void LensFlareSystem::setIsEnabled(bool value)
{
  _isEnabled = value;
}

Scene* LensFlareSystem::getScene()
{
  return _scene;
}

Mesh* LensFlareSystem::getEmitter()
{
  return _emitter;
}

void LensFlareSystem::setEmitter(Mesh* newEmitter)
{
  _emitter = newEmitter;
}

Vector3 LensFlareSystem::getEmitterPosition()
{
  return _emitter->position();
}

bool LensFlareSystem::computeEffectivePosition(Viewport& globalViewport)
{
  auto position = getEmitterPosition();

  auto identityMatrix  = Matrix::Identity();
  auto transformMatrix = _scene->getTransformMatrix();
  position = Vector3::Project(position, identityMatrix, transformMatrix,
                              globalViewport);

  _positionX = position.x;
  _positionY = position.y;

  position = Vector3::TransformCoordinates(getEmitterPosition(),
                                           _scene->getViewMatrix());

  if (viewportBorder > 0) {
    globalViewport.x -= viewportBorder;
    globalViewport.y -= viewportBorder;
    globalViewport.width += viewportBorder * 2;
    globalViewport.height += viewportBorder * 2;
    position.x += viewportBorder;
    position.y += viewportBorder;
    _positionX += viewportBorder;
    _positionY += viewportBorder;
  }

  if (position.z > 0.f) {
    if ((_positionX > globalViewport.x)
        && (_positionX < globalViewport.x + globalViewport.width)) {
      if ((_positionY > globalViewport.y)
          && (_positionY < globalViewport.y + globalViewport.height)) {
        return true;
      }
    }
    return true;
  }

  return false;
}

bool LensFlareSystem::_isVisible()
{
  if (!_isEnabled) {
    return false;
  }

  auto emitterPosition = getEmitterPosition();
  auto direction
    = emitterPosition.subtract(_scene->activeCamera->globalPosition());
  auto distance = direction.length();
  direction.normalize();

  Ray ray(_scene->activeCamera->globalPosition(), direction);
  auto pickInfo = _scene->pickWithRay(ray, meshesSelectionPredicate, true);

  return !pickInfo->hit || pickInfo->distance > distance;
}

bool LensFlareSystem::render()
{
  if (!_effect->isReady()) {
    return false;
  }

  auto engine         = _scene->getEngine();
  auto viewport       = _scene->activeCamera->viewport;
  auto globalViewport = viewport.toGlobal(engine->getRenderWidth(true),
                                          engine->getRenderHeight(true));

  // Position
  if (!computeEffectivePosition(globalViewport)) {
    return false;
  }

  // Visibility
  if (!_isVisible()) {
    return false;
  }

  // Intensity
  float awayX;
  float awayY;

  const auto borderLimitf    = static_cast<float>(borderLimit);
  auto globalViewport_x      = static_cast<float>(globalViewport.x);
  auto globalViewport_y      = static_cast<float>(globalViewport.y);
  auto globalViewport_width  = static_cast<float>(globalViewport.width);
  auto globalViewport_height = static_cast<float>(globalViewport.height);

  if (_positionX < borderLimitf + globalViewport_x) {
    awayX = borderLimitf + globalViewport_x - _positionX;
  }
  else if (_positionX
           > globalViewport_x + globalViewport_width - borderLimitf) {
    awayX = _positionX - globalViewport_x - globalViewport_width + borderLimitf;
  }
  else {
    awayX = 0.f;
  }

  if (_positionY < borderLimitf + globalViewport_y) {
    awayY = borderLimitf + globalViewport_y - _positionY;
  }
  else if (_positionY
           > globalViewport_y + globalViewport_height - borderLimitf) {
    awayY
      = _positionY - globalViewport_y - globalViewport_height + borderLimitf;
  }
  else {
    awayY = 0.f;
  }

  auto away = (awayX > awayY) ? awayX : awayY;
  away -= viewportBorder;
  if (away > borderLimitf) {
    away = borderLimitf;
  }

  auto intensity = 1.f - (away / borderLimitf);
  if (intensity < 0.f) {
    return false;
  }

  if (intensity > 1.f) {
    intensity = 1.f;
  }

  if (viewportBorder > 0) {
    globalViewport_x += viewportBorder;
    globalViewport_y += viewportBorder;
    globalViewport_width -= viewportBorder * 2;
    globalViewport_height -= viewportBorder * 2;
    _positionX -= viewportBorder;
    _positionY -= viewportBorder;
  }

  // Position
  auto centerX = globalViewport_x + globalViewport_width / 2.f;
  auto centerY = globalViewport_y + globalViewport_height / 2.f;
  auto distX   = centerX - _positionX;
  auto distY   = centerY - _positionY;

  // Effects
  engine->enableEffect(_effect);
  engine->setState(false);
  engine->setDepthBuffer(false);
  engine->setAlphaMode(Engine::ALPHA_ONEONE);

  // VBOs
  std::unordered_map<std::string, VertexBuffer*> vertexBuffersTmp;
  vertexBuffersTmp.reserve(_vertexBuffers.size());
  for (const auto& item : _vertexBuffers) {
    vertexBuffersTmp[item.first] = item.second.get();
  }
  engine->bindBuffers(vertexBuffersTmp, _indexBuffer.get(), _effect);

  // Flares
  for (auto& flare : lensFlares) {
    engine->setAlphaMode(flare->alphaMode);

    auto x = centerX - (distX * flare->position.x);
    auto y = centerY - (distY * flare->position.y);

    auto cw = flare->size;
    auto ch = flare->size * engine->getAspectRatio(_scene->activeCamera);
    auto cx = 2.f * (x / (globalViewport_width + globalViewport_x * 2.f)) - 1.f;
    auto cy
      = 1.f - 2.f * (y / (globalViewport_height + globalViewport_y * 2.f));

    auto viewportMatrix = Matrix::FromValues(cw / 2, 0, 0, 0, //
                                             0, ch / 2, 0, 0, //
                                             0, 0, 1, 0,      //
                                             cx, cy, 0, 1);

    _effect->setMatrix("viewportMatrix", viewportMatrix);

    // Texture
    _effect->setTexture("textureSampler", flare->texture);

    // Color
    _effect->setFloat4("color", flare->color.r * intensity,
                       flare->color.g * intensity, flare->color.b * intensity,
                       1.f);

    // Draw order
    engine->draw(true, 0, 6);
  }

  engine->setDepthBuffer(true);
  engine->setAlphaMode(Engine::ALPHA_DISABLE);
  return true;
}

void LensFlareSystem::dispose(bool /*doNotRecurse*/)
{
  if (std_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)) {
    auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKindChars];
    if (vertexBuffer) {
      vertexBuffer->dispose();
      _vertexBuffers.erase(VertexBuffer::PositionKindChars);
      vertexBuffer.reset(nullptr);
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }

  for (auto& lensFlare : lensFlares) {
    lensFlare->dispose();
  }

  lensFlares.clear();

  // Remove from scene
  _scene->lensFlareSystems.erase(
    std::remove_if(
      _scene->lensFlareSystems.begin(), _scene->lensFlareSystems.end(),
      [this](const std::unique_ptr<LensFlareSystem>& lensFlareSystem) {
        return lensFlareSystem.get() == this;
      }),
    _scene->lensFlareSystems.end());
}

LensFlareSystem*
LensFlareSystem::Parse(const Json::value& /*parsedLensFlareSystem*/,
                       Scene* /*scene*/, const std::string& /*rootUrl*/)
{
#if 0
  auto emitterId = Json::GetString(parsedLensFlareSystem, "emitterId");
  auto emitter   = scene->getLastEntryByID(emitterId);

  std::string name = parsedLensFlareSystem.contains("name") ?
                       Json::GetString(parsedLensFlareSystem, "name") :
                       "lensFlareSystem#" + emitterId;

  auto lensFlareSystem = LensFlareSystem::New(name, emitter, scene);

  lensFlareSystem->id = parsedLensFlareSystem.contains("id") ?
                          Json::GetString(parsedLensFlareSystem, "id") :
                          name;
  lensFlareSystem->borderLimit
    = Json::GetNumber(parsedLensFlareSystem, "borderLimit", 300);

  for (auto& parsedFlare : Json::GetArray(parsedLensFlareSystem, "flares")) {
    LensFlare::New(
      Json::GetNumber<float>(parsedFlare, "size", 0.f),
      Vector3::FromArray(Json::ToArray<float>(parsedFlare, "position")),
      Color3::FromArray(Json::ToArray<float>(parsedFlare, "color")),
      rootUrl + Json::GetString(parsedFlare, "textureName"), lensFlareSystem);
  }

  return lensFlareSystem;
#endif
  return nullptr;
}

Json::object LensFlareSystem::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
