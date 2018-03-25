#include <babylon/particles/particle_system.h>

#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/scalar.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/cone_particle_emitter.h>
#include <babylon/particles/emittertypes/sphere_directed_particle_emitter.h>
#include <babylon/particles/emittertypes/sphere_particle_emitter.h>
#include <babylon/particles/particle.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ParticleSystem::ParticleSystem(const string_t& iName, size_t capacity,
                               Scene* scene, Effect* customEffect,
                               bool isAnimationSheetEnabled, float epsilon)
    : manualEmitCount{-1}
    , disposeOnStop{false}
    , minAngularSpeed{0.f}
    , maxAngularSpeed{0.f}
    , preventAutoStart{false}
    , updateFunction{nullptr}
    , onAnimationEnd{nullptr}
    , forceDepthWrite{false}
    , direction1{this, &ParticleSystem::get_direction1,
                 &ParticleSystem::set_direction1}
    , direction2{this, &ParticleSystem::get_direction2,
                 &ParticleSystem::set_direction2}
    , minEmitBox{this, &ParticleSystem::get_minEmitBox,
                 &ParticleSystem::set_minEmitBox}
    , maxEmitBox{this, &ParticleSystem::get_maxEmitBox,
                 &ParticleSystem::set_maxEmitBox}
    , textureMask{Color4(1.f, 1.f, 1.f, 1.f)}
    , spriteCellLoop{true}
    , spriteCellChangeSpeed{0.f}
    , startSpriteCellID{0}
    , endSpriteCellID{0}
    , spriteCellWidth{0}
    , spriteCellHeight{0}
    , _vertexBufferSize{11}
    , _epsilon{epsilon}
    , _capacity{capacity}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _newPartsExcess{0}
    , _customEffect{customEffect}
    , _scaledColorStep{Color4(0.f, 0.f, 0.f, 0.f)}
    , _colorDiff{Color4(0.f, 0.f, 0.f, 0.f)}
    , _scaledDirection{Vector3::Zero()}
    , _scaledGravity{Vector3::Zero()}
    , _currentRenderId{-1}
    , _started{false}
    , _stopped{false}
    , _actualFrame{0}
    , _isAnimationSheetEnabled{isAnimationSheetEnabled}
    , _appendParticleVertexes{nullptr}
    , _zeroVector3{Vector3::Zero()}
{
  _scene->particleSystems.emplace_back(this);

  // IParticleSystem
  {
    id                  = iName;
    name                = iName;
    renderingGroupId    = 0;
    emitRate            = 10.f;
    updateSpeed         = 0.01f;
    targetStopDuration  = 0;
    particleTexture     = nullptr;
    blendMode           = ParticleSystem::BLENDMODE_ONEONE;
    minLifeTime         = 1.f;
    maxLifeTime         = 1.f;
    minSize             = 1.f;
    maxSize             = 1.f;
    layerMask           = 0x0FFFFFFF;
    color1              = Color4{1.f, 1.f, 1.f, 1.f};
    color2              = Color4{1.f, 1.f, 1.f, 1.f};
    colorDead           = Color4{0.f, 0.f, 0.f, 1.f};
    textureMask         = Color4{1.f, 1.f, 1.f, 1.f};
    emitRate            = 100;
    gravity             = Vector3::Zero();
    minEmitPower        = 1.f;
    maxEmitPower        = 1.f;
    particleEmitterType = nullptr;
  }

  _isAnimationSheetEnabled = isAnimationSheetEnabled;
  if (isAnimationSheetEnabled) {
    _vertexBufferSize = 12;
  }

  _createIndexBuffer();

  // 11 floats per particle (x, y, z, r, g, b, a, angle, size, offsetX, offsetY)
  // + 1 filler
  _vertexData.resize(capacity * _vertexBufferSize * 4);
  _vertexBuffer = ::std::make_unique<Buffer>(scene->getEngine(), _vertexData,
                                             true, _vertexBufferSize);

  auto positions
    = _vertexBuffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 3);
  auto colors
    = _vertexBuffer->createVertexBuffer(VertexBuffer::ColorKind, 3, 4);
  auto options
    = _vertexBuffer->createVertexBuffer(VertexBuffer::OptionsKind, 7, 4);

  if (_isAnimationSheetEnabled) {
    auto cellIndexBuffer
      = _vertexBuffer->createVertexBuffer(VertexBuffer::CellIndexKind, 11, 1);
    _vertexBuffers["cellIndex"] = ::std::move(cellIndexBuffer);
  }

  _vertexBuffers[VertexBuffer::PositionKindChars] = ::std::move(positions);
  _vertexBuffers[VertexBuffer::ColorKindChars]    = ::std::move(colors);
  _vertexBuffers["options"]                       = ::std::move(options);

  // Default emitter type
  particleEmitterType = ::std::make_unique<BoxParticleEmitter>();

  updateFunction = [this](vector_t<Particle*>& _particles) {
    for (unsigned int pIndex = 0; pIndex < _particles.size(); ++pIndex) {
      auto particle = _particles[pIndex];
      particle->age += static_cast<float>(_scaledUpdateSpeed);

      if (particle->age >= particle->lifeTime) {
        // Recycle by swapping with last particle
        _emitFromParticle(particle);
        recycleParticle(particle);
        --pIndex;
        continue;
      }
      else {
        particle->colorStep.scaleToRef(_scaledUpdateSpeed, _scaledColorStep);
        particle->color.addInPlace(_scaledColorStep);

        if (particle->color.a < 0.f) {
          particle->color.a = 0.f;
        }

        particle->angle
          += particle->angularSpeed * static_cast<float>(_scaledUpdateSpeed);

        particle->direction.scaleToRef(_scaledUpdateSpeed, _scaledDirection);
        particle->position.addInPlace(_scaledDirection);

        gravity.scaleToRef(_scaledUpdateSpeed, _scaledGravity);
        particle->direction.addInPlace(_scaledGravity);

        if (_isAnimationSheetEnabled) {
          particle->updateCellIndex(_scaledUpdateSpeed);
        }
      }
    }
  };
}

