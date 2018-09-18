#include <babylon/particles/particle.h>

#include <babylon/math/scalar.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

Particle::Particle(ParticleSystem* iParticleSystem)
    : position{Vector3::Zero()}
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
    , _initialDirection{nullopt_t}
    , _initialStartSpriteCellID{0}
    , _initialEndSpriteCellID{0}
    , _currentColorGradient{nullopt_t}
    , _currentColor1{Color4(0, 0, 0, 0)}
    , _currentColor2{Color4(0, 0, 0, 0)}
    , _currentSizeGradient{nullopt_t}
    , _currentSize1{0.f}
    , _currentSize2{0.f}
    , _currentAngularSpeedGradient{nullopt_t}
    , _currentAngularSpeed1{0.f}
    , _currentAngularSpeed2{0.f}
    , _currentVelocityGradient{nullopt_t}
    , _currentVelocity1{0.f}
    , _currentVelocity2{0.f}
    , _currentLimitVelocityGradient{nullopt_t}
    , _currentLimitVelocity1{0.f}
    , _currentLimitVelocity2{0.f}
    , _currentDragGradient{nullopt_t}
    , _currentDrag1{0.f}
    , _currentDrag2{0.f}
    , particleSystem{iParticleSystem}
    , _currentFrameCounter{0}

{
  if (!particleSystem->isAnimationSheetEnabled()) {
    return;
  }

  updateCellInfoFromSystem();
}

Particle::Particle(const Particle& other)
    : position{other.position}
    , direction{other.direction}
    , color{other.color}
    , colorStep{other.colorStep}
    , lifeTime{other.lifeTime}
    , age{other.age}
    , size{other.size}
    , scale{other.scale}
    , angle{other.angle}
    , angularSpeed{other.angularSpeed}
    , cellIndex{other.cellIndex}
    , _initialDirection{other._initialDirection}
    , _initialStartSpriteCellID{other._initialStartSpriteCellID}
    , _initialEndSpriteCellID{other._initialEndSpriteCellID}
    , _currentColorGradient{other._currentColorGradient}
    , _currentColor1{other._currentColor1}
    , _currentColor2{other._currentColor2}
    , _currentSizeGradient{other._currentSizeGradient}
    , _currentSize1{other._currentSize1}
    , _currentSize2{other._currentSize2}
    , _currentAngularSpeedGradient{other._currentAngularSpeedGradient}
    , _currentAngularSpeed1{other._currentAngularSpeed1}
    , _currentAngularSpeed2{other._currentAngularSpeed2}
    , _currentVelocityGradient{other._currentVelocityGradient}
    , _currentVelocity1{other._currentVelocity1}
    , _currentVelocity2{other._currentVelocity2}
    , _currentLimitVelocityGradient{other._currentLimitVelocityGradient}
    , _currentLimitVelocity1{other._currentLimitVelocity1}
    , _currentLimitVelocity2{other._currentLimitVelocity2}
    , _currentDragGradient{other._currentDragGradient}
    , _currentDrag1{other._currentDrag1}
    , _currentDrag2{other._currentDrag2}
    , particleSystem{other.particleSystem}
    , _currentFrameCounter{other._currentFrameCounter}
{
}

Particle::Particle(Particle&& other)
{
  *this = std::move(other);
}

