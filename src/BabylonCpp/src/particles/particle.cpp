#include <babylon/particles/particle.h>

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
    , _initialDirection{nullptr}
    , _currentColorGradient{nullptr}
    , _currentColor1{Color4(0, 0, 0, 0)}
    , _currentColor2{Color4(0, 0, 0, 0)}
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
    , _currentColorGradient{other._currentColorGradient}
    , _currentColor1{other._currentColor1}
    , _currentColor2{other._currentColor2}
    , particleSystem{other.particleSystem}
    , _currentFrameCounter{other._currentFrameCounter}
{
}

Particle::Particle(Particle&& other)
{
  *this = ::std::move(other);
}

Particle& Particle::operator=(const Particle& other)
{
  if (&other != this) {
    position              = other.position;
    direction             = other.direction;
    color                 = other.color;
    colorStep             = other.colorStep;
    lifeTime              = other.lifeTime;
    age                   = other.age;
    size                  = other.size;
    scale                 = other.scale;
    angle                 = other.angle;
    angularSpeed          = other.angularSpeed;
    cellIndex             = other.cellIndex;
    _initialDirection     = other._initialDirection;
    _currentColorGradient = other._currentColorGradient;
    _currentColor1        = other._currentColor1;
    _currentColor2        = other._currentColor2;
    particleSystem        = other.particleSystem;
    _currentFrameCounter  = other._currentFrameCounter;
  }

  return *this;
}

Particle& Particle::operator=(Particle&& other)
{
  if (&other != this) {
    position              = ::std::move(other.position);
    direction             = ::std::move(other.direction);
    color                 = ::std::move(other.color);
    colorStep             = ::std::move(other.colorStep);
    lifeTime              = ::std::move(other.lifeTime);
    age                   = ::std::move(other.age);
    size                  = ::std::move(other.size);
    scale                 = ::std::move(other.scale);
    angle                 = ::std::move(other.angle);
    angularSpeed          = ::std::move(other.angularSpeed);
    cellIndex             = ::std::move(other.cellIndex);
    _initialDirection     = ::std::move(other._initialDirection);
    _currentColorGradient = ::std::move(other._currentColorGradient);
    _currentColor1        = ::std::move(other._currentColor1);
    _currentColor2        = ::std::move(other._currentColor2);
    particleSystem        = ::std::move(other.particleSystem);
    _currentFrameCounter  = ::std::move(other._currentFrameCounter);
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

  if (particleSystem->spriteCellChangeSpeed == 0.f) {
    updateCellIndex = [this](float scaledUpdateSpeed) {
      _updateCellIndexWithSpeedCalculated(scaledUpdateSpeed);
    };
  }
  else {
    updateCellIndex = [this](float scaledUpdateSpeed) {
      _updateCellIndexWithSpeedCalculated(scaledUpdateSpeed);
    };
  }
}

void Particle::_updateCellIndexWithSpeedCalculated(float scaledUpdateSpeed)
{
  //   (ageOffset / scaledUpdateSpeed) / available cells
  float numberOfScaledUpdatesPerCell
    = ((lifeTime - age) / scaledUpdateSpeed)
      / (particleSystem->endSpriteCellID + 1 - cellIndex);

  _currentFrameCounter += scaledUpdateSpeed;
  if (_currentFrameCounter
      >= numberOfScaledUpdatesPerCell * scaledUpdateSpeed) {
    _currentFrameCounter = 0;
    ++cellIndex;
    if (cellIndex > particleSystem->endSpriteCellID) {
      cellIndex = particleSystem->endSpriteCellID;
    }
  }
}

void Particle::_updateCellIndexWithCustomSpeed()
{
  if (_currentFrameCounter >= particleSystem->spriteCellChangeSpeed) {
    ++cellIndex;
    _currentFrameCounter = 0;
    if (cellIndex > particleSystem->endSpriteCellID) {
      if (particleSystem->spriteCellLoop) {
        cellIndex = particleSystem->startSpriteCellID;
      }
      else {
        cellIndex = particleSystem->endSpriteCellID;
      }
    }
  }
  else {
    ++_currentFrameCounter;
  }
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
    other._initialDirection = nullptr;
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
}

} // end of namespace BABYLON
