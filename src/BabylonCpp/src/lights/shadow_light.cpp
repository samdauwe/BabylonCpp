#include <babylon/lights/shadow_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/axis.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

ShadowLight::ShadowLight(const std::string& iName, Scene* scene)
    : IShadowLight{iName, scene}
    , shadowMinZ{this, &ShadowLight::get_shadowMinZ, &ShadowLight::set_shadowMinZ}
    , shadowMaxZ{this, &ShadowLight::get_shadowMaxZ, &ShadowLight::set_shadowMaxZ}
    , _direction{nullptr}
    , _shadowMinZ{std::nullopt}
    , _shadowMaxZ{std::nullopt}
    , _needProjectionMatrixCompute{true}
{
}

ShadowLight::~ShadowLight() = default;

Vector3& ShadowLight::get_position()
{
  return _position;
}

void ShadowLight::set_position(const Vector3& value)
{
  _setPosition(value);
}

Vector3& ShadowLight::get_direction()
{
  return *_direction;
}

void ShadowLight::set_direction(const Vector3& value)
{
  _direction = std::make_unique<Vector3>(value);
}

Vector3& ShadowLight::transformedPosition()
{
  return *_transformedPosition;
}

Vector3& ShadowLight::transformedDirection()
{
  return *_transformedDirection;
}

void ShadowLight::_setDirection(const Vector3& value)
{
  _direction = std::make_unique<Vector3>(value);
}

void ShadowLight::_setPosition(const Vector3& value)
{
  _position = value;
}

std::optional<float>& ShadowLight::get_shadowMinZ()
{
  return _shadowMinZ;
}

void ShadowLight::set_shadowMinZ(const std::optional<float>& value)
{
  _shadowMinZ = value;
  forceProjectionMatrixCompute();
}

std::optional<float>& ShadowLight::get_shadowMaxZ()
{
  return _shadowMaxZ;
}

void ShadowLight::set_shadowMaxZ(const std::optional<float>& value)
{
  _shadowMaxZ = value;
  forceProjectionMatrixCompute();
}

bool ShadowLight::computeTransformedInformation()
{
  if (parent()) {
    if (!_transformedPosition) {
      _transformedPosition = std::make_unique<Vector3>(Vector3::Zero());
    }
    Vector3::TransformCoordinatesToRef(position, parent()->getWorldMatrix(), *_transformedPosition);

    // In case the direction is present.
    if (_direction) {
      if (!_transformedDirection) {
        _transformedDirection = std::make_unique<Vector3>(Vector3::Zero());
      }
      Vector3::TransformNormalToRef(direction(), parent()->getWorldMatrix(),
                                    *_transformedDirection);
    }
    return true;
  }
  return false;
}

Scene* ShadowLight::getScene() const
{
  return Node::getScene();
}

float ShadowLight::getDepthScale() const
{
  return 50.f;
}

Vector3 ShadowLight::getShadowDirection(unsigned int /*faceIndex*/)
{
  return _transformedDirection ? *_transformedDirection : direction();
}

Vector3 ShadowLight::getAbsolutePosition()
{
  return _transformedPosition ? *_transformedPosition : position;
}

Vector3 ShadowLight::setDirectionToTarget(const Vector3& target)
{
  direction = Vector3::Normalize(target.subtract(position));
  return direction();
}

Vector3 ShadowLight::getRotation()
{
  direction().normalize();
  auto xaxis = Vector3::Cross(direction(), Axis::Y());
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

void ShadowLight::_initCache()
{
  IShadowLight::_initCache();

  _cache.position = Vector3::Zero();
}

bool ShadowLight::_isSynchronized()
{
  return _cache.position.equals(position);
}

Matrix& ShadowLight::computeWorldMatrix(bool force, bool /*useWasUpdatedFlag*/)
{
  if (!force && isSynchronized()) {
    _currentRenderId = getScene()->getRenderId();
    return _worldMatrix;
  }

  _updateCache();
  _cache.position.copyFrom(position);

  Matrix::TranslationToRef(position().x, position().y, position().z, _worldMatrix);

  if (parent) {
    _worldMatrix.multiplyToRef(parent()->getWorldMatrix(), _worldMatrix);

    _markSyncedWithParent();
  }

  // Cache the determinant
  _worldMatrixDeterminantIsDirty = true;

  return _worldMatrix;
}

float ShadowLight::getDepthMinZ(const Camera& activeCamera) const
{
  return shadowMinZ().has_value() ? *shadowMinZ() : activeCamera.minZ;
}

float ShadowLight::getDepthMaxZ(const Camera& activeCamera) const
{
  return shadowMaxZ().has_value() ? *shadowMaxZ() : activeCamera.maxZ;
}

IShadowLight* ShadowLight::setShadowProjectionMatrix(Matrix& matrix, Matrix& viewMatrix,
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
