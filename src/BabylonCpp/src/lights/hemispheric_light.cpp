#include <babylon/lights/hemispheric_light.h>

#include <babylon/core/json.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

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

Node::Type HemisphericLight::type() const
{
  return Node::Type::HEMISPHERICLIGHT;
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

void HemisphericLight::transferToEffect(
  Effect* effect, const std::string& directionUniformName,
  const std::string& groundColorUniformName)
{
  auto normalizeDirection = Vector3::Normalize(direction);
  effect->setFloat4(directionUniformName, normalizeDirection.x,
                    normalizeDirection.y, normalizeDirection.z, 0.f);
  effect->setColor3(groundColorUniformName, groundColor.scale(intensity));
}

Matrix* HemisphericLight::_getWorldMatrix()
{
  if (!_worldMatrix) {
    _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
  }

  return _worldMatrix.get();
}

unsigned int HemisphericLight::getTypeID() const
{
  return 3;
}

} // end of namespace BABYLON