ParticleSystem::~ParticleSystem()
{
}

IReflect::Type ParticleSystem::type() const
{
  return IReflect::Type::PARTICLESYSTEM;
}

vector_t<Particle*>& ParticleSystem::particles()
{
  return _particles;
}

const char* ParticleSystem::getClassName() const
{
  return "ParticleSystem";
}

void ParticleSystem::setOnDispose(
  const ::std::function<void(ParticleSystem*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

bool ParticleSystem::isAnimationSheetEnabled() const
{
  return _isAnimationSheetEnabled;
}

Vector3& ParticleSystem::get_direction1()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->direction1;
  }

  return _zeroVector3;
}

void ParticleSystem::set_direction1(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->direction1 = value;
  }
}

Vector3& ParticleSystem::get_direction2()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->direction2;
  }

  return _zeroVector3;
}

void ParticleSystem::set_direction2(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->direction2 = value;
  }
}

Vector3& ParticleSystem::get_minEmitBox()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->minEmitBox;
  }

  return _zeroVector3;
}

void ParticleSystem::set_minEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->minEmitBox = value;
  }
}

Vector3& ParticleSystem::get_maxEmitBox()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->maxEmitBox;
  }

  return _zeroVector3;
}

void ParticleSystem::set_maxEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->maxEmitBox = value;
  }
}

void ParticleSystem::_createIndexBuffer()
{
  Uint32Array indices;
  int index = 0;
  for (size_t count = 0; count < _capacity; ++count) {
    indices.emplace_back(index);
    indices.emplace_back(index + 1);
    indices.emplace_back(index + 2);
    indices.emplace_back(index);
    indices.emplace_back(index + 2);
    indices.emplace_back(index + 3);
    index += 4;
  }

  _indexBuffer = _scene->getEngine()->createIndexBuffer(indices);
}

size_t ParticleSystem::getCapacity() const
{
  return _capacity;
}

bool ParticleSystem::isAlive() const
{
  return _alive;
}

bool ParticleSystem::isStarted() const
{
  return _started;
}

void ParticleSystem::start()
{
  _started     = true;
  _stopped     = false;
  _actualFrame = 0;
  if (!subEmitters.empty()) {
    activeSubSystems.clear();
  }
}

void ParticleSystem::stop()
{
  _stopped = true;
  _stopSubEmitters();
}

void ParticleSystem::stop(bool stopSubEmitters)
{
  _stopped = true;

  if (stopSubEmitters) {
    _stopSubEmitters();
  }
}

