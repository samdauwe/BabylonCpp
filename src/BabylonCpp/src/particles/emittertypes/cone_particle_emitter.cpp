#include <babylon/particles/emittertypes/cone_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/matrix.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

ConeParticleEmitter::ConeParticleEmitter(float iRadius, float iAngle,
                                         float iDirectionRandomizer)
    : radius{this, &ConeParticleEmitter::get_radius,
             &ConeParticleEmitter::set_radius}
    , angle{this, &ConeParticleEmitter::get_angle,
            &ConeParticleEmitter::set_angle}
    , directionRandomizer{iDirectionRandomizer}
{
  angle  = iAngle;
  radius = iRadius;
}

ConeParticleEmitter::~ConeParticleEmitter()
{
}

float ConeParticleEmitter::get_radius() const
{
  return _radius;
}

void ConeParticleEmitter::set_radius(float value)
{
  _radius = value;
  _buildHeight();
}

float ConeParticleEmitter::get_angle() const
{
  return _angle;
}

void ConeParticleEmitter::set_angle(float value)
{
  _angle = value;
  _buildHeight();
}

void ConeParticleEmitter::_buildHeight()
{
  if (_angle != 0.f) {
    _height = _radius / ::std::tan(_angle / 2.f);
  }
  else {
    _height = 1.f;
  }
}

void ConeParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                 Vector3& directionToUpdate,
                                                 Particle* particle)
{
  if (_angle == 0.f) {
    Vector3::TransformNormalFromFloatsToRef(0, 1.0, 0, worldMatrix,
                                            directionToUpdate);
  }
  else {
    // measure the direction Vector from the emitter to the particle.
    auto direction
      = particle->position.subtract(worldMatrix.getTranslation()).normalize();
    const auto randX = Scalar::RandomRange(0.f, directionRandomizer);
    const auto randY = Scalar::RandomRange(0.f, directionRandomizer);
    const auto randZ = Scalar::RandomRange(0.f, directionRandomizer);
    direction.x += randX;
    direction.y += randY;
    direction.z += randZ;
    direction.normalize();

    Vector3::TransformNormalFromFloatsToRef(
      direction.x, direction.y, direction.z, worldMatrix, directionToUpdate);
  }
}

void ConeParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                Vector3& positionToUpdate,
                                                Particle* /*particle*/)
{
  const auto s = Scalar::RandomRange(0.f, Math::PI2);
  auto h       = Scalar::RandomRange(0.f, 1.f);
  // Better distribution in a cone at normal angles.
  h           = 1.f - h * h;
  auto radius = Scalar::RandomRange(0.f, _radius);
  radius      = radius * h;

  const auto randX = radius * ::std::sin(s);
  const auto randZ = radius * ::std::cos(s);
  const auto randY = h * _height;

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                               positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> ConeParticleEmitter::clone() const
{
  auto newOne = ::std::make_unique<ConeParticleEmitter>(_radius, _angle,
                                                        directionRandomizer);

  return newOne;
}

void ConeParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", _radius);
  effect->setFloat("coneAngle", _angle);
  effect->setFloat("height", _height);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

const char* ConeParticleEmitter::getEffectDefines() const
{
  return "#define CONEEMITTER";
}

const char* ConeParticleEmitter::getClassName() const
{
  return "ConeParticleEmitter";
}

Json::object ConeParticleEmitter::serialize() const
{
  return Json::object();
}

void ConeParticleEmitter::parse(const Json::value& serializationObject)
{
  if (serializationObject.contains("radius")) {
    radius = Json::GetNumber<float>(serializationObject, "radius", 1.f);
  }
  if (serializationObject.contains("angle")) {
    angle = Json::GetNumber<float>(serializationObject, "angle", Math::PI);
  }
  if (serializationObject.contains("directionRandomizer")) {
    directionRandomizer
      = Json::GetNumber<float>(serializationObject, "directionRandomizer", 0.f);
  }
}

} // end of namespace BABYLON