Particle& Particle::operator=(const Particle& other)
{
  if (&other != this) {
    position                      = other.position;
    direction                     = other.direction;
    color                         = other.color;
    colorStep                     = other.colorStep;
    lifeTime                      = other.lifeTime;
    age                           = other.age;
    size                          = other.size;
    scale                         = other.scale;
    angle                         = other.angle;
    angularSpeed                  = other.angularSpeed;
    cellIndex                     = other.cellIndex;
    _initialDirection             = other._initialDirection;
    _initialStartSpriteCellID     = other._initialStartSpriteCellID;
    _initialEndSpriteCellID       = other._initialEndSpriteCellID;
    _currentColorGradient         = other._currentColorGradient;
    _currentColor1                = other._currentColor1;
    _currentColor2                = other._currentColor2;
    _currentSizeGradient          = other._currentSizeGradient;
    _currentSize1                 = other._currentSize1;
    _currentSize2                 = other._currentSize2;
    _currentAngularSpeedGradient  = other._currentAngularSpeedGradient;
    _currentAngularSpeed1         = other._currentAngularSpeed1;
    _currentAngularSpeed2         = other._currentAngularSpeed2;
    _currentVelocityGradient      = other._currentVelocityGradient;
    _currentVelocity1             = other._currentVelocity1;
    _currentVelocity2             = other._currentVelocity2;
    _currentLimitVelocityGradient = other._currentLimitVelocityGradient;
    _currentLimitVelocity1        = other._currentLimitVelocity1;
    _currentLimitVelocity2        = other._currentLimitVelocity2;
    _currentDragGradient          = other._currentDragGradient;
    _currentDrag1                 = other._currentDrag1;
    _currentDrag2                 = other._currentDrag2;
    particleSystem                = other.particleSystem;
    _currentFrameCounter          = other._currentFrameCounter;
  }

  return *this;
}

Particle& Particle::operator=(Particle&& other)
{
  if (&other != this) {
    position                  = std::move(other.position);
    direction                 = std::move(other.direction);
    color                     = std::move(other.color);
    colorStep                 = std::move(other.colorStep);
    lifeTime                  = std::move(other.lifeTime);
    age                       = std::move(other.age);
    size                      = std::move(other.size);
    scale                     = std::move(other.scale);
    angle                     = std::move(other.angle);
    angularSpeed              = std::move(other.angularSpeed);
    cellIndex                 = std::move(other.cellIndex);
    _initialDirection         = std::move(other._initialDirection);
    _initialStartSpriteCellID = std::move(other._initialStartSpriteCellID);
    _initialEndSpriteCellID   = std::move(other._initialEndSpriteCellID);
    _currentColorGradient     = std::move(other._currentColorGradient);
    _currentColor1            = std::move(other._currentColor1);
    _currentColor2            = std::move(other._currentColor2);
    _currentSizeGradient      = std::move(other._currentSizeGradient);
    _currentSize1             = std::move(other._currentSize1);
    _currentSize2             = std::move(other._currentSize2);
    _currentAngularSpeedGradient
      = std::move(other._currentAngularSpeedGradient);
    _currentAngularSpeed1    = std::move(other._currentAngularSpeed1);
    _currentAngularSpeed2    = std::move(other._currentAngularSpeed2);
    _currentVelocityGradient = std::move(other._currentVelocityGradient);
    _currentVelocity1        = std::move(other._currentVelocity1);
    _currentVelocity2        = std::move(other._currentVelocity2);
    _currentLimitVelocityGradient
      = std::move(other._currentLimitVelocityGradient);
    _currentLimitVelocity1 = std::move(other._currentLimitVelocity1);
    _currentLimitVelocity2 = std::move(other._currentLimitVelocity2);
    _currentDragGradient   = std::move(other._currentDragGradient);
    _currentDrag1          = std::move(other._currentDrag1);
    _currentDrag2          = std::move(other._currentDrag2);
    particleSystem         = std::move(other.particleSystem);
    _currentFrameCounter   = std::move(other._currentFrameCounter);
  }

  return *this;
}

Particle::~Particle()
{
}

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
  auto dist  = (_initialEndSpriteCellID - _initialStartSpriteCellID);
  auto ratio = Scalar::Clamp(
    std::fmod((age * particleSystem->spriteCellChangeSpeed), lifeTime)
    / lifeTime);

  cellIndex
    = static_cast<unsigned int>(_initialStartSpriteCellID + (ratio * dist));
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
    other._initialDirection = nullopt_t;
  }
  other.direction.copyFrom(direction);
  other.color.copyFrom(color);
  other.colorStep.copyFrom(colorStep);
  other.lifeTime = lifeTime;
  other.age      = age;
  other.size     = size;
  other.scale.copyFrom(scale);
  other.angle          = angle;
  other.angularSpeed   = angularSpeed;
  other.particleSystem = particleSystem;
  other.cellIndex      = cellIndex;
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
}

} // end of namespace BABYLON