void ParticleSystem::reset()
{
  _stockParticles.clear();
  _particles.clear();
}

void ParticleSystem::_appendParticleVertex(unsigned int index,
                                           Particle* particle, int offsetX,
                                           int offsetY)
{
  unsigned int offset      = index * _vertexBufferSize;
  _vertexData[offset]      = particle->position.x;
  _vertexData[offset + 1]  = particle->position.y;
  _vertexData[offset + 2]  = particle->position.z;
  _vertexData[offset + 3]  = particle->color.r;
  _vertexData[offset + 4]  = particle->color.g;
  _vertexData[offset + 5]  = particle->color.b;
  _vertexData[offset + 6]  = particle->color.a;
  _vertexData[offset + 7]  = particle->angle;
  _vertexData[offset + 8]  = particle->size;
  _vertexData[offset + 9]  = static_cast<float>(offsetX);
  _vertexData[offset + 10] = static_cast<float>(offsetY);
}

void ParticleSystem::_appendParticleVertexWithAnimation(unsigned int index,
                                                        Particle* particle,
                                                        int offsetX,
                                                        int offsetY)
{
  auto _offsetX = static_cast<float>(offsetX);
  auto _offsetY = static_cast<float>(offsetY);
  if (offsetX == 0) {
    _offsetX = _epsilon;
  }
  else if (offsetX == 1) {
    _offsetX = 1.f - _epsilon;
  }

  if (offsetY == 0) {
    _offsetY = _epsilon;
  }
  else if (offsetY == 1) {
    _offsetY = 1.f - _epsilon;
  }

  auto offset              = index * _vertexBufferSize;
  _vertexData[offset]      = particle->position.x;
  _vertexData[offset + 1]  = particle->position.y;
  _vertexData[offset + 2]  = particle->position.z;
  _vertexData[offset + 3]  = particle->color.r;
  _vertexData[offset + 4]  = particle->color.g;
  _vertexData[offset + 5]  = particle->color.b;
  _vertexData[offset + 6]  = particle->color.a;
  _vertexData[offset + 7]  = particle->angle;
  _vertexData[offset + 8]  = particle->size;
  _vertexData[offset + 9]  = _offsetX;
  _vertexData[offset + 10] = _offsetY;
  _vertexData[offset + 11] = particle->cellIndex;
}

void ParticleSystem::recycleParticle(Particle* particle)
{
  auto lastParticle = _particles.back();
  _particles.pop_back();

  if (lastParticle != particle) {
    lastParticle->copyTo(*particle);
  }
  _stockParticles.emplace_back(lastParticle);
}

void ParticleSystem::_stopSubEmitters()
{
  if (activeSubSystems.empty()) {
    return;
  }
  for (auto& subSystem : activeSubSystems) {
    subSystem->stop(true);
  }
  activeSubSystems.clear();
}

Particle* ParticleSystem::_createParticle()
{
  Particle* particle;
  if (!_stockParticles.empty()) {
    particle = _stockParticles.back();
    _stockParticles.pop_back();
    particle->age       = 0;
    particle->cellIndex = startSpriteCellID;
  }
  else {
    particle = new Particle(this);
  }
  return particle;
}

void ParticleSystem::_removeFromRoot()
{
  if (!_rootParticleSystem) {
    return;
  }

  _rootParticleSystem->activeSubSystems.erase(
    ::std::remove(_rootParticleSystem->activeSubSystems.begin(),
                  _rootParticleSystem->activeSubSystems.end(), this),
    _rootParticleSystem->activeSubSystems.end());
}

void ParticleSystem::_emitFromParticle(Particle* /*particle*/)
{
  if (subEmitters.empty()) {
    return;
  }
#if 0
  auto templateIndex
    = static_cast<size_t>(::std::floor(Math::random() * subEmitters.size()));

  auto subSystem
    = subEmitters[templateIndex]->clone(name + "_sub", particle->position);
  subSystem._rootParticleSystem = this;
  activeSubSystems.emplace_back(subSystem);
  subSystem.start();
#endif
}

