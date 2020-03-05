#include <babylon/particles/emittertypes/cone_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

ConeParticleEmitter::ConeParticleEmitter(float iRadius, float iAngle, float iDirectionRandomizer)
    : radiusRange{1.f}
    , heightRange{1.f}
    , emitFromSpawnPointOnly{false}
    , radius{this, &ConeParticleEmitter::get_radius, &ConeParticleEmitter::set_radius}
    , angle{this, &ConeParticleEmitter::get_angle, &ConeParticleEmitter::set_angle}
    , directionRandomizer{iDirectionRandomizer}
{
  angle  = iAngle;
  radius = iRadius;
}

ConeParticleEmitter::~ConeParticleEmitter() = default;

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
    _height = _radius / std::tan(_angle / 2.f);
  }
  else {
    _height = 1.f;
  }
}

void ConeParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                 Vector3& directionToUpdate, Particle* particle,
                                                 bool isLocal)
{
  if (std::abs(std::cos(_angle)) == 1.f) {
    if (isLocal) {
      directionToUpdate.x = 0.f;
      directionToUpdate.y = 1.f;
      directionToUpdate.z = 0.f;
      return;
    }
    Vector3::TransformNormalFromFloatsToRef(0.f, 1.0, 0.f, worldMatrix, directionToUpdate);
  }
  else {
    // measure the direction Vector from the emitter to the particle.
    auto direction   = particle->position.subtract(worldMatrix.getTranslation()).normalize();
    const auto randX = Scalar::RandomRange(0.f, directionRandomizer);
    const auto randY = Scalar::RandomRange(0.f, directionRandomizer);
    const auto randZ = Scalar::RandomRange(0.f, directionRandomizer);
    direction.x += randX;
    direction.y += randY;
    direction.z += randZ;
    direction.normalize();

    if (isLocal) {
      directionToUpdate.copyFrom(direction);
      return;
    }

    Vector3::TransformNormalFromFloatsToRef(direction.x, direction.y, direction.z, worldMatrix,
                                            directionToUpdate);
  }
}

void ConeParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                Vector3& positionToUpdate, Particle* /*particle*/,
                                                bool isLocal)
{
  const auto s = Scalar::RandomRange(0.f, Math::PI2);
  auto h       = 0.f;

  if (!emitFromSpawnPointOnly) {
    h = Scalar::RandomRange(0.f, heightRange);
    // Better distribution in a cone at normal angles.
    h = 1.f - h * h;
  }
  else {
    h = 0.0001f;
  }
  auto iRadius = _radius - Scalar::RandomRange(0.f, _radius * radiusRange);
  iRadius      = iRadius * h;

  const auto randX = iRadius * std::sin(s);
  const auto randZ = iRadius * std::cos(s);
  const auto randY = h * _height;

  if (isLocal) {
    positionToUpdate.x = randX;
    positionToUpdate.y = randY;
    positionToUpdate.z = randZ;
    return;
  }

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> ConeParticleEmitter::clone() const
{
  auto newOne = std::make_unique<ConeParticleEmitter>(_radius, _angle, directionRandomizer);

  return newOne;
}

void ConeParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat2("radius", _radius, radiusRange);
  effect->setFloat("coneAngle", _angle);
  effect->setFloat2("height", _height, heightRange);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

std::string ConeParticleEmitter::getEffectDefines() const
{
  std::string defines = "#define CONEEMITTER";

  if (emitFromSpawnPointOnly) {
    defines += "\n#define CONEEMITTERSPAWNPOINT";
  }

  return defines;
}

std::string ConeParticleEmitter::getClassName() const
{
  return "ConeParticleEmitter";
}

json ConeParticleEmitter::serialize() const
{
  return nullptr;
}

void ConeParticleEmitter::parse(const json& serializationObject, Scene* /*scene*/)
{
  if (json_util::has_valid_key_value(serializationObject, "radius")) {
    radius = json_util::get_number<float>(serializationObject, "radius", 1.f);
  }
  if (json_util::has_valid_key_value(serializationObject, "angle")) {
    angle = json_util::get_number<float>(serializationObject, "angle", Math::PI);
  }
  if (json_util::has_valid_key_value(serializationObject, "directionRandomizer")) {
    directionRandomizer
      = json_util::get_number<float>(serializationObject, "directionRandomizer", 0.f);
  }
  if (json_util::has_valid_key_value(serializationObject, "radiusRange")) {
    radiusRange = json_util::get_number<float>(serializationObject, "radiusRange", 1.f);
  }
  if (json_util::has_valid_key_value(serializationObject, "heightRange")) {
    heightRange = json_util::get_number<float>(serializationObject, "heightRange", 1.f);
  }
  if (json_util::has_valid_key_value(serializationObject, "emitFromSpawnPointOnly")) {
    emitFromSpawnPointOnly = json_util::get_bool(serializationObject, "emitFromSpawnPointOnly");
  }
}

} // end of namespace BABYLON
