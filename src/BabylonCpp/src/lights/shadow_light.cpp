#include <babylon/lights/shadow_light.h>

#include <babylon/math/axis.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

ShadowLight::ShadowLight(const std::string& name, Scene* scene)
    : IShadowLight{name, scene}, _needProjectionMatrixCompute{true}
{
}

ShadowLight::~ShadowLight()
{
}

Vector3& ShadowLight::direction()
{
  return *_direction;
}

void ShadowLight::setDirection(const Vector3& value)
{
  _direction = std::make_unique<Vector3>(value);
}

const Nullable<float>& ShadowLight::shadowMinZ() const
{
  return _shadowMinZ;
}

void ShadowLight::setShadowMinZ(float value)
{
  _shadowMinZ = value;
  forceProjectionMatrixCompute();
}

const Nullable<float>& ShadowLight::shadowMaxZ() const
{
  return _shadowMaxZ;
}

void ShadowLight::setShadowMaxZ(float value)
{
  _shadowMaxZ = value;
  forceProjectionMatrixCompute();
}

bool ShadowLight::computeTransformedInformation()
{
  if (parent() && parent()->getWorldMatrix()) {
    if (!transformedPosition) {
      transformedPosition = std::make_unique<Vector3>(Vector3::Zero());
    }
    Vector3::TransformCoordinatesToRef(position, *parent()->getWorldMatrix(),
                                       *transformedPosition);

    // In case the direction is present.
    if (_direction) {
      if (!transformedDirection) {
        transformedDirection = std::make_unique<Vector3>(Vector3::Zero());
      }
      Vector3::TransformNormalToRef(direction(), *parent()->getWorldMatrix(),
                                    *transformedDirection);
    }
    return true;
  }
  return false;
}

Scene* ShadowLight::getScene()
{
  return Node::getScene();
}

float ShadowLight::getDepthScale() const
{
  return 30.f;
}

Vector3 ShadowLight::getShadowDirection(unsigned int /*faceIndex*/)
{
  return transformedDirection ? *transformedDirection : direction();
}

Vector3 ShadowLight::getAbsolutePosition()
{
  return transformedPosition ? *transformedPosition : position;
}

Vector3 ShadowLight::setDirectionToTarget(const Vector3& target)
{
  setDirection(Vector3::Normalize(target.subtract(position)));
  return direction();
}

Vector3 ShadowLight::getRotation()
{
  direction().normalize();
  auto xaxis = Vector3::Cross(direction(), Axis::Y);
  auto yaxis = Vector3::Cross(xaxis, direction());
  return Vector3::RotationFromAxis(xaxis, yaxis, direction());
}

bool ShadowLight::needCube() const
{
  return false;
}

bool ShadowLight::needProjectionMatrixCompute()
{
  return _needProjectionMatrixCompute;
}

void ShadowLight::forceProjectionMatrixCompute()
{
  _needProjectionMatrixCompute = true;
}

Matrix* ShadowLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std::make_unique<Matrix>(Matrix::Identity());
  }

  Matrix::TranslationToRef(position.x, position.y, position.z, *_worldMatrix);

  return _worldMatrix.get();
}

IShadowLight* ShadowLight::setShadowProjectionMatrix(
  Matrix& matrix, Matrix& viewMatrix,
  const std::vector<AbstractMesh*>& renderList)
{
  if (customProjectionMatrixBuilder) {
    customProjectionMatrixBuilder(viewMatrix, renderList, matrix);
  }
  else {
    _setDefaultShadowProjectionMatrix(matrix, viewMatrix, renderList);
  }
  return this;
}

} // end of namespace BABYLON