void ParticleSystem::_update(int newParticles)
{
  // Update current
  _alive = !_particles.empty();

  updateFunction(_particles);

  // Add new ones
  Matrix worldMatrix;
  if (emitter.is<AbstractMesh*>()) {
    auto emitterMesh = emitter.get<AbstractMesh*>();
    worldMatrix      = *emitterMesh->getWorldMatrix();
  }
  else {
    auto emitterPosition = emitter.get<Vector3>();
    worldMatrix = Matrix::Translation(emitterPosition.x, emitterPosition.y,
                                      emitterPosition.z);
  }

  Particle* particle = nullptr;
  for (int index = 0; index < newParticles; ++index) {
    if (_particles.size() == _capacity) {
      break;
    }

    particle = _createParticle();

    _particles.emplace_back(particle);

    auto emitPower = Scalar::RandomRange(minEmitPower, maxEmitPower);

    if (startPositionFunction) {
      startPositionFunction(worldMatrix, particle->position, particle);
    }
    else {
      particleEmitterType->startPositionFunction(worldMatrix,
                                                 particle->position, particle);
    }

    if (startDirectionFunction) {
      startDirectionFunction(emitPower, worldMatrix, particle->direction,
                             particle);
    }
    else {
      particleEmitterType->startDirectionFunction(
        emitPower, worldMatrix, particle->direction, particle);
    }

    particle->lifeTime = Scalar::RandomRange(minLifeTime, maxLifeTime);

    particle->size = Scalar::RandomRange(minSize, maxSize);
    particle->angularSpeed
      = Scalar::RandomRange(minAngularSpeed, maxAngularSpeed);

    auto step = Scalar::RandomRange(0, 1.f);

    Color4::LerpToRef(color1, color2, step, particle->color);

    colorDead.subtractToRef(particle->color, _colorDiff);
    _colorDiff.scaleToRef(1.f / particle->lifeTime, particle->colorStep);
  }
}

Effect* ParticleSystem::_getEffect()
{
  if (_customEffect) {
    return _customEffect;
  };

  vector_t<string_t> defines;

  if (_scene->clipPlane()) {
    defines.emplace_back("#define CLIPPLANE");
  }

  if (_isAnimationSheetEnabled) {
    defines.emplace_back("#define ANIMATESHEET");
  }

  // Effect
  string_t joined = String::join(defines, '\n');
  if (_cachedDefines != joined) {
    _cachedDefines = joined;

    EffectCreationOptions options;
    if (_isAnimationSheetEnabled) {
      options.attributes
        = {VertexBuffer::PositionKindChars, VertexBuffer::ColorKindChars,
           "options", "cellIndex"};
      options.uniformsNames = {"invView",        "view",       "projection",
                               "particlesInfos", "vClipPlane", "textureMask"};
    }
    else {
      options.attributes = {VertexBuffer::PositionKindChars,
                            VertexBuffer::ColorKindChars, "options"};
      options.uniformsNames
        = {"invView", "view", "projection", "vClipPlane", "textureMask"};
    }

    options.samplers = {"diffuseSampler"};
    options.defines  = ::std::move(joined);

    _effect = _scene->getEngine()->createEffect("particles", options,
                                                _scene->getEngine());
  }

  return _effect;
}

void ParticleSystem::animate()
{
  if (!_started) {
    return;
  }

  auto effect = _getEffect();

  // Check
  if (!hasEmitter() || !effect->isReady() || !particleTexture
      || !particleTexture->isReady())
    return;

  if (_currentRenderId == _scene->getRenderId()) {
    return;
  }

  _currentRenderId = _scene->getRenderId();

  _scaledUpdateSpeed
    = static_cast<int>(updateSpeed * _scene->getAnimationRatio());

  // determine the number of particles we need to create
  int newParticles = 0;

  if (manualEmitCount > -1) {
    newParticles    = manualEmitCount;
    _newPartsExcess = 0;
    manualEmitCount = 0;
  }
  else {
    newParticles = ((emitRate * _scaledUpdateSpeed) >> 0);
    _newPartsExcess += emitRate * _scaledUpdateSpeed - newParticles;
  }

  if (_newPartsExcess > 1) {
    newParticles += _newPartsExcess >> 0;
    _newPartsExcess -= _newPartsExcess >> 0;
  }

  _alive = false;

  if (!_stopped) {
    _actualFrame += _scaledUpdateSpeed;

    if (targetStopDuration && _actualFrame >= targetStopDuration)
      stop();
  }
  else {
    newParticles = 0;
  }

  _update(newParticles);

  // Stopped?
  if (_stopped) {
    if (!_alive) {
      _started = false;
      if (onAnimationEnd) {
        onAnimationEnd();
      }
      if (disposeOnStop) {
        _scene->_toBeDisposed.emplace_back(this);
      }
    }
  }

  // Animation sheet
  if (_isAnimationSheetEnabled) {
    _appendParticleVertexes = [this](unsigned int offset, Particle* particle) {
      _appenedParticleVertexesWithSheet(offset, particle);
    };
  }
  else {
    _appendParticleVertexes = [this](unsigned int offset, Particle* particle) {
      _appenedParticleVertexesWithSheet(offset, particle);
    };
  }

  // Update VBO
  unsigned int offset = 0;
  for (auto& particle : _particles) {
    _appendParticleVertexes(offset, particle);
    offset += 4;
  }

  if (_vertexBuffer) {
    _vertexBuffer->update(_vertexData);
  }

  if (manualEmitCount == 0 && disposeOnStop) {
    stop();
  }
}

