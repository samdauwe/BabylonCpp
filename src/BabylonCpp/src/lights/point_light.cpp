#include <babylon/lights/point_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

PointLight::PointLight(const string_t& iName, const Vector3& iPosition,
                       Scene* scene)
    : ShadowLight{iName, scene}, _shadowAngle{Math::PI_2}
{
  setPosition(iPosition);
}

PointLight::~PointLight()
{
}

IReflect::Type PointLight::type() const
{
  return IReflect::Type::POINTLIGHT;
}

float PointLight::shadowAngle() const
{
  return _shadowAngle;
}

void PointLight::setShadowAngle(float value)
{
  _shadowAngle = value;
  forceProjectionMatrixCompute();
}

Vector3& PointLight::direction()
{
  return *_direction;
}

void PointLight::setDirection(const Vector3& value)
{
  auto previousNeedCube = needCube();
  _direction            = ::std::make_unique<Vector3>(value);
  if (needCube() != previousNeedCube && _shadowGenerator) {
    _shadowGenerator->recreateShadowMap();
  }
}

const char* PointLight::getClassName() const
{
  return "PointLight";
}

unsigned int PointLight::getTypeID() const
{
  return Light::LIGHTTYPEID_POINTLIGHT();
}

bool PointLight::needCube() const
{
  return !_direction;
}

Vector3 PointLight::getShadowDirection(unsigned int faceIndex)
{
  if (_direction) {
    return ShadowLight::getShadowDirection(faceIndex);
  }
  else {
    switch (faceIndex) {
      case 0:
        return Vector3(1.f, 0.f, 0.f);
      case 1:
        return Vector3(-1.f, 0.f, 0.f);
      case 2:
        return Vector3(0.f, -1.f, 0.f);
      case 3:
        return Vector3(0.f, 1.f, 0.f);
      case 4:
        return Vector3(0.f, 0.f, 1.f);
      case 5:
        return Vector3(0.f, 0.f, -1.f);
      default:
        break;
    }
  }

  return Vector3::Zero();
}

void PointLight::_setDefaultShadowProjectionMatrix(
  Matrix& matrix, const Matrix& /*viewMatrix*/,
  const vector_t<AbstractMesh*>& /*renderList*/)
{
  auto activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  Matrix::PerspectiveFovLHToRef(shadowAngle(), 1.f, getDepthMinZ(activeCamera),
                                getDepthMaxZ(activeCamera), matrix);
}

void PointLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void PointLight::transferToEffect(Effect* /*effect*/,
                                  const string_t& lightIndex)
{
  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData",            //
                                 transformedPosition().x, //
                                 transformedPosition().y, //
                                 transformedPosition().z, //
                                 0.f,                     //
                                 lightIndex);
    return;
  }

  _uniformBuffer->updateFloat4("vLightData", position().x, position().y,
                               position().z, 0, lightIndex);
}

} // end of namespace BABYLON
