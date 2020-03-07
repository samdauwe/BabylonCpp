#include <babylon/particles/particle.h>

#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/particles/sub_emitter.h>

namespace BABYLON {

size_t Particle::_Count = 0;

Particle::Particle(ParticleSystem* iParticleSystem)
    : id{Particle::_Count++}
    , position{Vector3::Zero()}
    , direction{Vector3::Zero()}
    , color{Color4(0.f, 0.f, 0.f, 0.f)}
    , colorStep{Color4(0.f, 0.f, 0.f, 0.f)}
    , lifeTime{1.f}
    , age{0.f}
    , size{0.f}
    , scale{Vector2(1.f, 1.f)}
    , angle{0.f}
    , angularSpeed{0.f}
    , cellIndex{0}
    , _randomCellOffset{std::nullopt}
    , _initialDirection{std::nullopt}
    , _initialStartSpriteCellID{0}
    , _initialEndSpriteCellID{0}
    , _currentColorGradient{std::nullopt}
    , _currentColor1{Color4(0, 0, 0, 0)}
    , _currentColor2{Color4(0, 0, 0, 0)}
    , _currentSizeGradient{std::nullopt}
    , _currentSize1{0.f}
    , _currentSize2{0.f}
    , _currentAngularSpeedGradient{std::nullopt}
    , _currentAngularSpeed1{0.f}
    , _currentAngularSpeed2{0.f}
    , _currentVelocityGradient{std::nullopt}
    , _currentVelocity1{0.f}
    , _currentVelocity2{0.f}
    , _currentLimitVelocityGradient{std::nullopt}
    , _currentLimitVelocity1{0.f}
    , _currentLimitVelocity2{0.f}
    , _currentDragGradient{std::nullopt}
    , _currentDrag1{0.f}
    , _currentDrag2{0.f}
    , _randomNoiseCoordinates1{std::nullopt}
    , _localPosition{std::nullopt}
    , particleSystem{iParticleSystem}
    , _currentFrameCounter{0}
{
  if (!particleSystem->isAnimationSheetEnabled()) {
    return;
  }

  updateCellInfoFromSystem();
}

Particle::Particle(const Particle& other) = default;

Particle::Particle(Particle&& other)
{
  *this = std::move(other);
}

Particle& Particle::operator=(const Particle& other) = default;

Particle& Particle::operator=(Particle&& other) = default;

Particle::~Particle() = default;

Particle* Particle::clone() const
{
  return new Particle(*this);
}

void Particle::updateCellInfoFromSystem()
{
  cellIndex = particleSystem->startSpriteCellID;
}

void Particle::updateCellIndex()
{
  auto offsetAge   = age;
  auto changeSpeed = particleSystem->spriteCellChangeSpeed;

  if (particleSystem->spriteRandomStartCell) {
    if (!_randomCellOffset.has_value()) {
      _randomCellOffset = Math::random() * lifeTime;
    }

    if (changeSpeed == 0.f) { // Special case when speed = 0 meaning we want to
                              // stay on initial cell
      changeSpeed = 1.f;
      offsetAge   = *_randomCellOffset;
    }
    else {
      offsetAge += *_randomCellOffset;
    }
  }

  auto dist  = (_initialEndSpriteCellID - _initialStartSpriteCellID);
  auto ratio = Scalar::Clamp(std::fmod((offsetAge * changeSpeed), lifeTime) / lifeTime);

  cellIndex = static_cast<unsigned int>(_initialStartSpriteCellID + (ratio * dist));
}

void Particle::_inheritParticleInfoToSubEmitter(const SubEmitterPtr& subEmitter)
{
  if (std::holds_alternative<AbstractMeshPtr>(subEmitter->particleSystem->emitter)) {
    auto emitterMesh = std::get<AbstractMeshPtr>(subEmitter->particleSystem->emitter);
    emitterMesh->position().copyFrom(position);
    if (subEmitter->inheritDirection) {
      emitterMesh->setDirection(direction.normalize(), 0, Math::PI_2);
    }
  }
  else if (std::holds_alternative<Vector3>(subEmitter->particleSystem->emitter)) {
    auto emitterPosition = std::get<Vector3>(subEmitter->particleSystem->emitter);
    emitterPosition.copyFrom(position);
  }
  // Set inheritedVelocityOffset to be used when new particles are created
  direction.scaleToRef(subEmitter->inheritedVelocityAmount / 2.f, TmpVectors::Vector3Array[0]);
  subEmitter->particleSystem->_inheritedVelocityOffset.copyFrom(TmpVectors::Vector3Array[0]);
}

void Particle::_inheritParticleInfoToSubEmitters()
{
  if (!_attachedSubEmitters.empty()) {
    for (const auto& subEmitter : _attachedSubEmitters) {
      _inheritParticleInfoToSubEmitter(subEmitter);
    }
  }
}

void Particle::_reset()
{
  age                           = 0.f;
  id                            = Particle::_Count++;
  _currentColorGradient         = std::nullopt;
  _currentSizeGradient          = std::nullopt;
  _currentAngularSpeedGradient  = std::nullopt;
  _currentVelocityGradient      = std::nullopt;
  _currentLimitVelocityGradient = std::nullopt;
  _currentDragGradient          = std::nullopt;
  cellIndex                     = particleSystem->startSpriteCellID;
  _randomCellOffset             = std::nullopt;
}

void Particle::copyTo(Particle& other)
{
  other.position.copyFrom(position);
  if (_initialDirection) {
    if (other._initialDirection) {
      other._initialDirection = _initialDirection;
    }
    else {
      other._initialDirection = _initialDirection;
    }
  }
  else {
    other._initialDirection = std::nullopt;
  }
  other.direction.copyFrom(direction);
  if (_localPosition) {
    if (other._localPosition) {
      other._localPosition->copyFrom(*_localPosition);
    }
    else {
      other._localPosition = _localPosition->copy();
    }
  }
  other.color.copyFrom(color);
  other.colorStep.copyFrom(colorStep);
  other.lifeTime          = lifeTime;
  other.age               = age;
  other._randomCellOffset = _randomCellOffset;
  other.size              = size;
  other.scale.copyFrom(scale);
  other.angle                = angle;
  other.angularSpeed         = angularSpeed;
  other.particleSystem       = particleSystem;
  other.cellIndex            = cellIndex;
  other.id                   = id;
  other._attachedSubEmitters = _attachedSubEmitters;
  if (_currentColorGradient) {
    other._currentColorGradient = _currentColorGradient;
    other._currentColor1.copyFrom(_currentColor1);
    other._currentColor2.copyFrom(_currentColor2);
  }
  if (_currentSizeGradient) {
    other._currentSizeGradient = _currentSizeGradient;
    other._currentSize1        = _currentSize1;
    other._currentSize2        = _currentSize2;
  }
  if (_currentAngularSpeedGradient) {
    other._currentAngularSpeedGradient = _currentAngularSpeedGradient;
    other._currentAngularSpeed1        = _currentAngularSpeed1;
    other._currentAngularSpeed2        = _currentAngularSpeed2;
  }
  if (_currentVelocityGradient) {
    other._currentVelocityGradient = _currentVelocityGradient;
    other._currentVelocity1        = _currentVelocity1;
    other._currentVelocity2        = _currentVelocity2;
  }
  if (_currentLimitVelocityGradient) {
    other._currentLimitVelocityGradient = _currentLimitVelocityGradient;
    other._currentLimitVelocity1        = _currentLimitVelocity1;
    other._currentLimitVelocity2        = _currentLimitVelocity2;
  }
  if (_currentDragGradient) {
    other._currentDragGradient = _currentDragGradient;
    other._currentDrag1        = _currentDrag1;
    other._currentDrag2        = _currentDrag2;
  }
  if (particleSystem->isAnimationSheetEnabled) {
    other._initialStartSpriteCellID = _initialStartSpriteCellID;
    other._initialEndSpriteCellID   = _initialEndSpriteCellID;
  }
  if (particleSystem->useRampGradients) {
    other.remapData.copyFrom(remapData);
  }
  if (_randomNoiseCoordinates1) {
    if (other._randomNoiseCoordinates1) {
      other._randomNoiseCoordinates1->copyFrom(*_randomNoiseCoordinates1);
      other._randomNoiseCoordinates2.copyFrom(_randomNoiseCoordinates2);
    }
    else {
      other._randomNoiseCoordinates1 = _randomNoiseCoordinates1;
      other._randomNoiseCoordinates2 = _randomNoiseCoordinates2;
    }
  }
}

} // end of namespace BABYLON
