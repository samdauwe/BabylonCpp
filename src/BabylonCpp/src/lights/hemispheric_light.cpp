#include <babylon/lights/hemispheric_light.h>

#include <babylon/core/json.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

HemisphericLight::HemisphericLight(const std::string& iName, Scene* scene)
    : HemisphericLight(iName, Vector3::Up(), scene)
{
}

HemisphericLight::HemisphericLight(const std::string& iName,
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

const char* HemisphericLight::getClassName() const
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
                                        const std::string& lightIndex)
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

} // end of namespace BABYLON
