#include <babylon/lights/spot_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/axis.h>

namespace BABYLON {

SpotLight::SpotLight(const std::string& iName, const Vector3& iPosition,
                     const Vector3& direction, float iAngle, float iExponent,
                     Scene* scene)
    : ShadowLight{iName, scene}, exponent{iExponent}
{
  position = iPosition;
  setDirection(direction);
  setAngle(iAngle);
}

SpotLight::~SpotLight()
{
}

IReflect::Type SpotLight::type() const
{
  return IReflect::Type::SPOTLIGHT;
}

const char* SpotLight::getClassName() const
{
  return "SpotLight";
}

unsigned int SpotLight::getTypeID() const
{
  return Light::LIGHTTYPEID_SPOTLIGHT;
}

float SpotLight::angle() const
{
  return _angle;
}

void SpotLight::setAngle(float value)
{
  _angle = value;
  forceProjectionMatrixCompute();
}

float SpotLight::shadowAngleScale() const
{
  return _shadowAngleScale;
}

void SpotLight::setShadowAngleScale(float value)
{
  _shadowAngleScale = value;
  forceProjectionMatrixCompute();
}

void SpotLight::_setDefaultShadowProjectionMatrix(
  Matrix& matrix, const Matrix& /*viewMatrix*/,
  const std::vector<AbstractMesh*>& /*renderList*/)
{
  auto activeCamera = getScene()->activeCamera;

  _shadowAngleScale = _shadowAngleScale ? *_shadowAngleScale : 1;
  auto angle        = _shadowAngleScale * _angle;

  Matrix::PerspectiveFovLHToRef(
    angle, 1.f, shadowMinZ() ? *shadowMinZ() : activeCamera->minZ,
    shadowMaxZ() ? *shadowMaxZ() : activeCamera->maxZ, matrix);
}

void SpotLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("vLightDirection", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->create();
}

void SpotLight::transferToEffect(Effect* /*effect*/,
                                 const std::string& lightIndex)
{
  auto normalizeDirection = Vector3::Zero();

  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData",             // Name
                                 (*transformedPosition).x, // X
                                 (*transformedPosition).y, // Y
                                 (*transformedPosition).z, // Z
                                 exponent,                 // Value
                                 lightIndex);              // Index

    normalizeDirection = Vector3::Normalize(*transformedDirection);
  }
  else {
    _uniformBuffer->updateFloat4("vLightData", // Name
                                 position.x,   // X
                                 position.y,   // Y
                                 position.z,   // Z
                                 exponent,     // Value
                                 lightIndex);  // Index

    normalizeDirection = Vector3::Normalize(direction());
  }

  _uniformBuffer->updateFloat4("vLightDirection",        // Name
                               normalizeDirection.x,     // X
                               normalizeDirection.y,     // Y
                               normalizeDirection.z,     // Z
                               std::cos(angle() * 0.5f), // Value
                               lightIndex);              // Index
}

} // end of namespace BABYLON
