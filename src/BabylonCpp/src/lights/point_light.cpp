#include <babylon/lights/point_light.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

bool PointLight::NodeConstructorAdded = false;

void PointLight::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "Light_Type_0", [](const std::string& iName, Scene* scene,
                       const std::optional<json>& /*options*/) {
      return PointLight::New(iName, Vector3::Zero(), scene);
    });
  PointLight::NodeConstructorAdded = true;
}

PointLight::PointLight(const std::string& iName, const Vector3& iPosition,
                       Scene* scene)
    : ShadowLight{iName, scene}
    , shadowAngle{this, &PointLight::get_shadowAngle,
                  &PointLight::set_shadowAngle}
    , _shadowAngle{Math::PI_2}
{
  position = iPosition;
}

PointLight::~PointLight()
{
}

IReflect::Type PointLight::type() const
{
  return IReflect::Type::POINTLIGHT;
}

float PointLight::get_shadowAngle() const
{
  return _shadowAngle;
}

void PointLight::set_shadowAngle(float value)
{
  _shadowAngle = value;
  forceProjectionMatrixCompute();
}

Vector3& PointLight::get_direction()
{
  return *_direction;
}

void PointLight::set_direction(const Vector3& value)
{
  auto previousNeedCube = needCube();
  _direction            = std::make_unique<Vector3>(value);
  if (needCube() != previousNeedCube && _shadowGenerator) {
    _shadowGenerator->recreateShadowMap();
  }
}

const std::string PointLight::getClassName() const
{
  return "PointLight";
}

unsigned int PointLight::getTypeID() const
{
  return Light::LIGHTTYPEID_POINTLIGHT;
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
  const std::vector<AbstractMesh*>& /*renderList*/)
{
  auto activeCamera = getScene()->activeCamera;

  if (!activeCamera) {
    return;
  }

  Matrix::PerspectiveFovLHToRef(shadowAngle(), 1.f, getDepthMinZ(*activeCamera),
                                getDepthMaxZ(*activeCamera), matrix);
}

void PointLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("vLightFalloff", 4);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

void PointLight::transferToEffect(const EffectPtr& /*effect*/,
                                  const std::string& lightIndex)
{
  if (computeTransformedInformation()) {
    _uniformBuffer->updateFloat4("vLightData",            //
                                 transformedPosition().x, //
                                 transformedPosition().y, //
                                 transformedPosition().z, //
                                 0.f,                     //
                                 lightIndex);
  }
  else {
    _uniformBuffer->updateFloat4("vLightData", position().x, position().y,
                                 position().z, 0, lightIndex);
  }

  _uniformBuffer->updateFloat4("vLightFalloff",      //
                               range(),              //
                               _inverseSquaredRange, //
                               0.f,                  //
                               0.f,                  //
                               lightIndex            //
  );
}

void PointLight::prepareLightSpecificDefines(MaterialDefines& defines,
                                             unsigned int lightIndex)
{
  defines.boolDef["POINTLIGHT" + std::to_string(lightIndex)] = true;
}

} // end of namespace BABYLON
