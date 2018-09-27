#include <babylon/particles/particle_system.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/image_processing_configuration_defines.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/scalar.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/cone_particle_emitter.h>
#include <babylon/particles/emittertypes/hemispheric_particle_emitter.h>
#include <babylon/particles/emittertypes/point_particle_emitter.h>
#include <babylon/particles/emittertypes/sphere_directed_particle_emitter.h>
#include <babylon/particles/emittertypes/sphere_particle_emitter.h>
#include <babylon/particles/particle.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ParticleSystem::ParticleSystem(const std::string& iName, size_t capacity,
                               Scene* scene, Effect* customEffect,
                               bool isAnimationSheetEnabled, float epsilon)
    : BaseParticleSystem{iName}
    , onDispose{this, &ParticleSystem::set_onDispose}
    , _vertexBufferSize{11}
    , _newPartsExcess{0}
    , _scaledColorStep{Color4(0.f, 0.f, 0.f, 0.f)}
    , _colorDiff{Color4(0.f, 0.f, 0.f, 0.f)}
    , _scaledDirection{Vector3::Zero()}
    , _scaledGravity{Vector3::Zero()}
    , _currentRenderId{-1}
    , _useInstancing{false}
    , _started{false}
    , _stopped{false}
    , _actualFrame{0}
    , _appendParticleVertexes{nullptr}
    , _zeroVector3{Vector3::Zero()}
{
  _capacity = capacity;

  _epsilon                 = epsilon;
  _isAnimationSheetEnabled = isAnimationSheetEnabled;

  _scene = scene ? scene : Engine::LastCreatedScene();

  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

  _customEffect = customEffect;

  _scene->particleSystems.emplace_back(this);

  _useInstancing = _scene->getEngine()->getCaps().instancedArrays;

  _createIndexBuffer();
  _createVertexBuffers();

  // Default emitter type
  particleEmitterType = std::make_unique<BoxParticleEmitter>();

  updateFunction = [this](std::vector<Particle*>& _particles) {
    std::optional<Uint8Array> noiseTextureData;
    std::optional<ISize> noiseTextureSize = std::nullopt;

    if (noiseTexture) { // We need to get texture data back to CPU
      noiseTextureData = noiseTexture->readPixels().uint8Array;
      noiseTextureSize = noiseTexture->getSize();
    }

    for (unsigned int index = 0; index < _particles.size(); ++index) {
      auto particle = _particles[index];
      particle->age += static_cast<float>(_scaledUpdateSpeed);

      if (particle->age >= particle->lifeTime) {
        // Recycle by swapping with last particle
        _emitFromParticle(particle);
        recycleParticle(particle);
        --index;
        continue;
      }
      else {
        auto ratio = particle->age / particle->lifeTime;

        // Color
        if (!_colorGradients.empty()) {
          Tools::GetCurrentGradient<ColorGradient>(
            ratio, _colorGradients,
            [&](ColorGradient& currentGradient, ColorGradient& nextGradient,
                float scale) {
              if (currentGradient != *particle->_currentColorGradient) {
                particle->_currentColor1.copyFrom(particle->_currentColor2);
                static_cast<ColorGradient&>(nextGradient)
                  .getColorToRef(particle->_currentColor2);
                particle->_currentColorGradient
                  = static_cast<ColorGradient&>(currentGradient);
              }
              Color4::LerpToRef(particle->_currentColor1,
                                particle->_currentColor2, scale,
                                particle->color);
            });
        }
        else {
          particle->colorStep.scaleToRef(_scaledUpdateSpeed, _scaledColorStep);
          particle->color.addInPlace(_scaledColorStep);

          if (particle->color.a < 0.f) {
            particle->color.a = 0.f;
          }
        }

        // Angular speed
        if (!_angularSpeedGradients.empty()) {
          Tools::GetCurrentGradient<FactorGradient>(
            ratio, _angularSpeedGradients,
            [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
                float scale) {
              if (currentGradient != particle->_currentAngularSpeedGradient) {
                particle->_currentAngularSpeed1
                  = particle->_currentAngularSpeed2;
                particle->_currentAngularSpeed2 = nextGradient.getFactor();
                particle->_currentAngularSpeedGradient = currentGradient;
              }
              particle->angularSpeed
                = Scalar::Lerp(particle->_currentAngularSpeed1,
                               particle->_currentAngularSpeed2, scale);
            });
        }
        particle->angle += particle->angularSpeed * _scaledUpdateSpeed;

        // Direction
        auto directionScale = _scaledUpdateSpeed;

        // Velocity
        if (_velocityGradients.size() > 0) {
          Tools::GetCurrentGradient<FactorGradient>(
            ratio, _velocityGradients,
            [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
                float scale) {
              if (currentGradient != particle->_currentVelocityGradient) {
                particle->_currentVelocity1 = particle->_currentVelocity2;
                particle->_currentVelocity2 = nextGradient.getFactor();
                particle->_currentVelocityGradient = currentGradient;
              }
              directionScale
                *= Scalar::Lerp(particle->_currentVelocity1,
                                particle->_currentVelocity2, scale);
            });
        }
        particle->direction.scaleToRef(directionScale, _scaledDirection);

        // Limit velocity
        if (!_limitVelocityGradients.empty()) {
          Tools::GetCurrentGradient<FactorGradient>(
            ratio, _limitVelocityGradients,
            [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
                float scale) {
              if (currentGradient != particle->_currentLimitVelocityGradient) {
                particle->_currentLimitVelocity1
                  = particle->_currentLimitVelocity2;
                particle->_currentLimitVelocity2 = nextGradient.getFactor();
                particle->_currentLimitVelocityGradient = currentGradient;
              }

              auto limitVelocity
                = Scalar::Lerp(particle->_currentLimitVelocity1,
                               particle->_currentLimitVelocity2, scale);
              auto currentVelocity = particle->direction.length();

              if (currentVelocity > limitVelocity) {
                particle->direction.scaleInPlace(limitVelocityDamping);
              }
            });
        }

        /// Drag
        if (!_dragGradients.empty()) {
          Tools::GetCurrentGradient<FactorGradient>(
            ratio, _dragGradients,
            [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
                float scale) {
              if (currentGradient != particle->_currentDragGradient) {
                particle->_currentDrag1        = particle->_currentDrag2;
                particle->_currentDrag2        = nextGradient.getFactor();
                particle->_currentDragGradient = currentGradient;
              }

              auto drag = Scalar::Lerp(particle->_currentDrag1,
                                       particle->_currentDrag2, scale);

              _scaledDirection.scaleInPlace(drag);
            });
        }

        particle->position.addInPlace(_scaledDirection);

        // Noise
        if (noiseTextureData && noiseTextureSize) {
          auto& localPosition   = Tmp::Vector3Array[0];
          auto& emitterPosition = Tmp::Vector3Array[1];

          _emitterWorldMatrix.getTranslationToRef(emitterPosition);
          particle->position.subtractToRef(emitterPosition, localPosition);

          auto fetchedColorR
            = _fetchR(localPosition.y, localPosition.z, noiseTextureSize->width,
                      noiseTextureSize->height, *noiseTextureData);
          auto fetchedColorG
            = _fetchR(localPosition.x + 0.33f, localPosition.z + 0.33f,
                      noiseTextureSize->width, noiseTextureSize->height,
                      *noiseTextureData);
          auto fetchedColorB
            = _fetchR(localPosition.x - 0.3f, localPosition.y - 0.33f,
                      noiseTextureSize->width, noiseTextureSize->height,
                      *noiseTextureData);

          auto& force       = Tmp::Vector3Array[0];
          auto& scaledForce = Tmp::Vector3Array[1];

          force.copyFromFloats((2.f * fetchedColorR - 1.f) * noiseStrength.x,
                               (2.f * fetchedColorG - 1.f) * noiseStrength.y,
                               (2.f * fetchedColorB - 1.f) * noiseStrength.z);

          force.scaleToRef(_scaledUpdateSpeed, scaledForce);
          particle->direction.addInPlace(scaledForce);
        }

        // Gravity
        gravity.scaleToRef(_scaledUpdateSpeed, _scaledGravity);
        particle->direction.addInPlace(_scaledGravity);

        // Size
        if (!_sizeGradients.empty()) {
          Tools::GetCurrentGradient<FactorGradient>(
            ratio, _sizeGradients,
            [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
                float scale) {
              if (currentGradient != particle->_currentSizeGradient) {
                particle->_currentSize1        = particle->_currentSize2;
                particle->_currentSize2        = nextGradient.getFactor();
                particle->_currentSizeGradient = currentGradient;
              }
              particle->size = Scalar::Lerp(particle->_currentSize1,
                                            particle->_currentSize2, scale);
            });
        }

        if (_isAnimationSheetEnabled) {
          particle->updateCellIndex();
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

const char* ParticleSystem::getClassName() const
{
  return "ParticleSystem";
}

Scene* ParticleSystem::getScene() const
{
  return _scene;
}

std::vector<Particle*>& ParticleSystem::particles()
{
  return _particles;
}

void ParticleSystem::set_onDispose(
  const std::function<void(ParticleSystem*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void ParticleSystem::_addFactorGradient(
  std::vector<FactorGradient>& factorGradients, float gradient, float factor,
  const std::optional<float>& factor2)
{
  FactorGradient newGradient;
  newGradient.gradient = gradient;
  newGradient.factor1  = factor;
  newGradient.factor2  = factor2;
  factorGradients.emplace_back(newGradient);

  std::sort(factorGradients.begin(), factorGradients.end(),
            [](const FactorGradient& a, const FactorGradient& b) {
              if (a.gradient < b.gradient) {
                return -1;
              }
              else if (a.gradient > b.gradient) {
                return 1;
              }

              return 0;
            });
}

void ParticleSystem::_removeFactorGradient(
  std::vector<FactorGradient>& factorGradients, float gradient)
{
  if (factorGradients.empty()) {
    return;
  }

  factorGradients.erase(
    std::remove_if(factorGradients.begin(), factorGradients.end(),
                   [&gradient](const FactorGradient& factorGradient) {
                     return stl_util::almost_equal(factorGradient.gradient,
                                                   gradient);
                   }),
    factorGradients.end());
}

IParticleSystem&
ParticleSystem::addLifeTimeGradient(float gradient, float factor,
                                    const std::optional<float>& factor2)
{
  _addFactorGradient(_lifeTimeGradients, gradient, factor, factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeLifeTimeGradient(float gradient)
{
  _removeFactorGradient(_lifeTimeGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addSizeGradient(float gradient, float factor,
                                const std::optional<float>& factor2)
{
  _addFactorGradient(_sizeGradients, gradient, factor, factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeSizeGradient(float gradient)
{
  _removeFactorGradient(_sizeGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addAngularSpeedGradient(float gradient, float factor,
                                        const std::optional<float>& factor2)
{
  _addFactorGradient(_angularSpeedGradients, gradient, factor, factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeAngularSpeedGradient(float gradient)
{
  _removeFactorGradient(_angularSpeedGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addVelocityGradient(float gradient, float factor,
                                    const std::optional<float>& factor2)
{
  _addFactorGradient(_velocityGradients, gradient, factor, factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeVelocityGradient(float gradient)
{
  _removeFactorGradient(_velocityGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addLimitVelocityGradient(float gradient, float factor,
                                         const std::optional<float>& factor2)
{
  _addFactorGradient(_limitVelocityGradients, gradient, factor, *factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeLimitVelocityGradient(float gradient)
{
  _removeFactorGradient(_limitVelocityGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addDragGradient(float gradient, float factor,
                                const std::optional<float>& factor2)
{
  _addFactorGradient(_dragGradients, gradient, factor, factor2);

  return *this;
}

IParticleSystem& ParticleSystem::removeDragGradient(float gradient)
{
  _removeFactorGradient(_dragGradients, gradient);

  return *this;
}

IParticleSystem&
ParticleSystem::addColorGradient(float gradient, const Color4& color,
                                 const std::optional<Color4>& color2)
{
  ColorGradient colorGradient;
  colorGradient.gradient = gradient;
  colorGradient.color1   = color;
  colorGradient.color2   = color2;
  _colorGradients.emplace_back(colorGradient);

  std::sort(_colorGradients.begin(), _colorGradients.end(),
            [](const ColorGradient& a, const ColorGradient& b) {
              if (a.gradient < b.gradient) {
                return -1;
              }
              else if (a.gradient > b.gradient) {
                return 1;
              }

              return 0;
            });

  return *this;
}

IParticleSystem& ParticleSystem::removeColorGradient(float gradient)
{
  if (_colorGradients.empty()) {
    return *this;
  }

  _colorGradients.erase(
    std::remove_if(_colorGradients.begin(), _colorGradients.end(),
                   [&gradient](const ColorGradient& colorGradient) {
                     return stl_util::almost_equal(colorGradient.gradient,
                                                   gradient);
                   }),
    _colorGradients.end());

  return *this;
}

float ParticleSystem::_fetchR(float u, float v, float width, float height,
                              const Uint8Array& pixels)
{
  u = std::abs(u) * 0.5f + 0.5f;
  v = std::abs(v) * 0.5f + 0.5f;

  auto wrappedU = std::fmod((u * width), width);
  auto wrappedV = std::fmod((v * height), height);

  auto position = static_cast<size_t>((wrappedU + wrappedV * width) * 4);
  return static_cast<float>(pixels[position]) / 255.f;
}

void ParticleSystem::_reset()
{
}

void ParticleSystem::_resetEffect()
{
  if (_vertexBuffer) {
    _vertexBuffer->dispose();
    _vertexBuffer = nullptr;
  }

  if (_spriteBuffer) {
    _spriteBuffer->dispose();
    _spriteBuffer = nullptr;
  }

  _createVertexBuffers();
}

void ParticleSystem::_createVertexBuffers()
{
  _vertexBufferSize = _useInstancing ? 10 : 12;
  if (_isAnimationSheetEnabled) {
    _vertexBufferSize += 1;
  }

  if (!_isBillboardBased) {
    _vertexBufferSize += 3;
  }

  auto engine = _scene->getEngine();
  _vertexData
    = Float32Array(_capacity * _vertexBufferSize * (_useInstancing ? 1 : 4));
  _vertexBuffer
    = std::make_unique<Buffer>(engine, _vertexData, true, _vertexBufferSize);

  size_t dataOffset = 0;
  auto positions
    = _vertexBuffer->createVertexBuffer(VertexBuffer::PositionKind, dataOffset,
                                        3, _vertexBufferSize, _useInstancing);
  _vertexBuffers[VertexBuffer::PositionKindChars] = std::move(positions);
  dataOffset += 3;

  auto colors = _vertexBuffer->createVertexBuffer(
    VertexBuffer::ColorKind, dataOffset, 4, _vertexBufferSize, _useInstancing);
  _vertexBuffers[VertexBuffer::ColorKindChars] = std::move(colors);
  dataOffset += 4;

  auto options = _vertexBuffer->createVertexBuffer(
    VertexBuffer::AngleKind, dataOffset, 1, _vertexBufferSize, _useInstancing);
  _vertexBuffers[VertexBuffer::AngleKindChars] = std::move(options);
  dataOffset += 1;

  auto size = _vertexBuffer->createVertexBuffer(
    VertexBuffer::SizeKind, dataOffset, 2, _vertexBufferSize, _useInstancing);
  _vertexBuffers[VertexBuffer::SizeKindChars] = std::move(size);
  dataOffset += 2;

  if (_isAnimationSheetEnabled) {
    auto cellIndexBuffer = _vertexBuffer->createVertexBuffer(
      VertexBuffer::CellIndexKind, dataOffset, 1, _vertexBufferSize,
      _useInstancing);
    _vertexBuffers[VertexBuffer::CellIndexKindChars]
      = std::move(cellIndexBuffer);
    dataOffset += 1;
  }

  if (!_isBillboardBased) {
    auto directionBuffer = _vertexBuffer->createVertexBuffer(
      VertexBuffer::DirectionKind, dataOffset, 3, _vertexBufferSize,
      _useInstancing);
    _vertexBuffers[VertexBuffer::DirectionKindChars]
      = std::move(directionBuffer);
    dataOffset += 3;
  }

  std::unique_ptr<VertexBuffer> offsets = nullptr;
  if (_useInstancing) {
    Float32Array spriteData{0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};
    _spriteBuffer = std::make_unique<Buffer>(engine, spriteData, false, 2);
    offsets = _spriteBuffer->createVertexBuffer(VertexBuffer::OffsetKind, 0, 2);
  }
  else {
    offsets
      = _vertexBuffer->createVertexBuffer(VertexBuffer::OffsetKind, dataOffset,
                                          2, _vertexBufferSize, _useInstancing);
    dataOffset += 2;
  }
  _vertexBuffers[VertexBuffer::OffsetKindChars] = std::move(offsets);
}

void ParticleSystem::_createIndexBuffer()
{
  if (_useInstancing) {
    return;
  }
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

void ParticleSystem::start(size_t delay)
{
  if (delay > 0) {
    // Timeout
  }

  _started     = true;
  _stopped     = false;
  _actualFrame = 0;
  if (!subEmitters.empty()) {
    activeSubSystems.clear();
  }

  if (preWarmCycles) {
    for (size_t index = 0; index < preWarmCycles; ++index) {
      animate(true);
    }
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
  unsigned int offset = index * _vertexBufferSize;

  _vertexData[offset++] = particle->position.x;
  _vertexData[offset++] = particle->position.y;
  _vertexData[offset++] = particle->position.z;
  _vertexData[offset++] = particle->color.r;
  _vertexData[offset++] = particle->color.g;
  _vertexData[offset++] = particle->color.b;
  _vertexData[offset++] = particle->color.a;
  _vertexData[offset++] = particle->angle;

  _vertexData[offset++] = particle->scale.x * particle->size;
  _vertexData[offset++] = particle->scale.y * particle->size;

  if (_isAnimationSheetEnabled) {
    _vertexData[offset++] = particle->cellIndex;
  }

  if (!_isBillboardBased) {
    if (particle->_initialDirection) {
      auto& particleInitialDirection = *particle->_initialDirection;
      _vertexData[offset++]          = particleInitialDirection.x;
      _vertexData[offset++]          = particleInitialDirection.y;
      _vertexData[offset++]          = particleInitialDirection.z;
    }
    else {
      _vertexData[offset++] = particle->direction.x;
      _vertexData[offset++] = particle->direction.y;
      _vertexData[offset++] = particle->direction.z;
    }
  }

  if (!_useInstancing) {
    auto _offsetX = static_cast<float>(offsetX);
    auto _offsetY = static_cast<float>(offsetY);

    if (_isAnimationSheetEnabled) {
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
    }

    _vertexData[offset++] = _offsetX;
    _vertexData[offset++] = _offsetY;
  }
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
    particle->age                   = 0;
    particle->_currentColorGradient = std::nullopt;
    particle->cellIndex             = startSpriteCellID;
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
    std::remove(_rootParticleSystem->activeSubSystems.begin(),
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
    = static_cast<size_t>(std::floor(Math::random() * subEmitters.size()));

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

  if (emitter.is<AbstractMeshPtr>()) {
    auto emitterMesh    = emitter.get<AbstractMeshPtr>();
    _emitterWorldMatrix = *emitterMesh->getWorldMatrix();
  }
  else {
    auto emitterPosition = emitter.get<Vector3>();
    _emitterWorldMatrix  = Matrix::Translation(
      emitterPosition.x, emitterPosition.y, emitterPosition.z);
  }

  updateFunction(_particles);

  // Add new ones
  Particle* particle = nullptr;
  for (int index = 0; index < newParticles; ++index) {
    if (_particles.size() == _capacity) {
      break;
    }

    particle = _createParticle();

    _particles.emplace_back(particle);

    // Emitter
    auto emitPower = Scalar::RandomRange(minEmitPower, maxEmitPower);

    if (startPositionFunction) {
      startPositionFunction(_emitterWorldMatrix, particle->position, particle);
    }
    else {
      particleEmitterType->startPositionFunction(_emitterWorldMatrix,
                                                 particle->position, particle);
    }

    if (startDirectionFunction) {
      startDirectionFunction(_emitterWorldMatrix, particle->direction,
                             particle);
    }
    else {
      particleEmitterType->startDirectionFunction(
        _emitterWorldMatrix, particle->direction, particle);
    }

    if (emitPower == 0.f) {
      if (!particle->_initialDirection) {
        particle->_initialDirection = particle->direction;
      }
      else {
        particle->_initialDirection = particle->direction;
      }
    }
    else {
      particle->_initialDirection = std::nullopt;
    }

    particle->direction.scaleInPlace(emitPower);

    // Life time
    if (targetStopDuration && !_lifeTimeGradients.empty()) {
      auto ratio = Scalar::Clamp(_actualFrame / targetStopDuration);
      Tools::GetCurrentGradient<FactorGradient>(
        ratio, _lifeTimeGradients,
        [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
            float /*scale*/) {
          auto& factorGradient1 = currentGradient;
          auto& factorGradient2 = nextGradient;
          auto lifeTime1        = factorGradient1.getFactor();
          auto lifeTime2        = factorGradient2.getFactor();
          auto gradient
            = (ratio - factorGradient1.gradient)
              / (factorGradient2.gradient - factorGradient1.gradient);
          particle->lifeTime = Scalar::Lerp(lifeTime1, lifeTime2, gradient);
        });
    }
    else {
      particle->lifeTime = Scalar::RandomRange(minLifeTime, maxLifeTime);
    }

    // Size
    if (!_sizeGradients.empty()) {
      particle->size = Scalar::RandomRange(minSize, maxSize);
    }
    else {
      particle->_currentSizeGradient = _sizeGradients[0];
      particle->_currentSize1 = (*particle->_currentSizeGradient).getFactor();
      particle->size          = particle->_currentSize1;

      if (_sizeGradients.size() > 1) {
        particle->_currentSize2 = _sizeGradients[1].getFactor();
      }
      else {
        particle->_currentSize2 = particle->_currentSize1;
      }
    }
    // Size and scale
    particle->scale.copyFromFloats(Scalar::RandomRange(minScaleX, maxScaleX),
                                   Scalar::RandomRange(minScaleY, maxScaleY));

    // Angle
    if (!_angularSpeedGradients.empty()) {
      particle->angularSpeed
        = Scalar::RandomRange(minAngularSpeed, maxAngularSpeed);
    }
    else {
      particle->_currentAngularSpeedGradient = _angularSpeedGradients[0];
      particle->angularSpeed
        = (*particle->_currentAngularSpeedGradient).getFactor();
      particle->_currentAngularSpeed1 = particle->angularSpeed;

      if (_angularSpeedGradients.size() > 1) {
        particle->_currentAngularSpeed2 = _angularSpeedGradients[1].getFactor();
      }
      else {
        particle->_currentAngularSpeed2 = particle->_currentAngularSpeed1;
      }
    }
    particle->angle
      = Scalar::RandomRange(minInitialRotation, maxInitialRotation);

    // Velocity
    if (!_velocityGradients.empty()) {
      particle->_currentVelocityGradient = _velocityGradients[0];
      particle->_currentVelocity1
        = (*particle->_currentVelocityGradient).getFactor();

      if (_velocityGradients.size() > 1) {
        particle->_currentVelocity2 = _velocityGradients[1].getFactor();
      }
      else {
        particle->_currentVelocity2 = particle->_currentVelocity1;
      }
    }

    // Limit velocity
    if (!_limitVelocityGradients.empty()) {
      particle->_currentLimitVelocityGradient = _limitVelocityGradients[0];
      particle->_currentLimitVelocity1
        = particle->_currentLimitVelocityGradient->getFactor();

      if (_limitVelocityGradients.size() > 1) {
        particle->_currentLimitVelocity2
          = _limitVelocityGradients[1].getFactor();
      }
      else {
        particle->_currentLimitVelocity2 = particle->_currentLimitVelocity1;
      }
    }

    // Drag
    if (_dragGradients.empty()) {
      particle->_currentDragGradient = _dragGradients[0];
      particle->_currentDrag1 = particle->_currentDragGradient->getFactor();

      if (_dragGradients.size() > 1) {
        particle->_currentDrag2 = _dragGradients[1].getFactor();
      }
      else {
        particle->_currentDrag2 = particle->_currentDrag1;
      }
    }

    // Color
    if (_colorGradients.empty()) {
      auto step = Scalar::RandomRange(0.f, 1.f);

      Color4::LerpToRef(color1, color2, step, particle->color);

      colorDead.subtractToRef(particle->color, _colorDiff);
      _colorDiff.scaleToRef(1.f / particle->lifeTime, particle->colorStep);
    }
    else {
      auto currentColorGradient = _colorGradients[0];
      currentColorGradient.getColorToRef(particle->color);
      particle->_currentColorGradient = currentColorGradient;
      particle->_currentColor1.copyFrom(particle->color);

      if (_colorGradients.size() > 1) {
        _colorGradients[1].getColorToRef(particle->_currentColor2);
      }
      else {
        particle->_currentColor2.copyFrom(particle->color);
      }
    }

    // Sheet
    if (_isAnimationSheetEnabled) {
      particle->_initialStartSpriteCellID = startSpriteCellID;
      particle->_initialEndSpriteCellID   = endSpriteCellID;
    }
  }
}

std::vector<std::string>
ParticleSystem::_GetAttributeNamesOrOptions(bool isAnimationSheetEnabled,
                                            bool isBillboardBased)
{
  std::vector<std::string> attributeNamesOrOptions{VertexBuffer::PositionKindChars,
                                             VertexBuffer::ColorKindChars,
                                             "angle", "offset", "size"};

  if (isAnimationSheetEnabled) {
    attributeNamesOrOptions.emplace_back("cellIndex");
  }

  if (!isBillboardBased) {
    attributeNamesOrOptions.emplace_back("direction");
  }

  return attributeNamesOrOptions;
}

std::vector<std::string>
ParticleSystem::_GetEffectCreationOptions(bool isAnimationSheetEnabled)
{
  std::vector<std::string> effectCreationOption{
    "invView",          "view",        "projection",  "vClipPlane",
    "vClipPlane2",      "vClipPlane3", "vClipPlane4", "textureMask",
    "translationPivot", "eyePosition"};

  if (isAnimationSheetEnabled) {
    effectCreationOption.emplace_back("particlesInfos");
  }

  return effectCreationOption;
}

Effect* ParticleSystem::_getEffect()
{
  if (_customEffect) {
    return _customEffect;
  };

  std::vector<std::string> defines;

  if (_scene->clipPlane.has_value()) {
    defines.emplace_back("#define CLIPPLANE");
  }

  if (_scene->clipPlane2.has_value()) {
    defines.emplace_back("#define CLIPPLANE2");
  }

  if (_scene->clipPlane3.has_value()) {
    defines.emplace_back("#define CLIPPLANE3");
  }

  if (_scene->clipPlane4.has_value()) {
    defines.emplace_back("#define CLIPPLANE4");
  }

  if (_isAnimationSheetEnabled) {
    defines.emplace_back("#define ANIMATESHEET");
  }

  if (_isBillboardBased) {
    defines.emplace_back("#define BILLBOARD");

    switch (billboardMode) {
      case AbstractMesh::BILLBOARDMODE_Y:
        defines.emplace_back("#define BILLBOARDY");
        break;
      case AbstractMesh::BILLBOARDMODE_ALL:
      default:
        break;
    }
  }

  if (_imageProcessingConfiguration) {
    _imageProcessingConfiguration->prepareDefines(
      *_imageProcessingConfigurationDefines);
    defines.emplace_back(_imageProcessingConfigurationDefines->toString());
  }

  // Effect
  std::string join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    auto attributesNamesOrOptions = ParticleSystem::_GetAttributeNamesOrOptions(
      _isAnimationSheetEnabled, _isBillboardBased);
    auto effectCreationOption
      = ParticleSystem::_GetEffectCreationOptions(_isAnimationSheetEnabled);

    std::vector<std::string> samplers{"diffuseSampler"};

    // if (ImageProcessingConfiguration)
    {
      ImageProcessingConfiguration::PrepareUniforms(
        effectCreationOption, *_imageProcessingConfigurationDefines);
      ImageProcessingConfiguration::PrepareSamplers(
        samplers, *_imageProcessingConfigurationDefines);
    }

    EffectCreationOptions options;
    options.attributes    = std::move(attributesNamesOrOptions);
    options.uniformsNames = std::move(effectCreationOption);
    options.samplers      = std::move(samplers);
    options.defines       = std::move(join);

    _effect = _scene->getEngine()->createEffect("particles", options,
                                                _scene->getEngine());
  }

  return _effect;
}

void ParticleSystem::animate(bool preWarmOnly)
{
  if (!_started) {
    return;
  }

  if (!preWarmOnly) {
    auto effect = _getEffect();

    // Check
    if (!hasEmitter() || !_imageProcessingConfiguration->isReady()
        || !effect->isReady() || !particleTexture
        || !particleTexture->isReady())
      return;

    if (_currentRenderId == _scene->getRenderId()) {
      return;
    }
    _currentRenderId = _scene->getRenderId();
  }

  _scaledUpdateSpeed = static_cast<int>(
    updateSpeed
    * (preWarmOnly ? preWarmStepOffset : _scene->getAnimationRatio()));

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

  if (!preWarmOnly) {
    // Update VBO
    unsigned int offset = 0;
    for (auto& particle : _particles) {
      _appendParticleVertices(offset, particle);
      offset += _useInstancing ? 1 : 4;
    }

    if (_vertexBuffer) {
      _vertexBuffer->update(_vertexData);
    }

    if (manualEmitCount == 0 && disposeOnStop) {
      stop();
    }
  }
}

void ParticleSystem::_appendParticleVertices(unsigned int offset,
                                             Particle* particle)
{
  _appendParticleVertex(offset++, particle, 0, 0);
  if (!_useInstancing) {
    _appendParticleVertex(offset++, particle, 1, 0);
    _appendParticleVertex(offset++, particle, 1, 1);
    _appendParticleVertex(offset++, particle, 0, 1);
  }
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
  if (!hasEmitter() || !_imageProcessingConfiguration->isReady()
      || !effect->isReady() || !particleTexture
      || !particleTexture->isReady()) {
    return false;
  }

  return true;
}

size_t ParticleSystem::render(bool /*preWarm*/)
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

  effect->setVector2("translationPivot", translationPivot);
  effect->setFloat4("textureMask", textureMask.r, textureMask.g, textureMask.b,
                    textureMask.a);

  if (_isBillboardBased) {
    auto camera = _scene->activeCamera;
    effect->setVector3("eyePosition", camera->globalPosition);
  }

  if (_scene->clipPlane.has_value() || _scene->clipPlane2.has_value()
      || _scene->clipPlane3.has_value() || _scene->clipPlane4.has_value()) {
    auto invView = viewMatrix;
    invView.invert();
    effect->setMatrix("invView", invView);

    MaterialHelper::BindClipPlane(effect, _scene);
  }

  // VBOs
  std::unordered_map<std::string, VertexBuffer*> vertexBuffersTmp;
  for (auto& item : _vertexBuffers) {
    vertexBuffersTmp[item.first] = item.second.get();
  }
  engine->bindBuffers(vertexBuffersTmp, _indexBuffer.get(), effect);

  // image processing
  if (_imageProcessingConfiguration
      && !_imageProcessingConfiguration->applyByPostProcess) {
    _imageProcessingConfiguration->bind(effect);
  }

  // Draw order
  if (blendMode == ParticleSystem::BLENDMODE_ADD) {
    engine->setAlphaMode(EngineConstants::ALPHA_ADD);
  }
  else if (blendMode == ParticleSystem::BLENDMODE_ONEONE) {
    engine->setAlphaMode(EngineConstants::ALPHA_ONEONE);
  }
  else if (blendMode == ParticleSystem::BLENDMODE_STANDARD) {
    engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
  }

  if (forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  if (_useInstancing) {
    engine->drawArraysType(Material::TriangleFanDrawMode(), 0, 4,
                           static_cast<int>(_particles.size()));
    engine->unbindInstanceAttributes();
  }
  else {
    engine->drawElementsType(Material::TriangleFillMode(), 0,
                             static_cast<int>(_particles.size() * 6));
  }
  engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);

  return _particles.size();
}

void ParticleSystem::dispose(bool disposeTexture,
                             bool /*disposeMaterialAndTextures*/)
{
  if (_vertexBuffer) {
    _vertexBuffer->dispose();
    _vertexBuffer = nullptr;
  }

  if (_spriteBuffer) {
    _spriteBuffer->dispose();
    _spriteBuffer = nullptr;
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer = nullptr;
  }

  if (disposeTexture && particleTexture) {
    particleTexture->dispose();
    particleTexture = nullptr;
  }

  if (disposeTexture && noiseTexture) {
    noiseTexture->dispose();
    noiseTexture = nullptr;
  }

  _removeFromRoot();

  // Remove from scene
  _scene->particleSystems.erase(
    std::remove_if(_scene->particleSystems.begin(),
                   _scene->particleSystems.end(),
                   [this](const IParticleSystemPtr& particleSystem) {
                     return particleSystem.get() == this;
                   }),
    _scene->particleSystems.end());

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

std::vector<AnimationPtr> ParticleSystem::getAnimations()
{
  return animations;
}

IParticleSystem* ParticleSystem::clone(const std::string& /*iName*/,
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

void ParticleSystem::_Serialize(Json::object& /*serializationObject*/,
                                IParticleSystem* /*particleSystem*/)
{
}

ParticleSystem*
ParticleSystem::_Parse(const Json::value& /*parsedParticleSystem*/,
                       IParticleSystem* /*particleSystem*/, Scene* /*scene*/,
                       const std::string& /*url*/)
{
  return nullptr;
}

ParticleSystem* ParticleSystem::Parse(const Json::value& parsedParticleSystem,
                                      Scene* scene, const std::string& rootUrl)
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
    particleSystem->emitter.set<AbstractMeshPtr>(scene->getLastMeshByID(
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
#if 0
    scene->beginAnimation(
      particleSystem,
      Json::GetNumber(parsedParticleSystem, "autoAnimateFrom", 0),
      Json::GetNumber(parsedParticleSystem, "autoAnimateTo", 0),
      Json::GetBool(parsedParticleSystem, "autoAnimateLoop"),
      Json::GetNumber(parsedParticleSystem, "autoAnimateSpeed", 1.f));
#endif
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
  particleSystem->spriteCellChangeSpeed = Json::GetNumber<float>(
    parsedParticleSystem, "spriteCellChangeSpeed", 1.f);
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
