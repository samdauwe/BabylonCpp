#include <babylon/lights/spot_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/axis.h>

namespace BABYLON {

SpotLight::SpotLight(const std::string& iName, const Vector3& iPosition,
                     const Vector3& iDirection, float iAngle, float iExponent,
                     Scene* scene)
    : IShadowLight{iName, scene}
    , position{iPosition}
    , direction{iDirection}
    , angle{iAngle}
    , exponent{iExponent}
    , _transformedDirection{nullptr}
    , _worldMatrix{nullptr}
{
}

SpotLight::~SpotLight()
{
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

const char* SpotLight::getClassName() const
{
  return "SpotLight";
}

IReflect::Type SpotLight::type() const
{
  return IReflect::Type::SPOTLIGHT;
}

Scene* SpotLight::getScene()
{
  return Node::getScene();
}

Vector3 SpotLight::getAbsolutePosition()
{
  return transformedPosition ? *transformedPosition : position;
}

float SpotLight::getDepthScale() const
{
  return 30.f;
}

void SpotLight::setShadowProjectionMatrix(
  Matrix& matrix, const Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  if (customProjectionMatrixBuilder) {
    customProjectionMatrixBuilder(viewMatrix, renderList, matrix);
  }
  else {
    auto activeCamera = getScene()->activeCamera;
    Matrix::PerspectiveFovLHToRef(
      angle, 1.f, shadowMinZ.hasValue() ? shadowMinZ.value : activeCamera->minZ,
      shadowMaxZ.hasValue() ? shadowMaxZ.value : activeCamera->maxZ, matrix);
  }
}

bool SpotLight::needCube() const
{
  return false;
}

bool SpotLight::needRefreshPerFrame() const
{
  return false;
}

Vector3 SpotLight::getShadowDirection(unsigned int /*faceIndex*/)
{
  return direction;
}

Vector3& SpotLight::setDirectionToTarget(Vector3& target)
{
  direction = Vector3::Normalize(target.subtract(position));
  return direction;
}

bool SpotLight::computeTransformedPosition()
{
  if (parent() && parent()->getWorldMatrix()) {
    if (!transformedPosition) {
      transformedPosition = std_util::make_unique<Vector3>(Vector3::Zero());
    }

    Vector3::TransformCoordinatesToRef(position, *parent()->getWorldMatrix(),
                                       *transformedPosition);
    return true;
  }

  return false;
}

void SpotLight::transferToEffect(Effect* /*effect*/,
                                 const std::string& lightIndex)
{
  auto normalizeDirection = Vector3::Zero();

  if (parent() && parent()->getWorldMatrix()) {
    if (!_transformedDirection) {
      transformedPosition = std_util::make_unique<Vector3>(Vector3::Zero());
    }

    computeTransformedPosition();

    Vector3::TransformNormalToRef(direction, *parent()->getWorldMatrix(),
                                  *_transformedDirection);

    _uniformBuffer->updateFloat4("vLightData",           // Name
                                 transformedPosition->x, // X
                                 transformedPosition->y, // Y
                                 transformedPosition->z, // Z
                                 exponent,               // Value
                                 lightIndex);            // Index

    normalizeDirection = Vector3::Normalize(*_transformedDirection);
  }
  else {
    _uniformBuffer->updateFloat4("vLightData", // Name
                                 position.x,   // X
                                 position.y,   // Y
                                 position.z,   // Z
                                 exponent,     // Value
                                 lightIndex);  // Index
    normalizeDirection = Vector3::Normalize(direction);
  }

  _uniformBuffer->updateFloat4("vLightDirection",      // Name
                               normalizeDirection.x,   // X
                               normalizeDirection.y,   // Y
                               normalizeDirection.z,   // Z
                               std::cos(angle * 0.5f), // Value
                               lightIndex);            // Index
}

Matrix* SpotLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
  }

  Matrix::TranslationToRef(position.x, position.y, position.z, *_worldMatrix);

  return _worldMatrix.get();
}

unsigned int SpotLight::getTypeID() const
{
  return 2;
}

Vector3 SpotLight::getRotation()
{

  direction.normalize();

  auto xaxis = Vector3::Cross(direction, Axis::Y);
  auto yaxis = Vector3::Cross(xaxis, direction);

  return Vector3::RotationFromAxis(xaxis, yaxis, direction);
}

} // end of namespace BABYLON