void ParticleSystem::_appenedParticleVertexesWithSheet(unsigned int offset,
                                                       Particle* particle)
{
  _appendParticleVertexWithAnimation(offset++, particle, 0, 0);
  _appendParticleVertexWithAnimation(offset++, particle, 1, 0);
  _appendParticleVertexWithAnimation(offset++, particle, 1, 1);
  _appendParticleVertexWithAnimation(offset++, particle, 0, 1);
}

void ParticleSystem::_appenedParticleVertexesNoSheet(unsigned int offset,
                                                     Particle* particle)
{
  _appendParticleVertex(offset++, particle, 0, 0);
  _appendParticleVertex(offset++, particle, 1, 0);
  _appendParticleVertex(offset++, particle, 1, 1);
  _appendParticleVertex(offset++, particle, 0, 1);
}

void ParticleSystem::rebuild()
{
  _createIndexBuffer();

  if (_vertexBuffer) {
    _vertexBuffer->_rebuild();
  }
}

bool ParticleSystem::isReady()
{
  auto effect = _getEffect();
  if (!hasEmitter() || !effect->isReady() || !particleTexture
      || !particleTexture->isReady()) {
    return false;
  }

  return true;
}

size_t ParticleSystem::render()
{
  auto effect = _getEffect();

  // Check
  if (!isReady() || _particles.empty()) {
    return 0;
  }

  auto engine = _scene->getEngine();

  // Render
  engine->enableEffect(effect);
  engine->setState(false);

  auto viewMatrix = _scene->getViewMatrix();
  effect->setTexture("diffuseSampler", particleTexture);
  effect->setMatrix("view", viewMatrix);
  effect->setMatrix("projection", _scene->getProjectionMatrix());

  if (_isAnimationSheetEnabled && particleTexture) {
    auto baseSize = particleTexture->getBaseSize();
    effect->setFloat3("particlesInfos",
                      spriteCellWidth / static_cast<float>(baseSize.width),
                      spriteCellHeight / static_cast<float>(baseSize.height),
                      static_cast<float>(baseSize.width) / spriteCellWidth);
  }

  effect->setFloat4("textureMask", textureMask.r, textureMask.g, textureMask.b,
                    textureMask.a);

  if (_scene->clipPlane()) {
    auto clipPlane = _scene->clipPlane();
    auto invView   = viewMatrix;
    invView.invert();
    effect->setMatrix("invView", invView);
    effect->setFloat4("vClipPlane", clipPlane->normal.x, clipPlane->normal.y,
                      clipPlane->normal.z, clipPlane->d);
  }

  // VBOs
  unordered_map_t<string_t, VertexBuffer*> vertexBuffersTmp;
  for (auto& item : _vertexBuffers) {
    vertexBuffersTmp[item.first] = item.second.get();
  }
  engine->bindBuffers(vertexBuffersTmp, _indexBuffer.get(), effect);

  // Draw order
  if (blendMode == ParticleSystem::BLENDMODE_ONEONE) {
    engine->setAlphaMode(EngineConstants::ALPHA_ONEONE);
  }
  else {
    engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
  }

  if (forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  engine->drawElementsType(Material::TriangleFillMode(), 0,
                           static_cast<int>(_particles.size() * 6));
  engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);

  return _particles.size();
}

