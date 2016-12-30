#include <babylon/particles/particle.h>

namespace BABYLON {

Particle::Particle()
    : position{Vector3::Zero()}
    , direction{Vector3::Zero()}
    , color{Color4(0.f, 0.f, 0.f, 0.f)}
    , colorStep{Color4(0.f, 0.f, 0.f, 0.f)}
    , lifeTime{1.f}
    , age{0.f}
    , size{0.f}
    , angle{0.f}
    , angularSpeed{0.f}
{
}

Particle::Particle(const Particle& other)
    : position{other.position}
    , direction{other.direction}
    , color{other.color}
    , colorStep{other.colorStep}
    , lifeTime{other.lifeTime}
    , age{other.age}
    , size{other.size}
    , angle{other.angle}
    , angularSpeed{other.angularSpeed}
{
}

Particle::Particle(Particle&& other)
{
  *this = std::move(other);
}

Particle& Particle::operator=(const Particle& other)
{
  if (&other != this) {
    position     = other.position;
    direction    = other.direction;
    color        = other.color;
    colorStep    = other.colorStep;
    lifeTime     = other.lifeTime;
    age          = other.age;
    size         = other.size;
    angle        = other.angle;
    angularSpeed = other.angularSpeed;
  }

  return *this;
}

Particle& Particle::operator=(Particle&& other)
{
  if (&other != this) {
    std::swap(position, other.position);
    std::swap(direction, other.direction);
    std::swap(color, other.color);
    std::swap(colorStep, other.colorStep);
    std::swap(lifeTime, other.lifeTime);
    std::swap(age, other.age);
    std::swap(size, other.size);
    std::swap(angle, other.angle);
    std::swap(angularSpeed, other.angularSpeed);
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

void Particle::copyTo(Particle& other)
{
  other.position.copyFrom(position);
  other.direction.copyFrom(direction);
  other.color.copyFrom(color);
  other.colorStep.copyFrom(colorStep);
  other.lifeTime     = lifeTime;
  other.age          = age;
  other.size         = size;
  other.angle        = angle;
  other.angularSpeed = angularSpeed;
}

} // end of namespace BABYLON