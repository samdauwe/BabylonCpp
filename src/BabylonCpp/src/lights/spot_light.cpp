#include <babylon/lights/spot_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/axis.h>

namespace BABYLON {

SpotLight::SpotLight(const string_t& iName, const Vector3& iPosition,
                     const Vector3& direction, float iAngle, float iExponent,
                     Scene* scene)
    : ShadowLight{iName, scene}
    , exponent{iExponent}
    , _projectionTextureLightNear{1e-6f}
    , _projectionTextureLightFar{1000.f}
    , _projectionTextureUpDirection{Vector3::Up()}
    , _projectionTextureMatrix{Matrix::Zero()}
    , _projectionTexture{nullptr}
    , _projectionTextureViewLightDirty{true}
    , _projectionTextureProjectionLightDirty{true}
    , _projectionTextureDirty{true}
    , _projectionTextureViewTargetVector{Vector3::Zero()}
    , _projectionTextureViewLightMatrix{Matrix::Zero()}
    , _projectionTextureProjectionLightMatrix{Matrix::Zero()}
    , _projectionTextureScalingMatrix{Matrix::FromValues( //
        0.5f, 0.f, 0.f, 0.f,                              //
        0.f, 0.5f, 0.f, 0.f,                              //
        0.f, 0.f, 0.5f, 0.f,                              //
        0.5f, 0.5f, 0.5f, 1.f                             //
        )}
{
  setPosition(iPosition);
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
  return Light::LIGHTTYPEID_SPOTLIGHT();
}

float SpotLight::angle() const
{
  return _angle;
}

void SpotLight::setAngle(float value)
{
  _angle                                 = value;
  _projectionTextureProjectionLightDirty = true;
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
Matrix& SpotLight::projectionTextureMatrix()
{
  return _projectionTextureMatrix;
}

const Matrix& SpotLight::projectionTextureMatrix() const
{
  return _projectionTextureMatrix;
}

float SpotLight::projectionTextureLightNear() const
{
  return _projectionTextureLightNear;
}

void SpotLight::setProjectionTextureLightNear(float value)
{
  _projectionTextureLightNear            = value;
  _projectionTextureProjectionLightDirty = true;
}

float SpotLight::projectionTextureLightFar() const
{
  return _projectionTextureLightFar;
}

void SpotLight::setProjectionTextureLightFar(float value)
{
  _projectionTextureLightFar             = value;
  _projectionTextureProjectionLightDirty = true;
}

Vector3& SpotLight::projectionTextureUpDirection()
{
  return _projectionTextureUpDirection;
}

const Vector3& SpotLight::projectionTextureUpDirection() const
{
  return _projectionTextureUpDirection;
}

void SpotLight::setProjectionTextureUpDirection(const Vector3& value)
{
  _projectionTextureUpDirection          = value;
  _projectionTextureProjectionLightDirty = true;
}

BaseTexture* SpotLight::projectionTexture() const
{
  return _projectionTexture;
}

void SpotLight::setProjectionTexture(BaseTexture* value)
{
  _projectionTexture      = value;
  _projectionTextureDirty = true;
}

void SpotLight::_setDirection(const Vector3& value)
{
  ShadowLight::_setDirection(value);
  _projectionTextureViewLightDirty = true;
}

void SpotLight::_setPosition(const Vector3& value)
{
  ShadowLight::_setPosition(value);
  _projectionTextureViewLightDirty = true;
}

void SpotLight::_setDefaultShadowProjectionMatrix(
  Matrix& matrix, const Matrix& /*viewMatrix*/,
  const vector_t<AbstractMesh*>& /*renderList*/)
{
  auto activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  _shadowAngleScale = _shadowAngleScale ? *_shadowAngleScale : 1.f;
  auto angle        = _shadowAngleScale * _angle;

  Matrix::PerspectiveFovLHToRef(angle, 1.f, getDepthMinZ(activeCamera),
                                getDepthMaxZ(activeCamera), matrix);
}

void SpotLight::_computeProjectionTextureViewLightMatrix()
{
  _projectionTextureViewLightDirty = false;
  _projectionTextureDirty          = true;

  position().addToRef(direction(), _projectionTextureViewTargetVector);
  Matrix::LookAtLHToRef(position(), _projectionTextureViewTargetVector,
                        _projectionTextureUpDirection,
                        _projectionTextureViewLightMatrix);
}

void SpotLight::_computeProjectionTextureProjectionLightMatrix()
{
  _projectionTextureProjectionLightDirty = false;
  _projectionTextureDirty                = true;

  const auto light_far  = projectionTextureLightFar();
  const auto light_near = projectionTextureLightNear();

  const auto P = light_far / (light_far - light_near);
  const auto Q = -P * light_near;
  const auto S = 1.f / ::std::tan(_angle / 2.f);
  const auto A = 1.f;

  Matrix::FromValuesToRef(                                    //
    S / A, 0.f, 0.f, 0.f,                                     //
    0.f, S, 0.f, 0.f,                                         //
    0.f, 0.f, P, 1.f,                                         //
    0.f, 0.f, Q, 0.f, _projectionTextureProjectionLightMatrix //
  );
}

void SpotLight::_computeProjectionTextureMatrix()
{
  _projectionTextureDirty = false;

  _projectionTextureViewLightMatrix.multiplyToRef(
    _projectionTextureProjectionLightMatrix, _projectionTextureMatrix);
  _projectionTextureMatrix.multiplyToRef(_projectionTextureScalingMatrix,
                                         _projectionTextureMatrix);
}

void SpotLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("vLightDirection", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void SpotLight::transferToEffect(Effect* effect, const string_t& lightIndex)
{
  auto normalizeDirection = Vector3::Zero();

  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData",            // Name
                                 transformedPosition().x, // X
                                 transformedPosition().y, // Y
                                 transformedPosition().z, // Z
                                 exponent,                // Value
                                 lightIndex);             // Index

    normalizeDirection = Vector3::Normalize(transformedDirection());
  }
  else {
    _uniformBuffer->updateFloat4("vLightData", // Name
                                 position().x, // X
                                 position().y, // Y
                                 position().z, // Z
                                 exponent,     // Value
                                 lightIndex);  // Index

    normalizeDirection = Vector3::Normalize(direction());
  }

  _uniformBuffer->updateFloat4("vLightDirection",          // Name
                               normalizeDirection.x,       // X
                               normalizeDirection.y,       // Y
                               normalizeDirection.z,       // Z
                               ::std::cos(angle() * 0.5f), // Value
                               lightIndex);                // Index

  if (projectionTexture() && projectionTexture()->isReady()) {
    if (_projectionTextureViewLightDirty) {
      _computeProjectionTextureViewLightMatrix();
    }
    if (_projectionTextureProjectionLightDirty) {
      _computeProjectionTextureProjectionLightMatrix();
    }
    if (_projectionTextureDirty) {
      _computeProjectionTextureMatrix();
    }
    effect->setMatrix("textureProjectionMatrix" + lightIndex,
                      _projectionTextureMatrix);
    effect->setTexture("projectionLightSampler" + lightIndex,
                       projectionTexture());
  }
}

void SpotLight::dispose(bool doNotRecurse)
{
  ShadowLight::dispose(doNotRecurse);
  if (_projectionTexture) {
    _projectionTexture->dispose();
  }
}

} // end of namespace BABYLON
