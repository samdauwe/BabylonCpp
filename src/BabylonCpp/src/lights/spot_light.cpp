#include <babylon/lights/spot_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/axis.h>

namespace BABYLON {

bool SpotLight::NodeConstructorAdded = false;

void SpotLight::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "Light_Type_2", [](const string_t& name, Scene* scene,
                       const nullable_t<Json::value>& /*options*/) {
      return SpotLight::New(name, Vector3::Zero(), Vector3::Zero(), 0.f, 0.f,
                            scene);
    });
  SpotLight::NodeConstructorAdded = true;
}

SpotLight::SpotLight(const string_t& iName, const Vector3& iPosition,
                     const Vector3& iDirection, float iAngle, float iExponent,
                     Scene* scene)
    : ShadowLight{iName, scene}
    , exponent{iExponent}
    , angle{this, &SpotLight::get_angle, &SpotLight::set_angle}
    , innerAngle{this, &SpotLight::get_innerAngle, &SpotLight::set_innerAngle}
    , shadowAngleScale{this, &SpotLight::get_shadowAngleScale,
                       &SpotLight::set_shadowAngleScale}
    , projectionTextureMatrix{this, &SpotLight::get_projectionTextureMatrix}
    , projectionTextureLightNear{this,
                                 &SpotLight::get_projectionTextureLightNear,
                                 &SpotLight::set_projectionTextureLightNear}
    , projectionTextureLightFar{this, &SpotLight::get_projectionTextureLightFar,
                                &SpotLight::set_projectionTextureLightFar}
    , projectionTextureUpDirection{this,
                                   &SpotLight::get_projectionTextureUpDirection,
                                   &SpotLight::set_projectionTextureUpDirection}
    , projectionTexture{this, &SpotLight::get_projectionTexture,
                        &SpotLight::set_projectionTexture}
    , _projectionTextureLightNear{1e-6f}
    , _projectionTextureLightFar{1000.f}
    , _projectionTextureUpDirection{Vector3::Up()}
    , _innerAngle{0.f}
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
  position  = iPosition;
  direction = iDirection;
  set_angle(iAngle);
}

SpotLight::~SpotLight()
{
}

IReflect::Type SpotLight::type() const
{
  return IReflect::Type::SPOTLIGHT;
}

const string_t SpotLight::getClassName() const
{
  return "SpotLight";
}

unsigned int SpotLight::getTypeID() const
{
  return Light::LIGHTTYPEID_SPOTLIGHT;
}

float SpotLight::get_angle() const
{
  return _angle;
}

void SpotLight::set_angle(float value)
{
  _angle                                 = value;
  _cosHalfAngle                          = std::cos(value * 0.5f);
  _projectionTextureProjectionLightDirty = true;
  forceProjectionMatrixCompute();
  _computeAngleValues();
}

float SpotLight::get_innerAngle() const
{
  return _angle;
}

void SpotLight::set_innerAngle(float value)
{
  _innerAngle = value;
  _computeAngleValues();
}

float SpotLight::get_shadowAngleScale() const
{
  return _shadowAngleScale;
}

void SpotLight::set_shadowAngleScale(float value)
{
  _shadowAngleScale = value;
  forceProjectionMatrixCompute();
}

Matrix& SpotLight::get_projectionTextureMatrix()
{
  return _projectionTextureMatrix;
}

float SpotLight::get_projectionTextureLightNear() const
{
  return _projectionTextureLightNear;
}

void SpotLight::set_projectionTextureLightNear(float value)
{
  _projectionTextureLightNear            = value;
  _projectionTextureProjectionLightDirty = true;
}

float SpotLight::get_projectionTextureLightFar() const
{
  return _projectionTextureLightFar;
}

void SpotLight::set_projectionTextureLightFar(float value)
{
  _projectionTextureLightFar             = value;
  _projectionTextureProjectionLightDirty = true;
}

Vector3& SpotLight::get_projectionTextureUpDirection()
{
  return _projectionTextureUpDirection;
}

void SpotLight::set_projectionTextureUpDirection(const Vector3& value)
{
  _projectionTextureUpDirection          = value;
  _projectionTextureProjectionLightDirty = true;
}

BaseTexture*& SpotLight::get_projectionTexture()
{
  return _projectionTexture;
}

void SpotLight::set_projectionTexture(BaseTexture* const& value)
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
  _uniformBuffer->addUniform("vLightFalloff", 4);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void SpotLight::_computeAngleValues()
{
  _lightAngleScale
    = 1.f
      / ::std::max(0.001f, (::std::cos(_innerAngle * 0.5f) - _cosHalfAngle));
  _lightAngleOffset = -_cosHalfAngle * _lightAngleScale;
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
                                 lightIndex               // Index
    );

    normalizeDirection = Vector3::Normalize(transformedDirection());
  }
  else {
    _uniformBuffer->updateFloat4("vLightData", // Name
                                 position().x, // X
                                 position().y, // Y
                                 position().z, // Z
                                 exponent,     // Value
                                 lightIndex    // Index
    );

    normalizeDirection = Vector3::Normalize(direction());
  }

  _uniformBuffer->updateFloat4("vLightDirection",    // Name
                               normalizeDirection.x, // X
                               normalizeDirection.y, // Y
                               normalizeDirection.z, // Z
                               _cosHalfAngle,        // Value
                               lightIndex            // Index
  );

  _uniformBuffer->updateFloat4("vLightFalloff",      // Name
                               range,                // X
                               _inverseSquaredRange, // Y
                               _lightAngleScale,     // Z
                               _lightAngleOffset,    // Value
                               lightIndex            // Index
  );

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

void SpotLight::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  ShadowLight::dispose(doNotRecurse, disposeMaterialAndTextures);
  if (_projectionTexture) {
    _projectionTexture->dispose();
  }
}

void SpotLight::prepareLightSpecificDefines(MaterialDefines& defines,
                                            unsigned int lightIndex)
{
  defines.resizeLights(lightIndex);
  defines.spotlights[lightIndex]            = true;
  defines.projectedLightTexture[lightIndex] = projectionTexture ? true : false;
}

} // end of namespace BABYLON