void ParticleSystem::dispose(bool disposeTexture)
{
  if (_vertexBuffer) {
    _vertexBuffer->dispose();
    _vertexBuffer = nullptr;
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer = nullptr;
  }

  if (disposeTexture && particleTexture) {
    particleTexture->dispose();
    particleTexture = nullptr;
  }

  _removeFromRoot();

  // Remove from scene
  _scene->particleSystems.erase(
    ::std::remove_if(
      _scene->particleSystems.begin(), _scene->particleSystems.end(),
      [this](const unique_ptr_t<IParticleSystem>& particleSystem) {
        return particleSystem.get() == this;
      }),
    _scene->particleSystems.end());

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

vector_t<Animation*> ParticleSystem::getAnimations()
{
  return animations;
}

SphereParticleEmitter* ParticleSystem::createSphereEmitter(float radius)
{
  auto particleEmitter = ::std::make_unique<SphereParticleEmitter>(radius);
  particleEmitterType  = ::std::move(particleEmitter);
  return particleEmitter.get();
}

SphereDirectedParticleEmitter* ParticleSystem::createDirectedSphereEmitter(
  float radius, const Vector3& direction1, const Vector3& direction2)
{
  auto particleEmitter = ::std::make_unique<SphereDirectedParticleEmitter>(
    radius, direction1, direction2);
  particleEmitterType = ::std::move(particleEmitter);
  return particleEmitter.get();
}

ConeParticleEmitter* ParticleSystem::createConeEmitter(float radius,
                                                       float angle)
{
  auto particleEmitter = ::std::make_unique<ConeParticleEmitter>(radius, angle);
  particleEmitterType  = ::std::move(particleEmitter);
  return particleEmitter.get();
}

BoxParticleEmitter* ParticleSystem::createBoxEmitter(const Vector3& iDirection1,
                                                     const Vector3& iDirection2,
                                                     const Vector3& iMinEmitBox,
                                                     const Vector3& iMaxEmitBox)
{
  auto particleEmitter = ::std::make_unique<BoxParticleEmitter>();
  particleEmitterType  = ::std::move(particleEmitter);
  direction1           = iDirection1;
  direction2           = iDirection2;
  minEmitBox           = iMinEmitBox;
  maxEmitBox           = iMaxEmitBox;
  return particleEmitter.get();
}

IParticleSystem* ParticleSystem::clone(const string_t& /*iName*/,
                                       Mesh* /*newEmitter*/)
{
  // ParticleSystem* result = new ParticleSystem(_name, _capacity, _scene);

  // TODO FIXME
  // Tools.DeepCopy(this, result, ["particles"],
  //               [ "_vertexDeclaration", "_vertexStrideSize" ]);

  // if (newEmitter == nullptr) {
  //  newEmitter = emitter;
  //}

  /*result->emitter = newEmitter;
  if (particleTexture) {
    result->particleTexture = new Texture(particleTexture->url, _scene);
  }

  result->start();

  return result;*/
  return nullptr;
}

Json::object ParticleSystem::serialize() const
{
  return Json::object();
}

ParticleSystem* ParticleSystem::Parse(const Json::value& parsedParticleSystem,
                                      Scene* scene, const string_t& rootUrl)
{
  auto name           = Json::GetString(parsedParticleSystem, "name");
  auto particleSystem = new ParticleSystem(
    name, Json::GetNumber(parsedParticleSystem, "capacity", 0ul), scene);

  if (parsedParticleSystem.contains("id")) {
    particleSystem->id = Json::GetString(parsedParticleSystem, "id");
  }

  // Auto start
  if (parsedParticleSystem.contains("preventAutoStart")) {
    particleSystem->preventAutoStart
      = Json::GetBool(parsedParticleSystem, "preventAutoStart");
  }

  // Texture
  if (parsedParticleSystem.contains("textureName")) {
    particleSystem->particleTexture = Texture::New(
      rootUrl + Json::GetString(parsedParticleSystem, "textureName"), scene);
    particleSystem->particleTexture->name
      = Json::GetString(parsedParticleSystem, "textureName");
  }

  // Emitter
  if (parsedParticleSystem.contains("emitterId")) {
    particleSystem->emitter.set<AbstractMesh*>(scene->getLastMeshByID(
      Json::GetString(parsedParticleSystem, "emitterId")));
  }
  else {
    particleSystem->emitter.set<Vector3>(Vector3::FromArray(
      Json::ToArray<float>(parsedParticleSystem, "emitter")));
  }

  // Animations
  if (parsedParticleSystem.contains("animations")) {
    for (auto& parsedAnimation :
         Json::GetArray(parsedParticleSystem, "animations"))
      particleSystem->animations.emplace_back(
        Animation::Parse(parsedAnimation));
  }

  if (parsedParticleSystem.contains("autoAnimate")) {
    scene->beginAnimation(
      particleSystem,
      Json::GetNumber(parsedParticleSystem, "autoAnimateFrom", 0),
      Json::GetNumber(parsedParticleSystem, "autoAnimateTo", 0),
      Json::GetBool(parsedParticleSystem, "autoAnimateLoop"),
      Json::GetNumber(parsedParticleSystem, "autoAnimateSpeed", 1.f));
  }

  // Particle system
  particleSystem->minAngularSpeed
    = Json::GetNumber(parsedParticleSystem, "minAngularSpeed", 0.f);
  particleSystem->maxAngularSpeed
    = Json::GetNumber(parsedParticleSystem, "maxAngularSpeed", 0.f);
  particleSystem->minSize
    = Json::GetNumber(parsedParticleSystem, "minSize", 1.f);
  particleSystem->maxSize
    = Json::GetNumber(parsedParticleSystem, "maxSize", 1.f);
  particleSystem->minLifeTime
    = Json::GetNumber(parsedParticleSystem, "minLifeTime", 1.f);
  particleSystem->maxLifeTime
    = Json::GetNumber(parsedParticleSystem, "maxLifeTime", 1.f);
  particleSystem->minEmitPower
    = Json::GetNumber(parsedParticleSystem, "minEmitPower", 1.f);
  particleSystem->maxEmitPower
    = Json::GetNumber(parsedParticleSystem, "maxEmitPower", 1.f);
  particleSystem->emitRate
    = Json::GetNumber(parsedParticleSystem, "emitRate", 10);
  particleSystem->minEmitBox = Vector3::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "minEmitBox"));
  particleSystem->maxEmitBox = Vector3::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "maxEmitBox"));
  particleSystem->gravity
    = Vector3::FromArray(Json::ToArray<float>(parsedParticleSystem, "gravity"));
  particleSystem->direction1 = Vector3::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "direction1"));
  particleSystem->direction2 = Vector3::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "direction2"));
  particleSystem->color1
    = Color4::FromArray(Json::ToArray<float>(parsedParticleSystem, "color1"));
  particleSystem->color2
    = Color4::FromArray(Json::ToArray<float>(parsedParticleSystem, "color2"));
  particleSystem->colorDead = Color4::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "colorDead"));
  particleSystem->updateSpeed
    = Json::GetNumber(parsedParticleSystem, "updateSpeed", 0.01f);
  particleSystem->targetStopDuration
    = Json::GetNumber(parsedParticleSystem, "targetStopDuration", 0);
  particleSystem->textureMask = Color4::FromArray(
    Json::ToArray<float>(parsedParticleSystem, "textureMask"));
  particleSystem->blendMode = Json::GetNumber<unsigned>(
    parsedParticleSystem, "blendMode", ParticleSystem::BLENDMODE_ONEONE);

  particleSystem->startSpriteCellID
    = Json::GetNumber<unsigned>(parsedParticleSystem, "startSpriteCellID", 0u);
  particleSystem->endSpriteCellID
    = Json::GetNumber<unsigned>(parsedParticleSystem, "endSpriteCellID", 0u);
  particleSystem->spriteCellLoop
    = Json::GetBool(parsedParticleSystem, "spriteCellLoop", true);
  particleSystem->spriteCellChangeSpeed = Json::GetNumber<float>(
    parsedParticleSystem, "spriteCellChangeSpeed", 0.f);
  particleSystem->spriteCellWidth
    = Json::GetNumber<unsigned>(parsedParticleSystem, "spriteCellWidth", 0u);
  particleSystem->spriteCellHeight
    = Json::GetNumber<unsigned>(parsedParticleSystem, "spriteCellHeight", 0u);

  if (!particleSystem->preventAutoStart) {
    particleSystem->start();
  }

  return particleSystem;
}

} // end of namespace BABYLON
