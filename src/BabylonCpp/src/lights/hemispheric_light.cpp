#include <babylon/lights/hemispheric_light.h>

#include <babylon/core/json.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

bool HemisphericLight::NodeConstructorAdded = false;

::std::function<void()> HemisphericLight::AddNodeConstructor = []() {
  Node::AddNodeConstructor(
    "Light_Type_3", [](const string_t& name, Scene* scene,
                       const nullable_t<Json::value>& /*options*/) {
      return HemisphericLight::New(name, Vector3::Zero(), scene);
    });
  HemisphericLight::NodeConstructorAdded = true;
};

HemisphericLight::HemisphericLight(const string_t& iName, Scene* scene)
    : HemisphericLight(iName, Vector3::Up(), scene)
{
}

HemisphericLight::HemisphericLight(const string_t& iName,
                                   const Vector3& iDirection, Scene* scene)
    : Light{iName, scene}
    , groundColor{Color3(0.f, 0.f, 0.f)}
    , direction{iDirection}
    , _worldMatrix{nullptr}
{
}

HemisphericLight::~HemisphericLight()
{
}

void HemisphericLight::_buildUniformLayout()
{
  _uniformBuffer->addUniform("vLightData", 4);
  _uniformBuffer->addUniform("vLightDiffuse", 4);
  _uniformBuffer->addUniform("vLightSpecular", 3);
  _uniformBuffer->addUniform("vLightGround", 3);
  _uniformBuffer->addUniform("shadowsInfo", 3);
  _uniformBuffer->addUniform("depthValues", 2);
  _uniformBuffer->create();
}

const string_t HemisphericLight::getClassName() const
{
  return "HemisphericLight";
}

IReflect::Type HemisphericLight::type() const
{
  return IReflect::Type::HEMISPHERICLIGHT;
}

Vector3 HemisphericLight::setDirectionToTarget(const Vector3& target)
{
  direction = Vector3::Normalize(target.subtract(Vector3::Zero()));
  return direction;
}

ShadowGenerator* HemisphericLight::getShadowGenerator()
{
  return nullptr;
}

void HemisphericLight::transferToEffect(Effect* /*effect*/,
                                        const string_t& lightIndex)
{
  auto normalizeDirection = Vector3::Normalize(direction);

  _uniformBuffer->updateFloat4("vLightData",         //
                               normalizeDirection.x, //
                               normalizeDirection.y, //
                               normalizeDirection.z, //
                               0.f,                  //
                               lightIndex);

  _uniformBuffer->updateColor3("vLightGround", groundColor.scale(intensity),
                               lightIndex);
}

Matrix* HemisphericLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
  }

  return _worldMatrix.get();
}

unsigned int HemisphericLight::getTypeID() const
{
  return Light::LIGHTTYPEID_HEMISPHERICLIGHT;
}

void HemisphericLight::prepareLightSpecificDefines(MaterialDefines& defines,
                                                   unsigned int lightIndex)
{
  defines.resizeLights(lightIndex);
  defines.hemilights[lightIndex] = true;
}

} // end of namespace BABYLON
