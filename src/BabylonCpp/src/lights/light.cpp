#include <babylon/lights/light.h>

#include <babylon/animations/animation.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/serialization_helper.h>

namespace BABYLON {

Light::Light(const std::string& iName, Scene* scene)
    : Node{iName, scene}
    , diffuse{Color3(1.f, 1.f, 1.f)}
    , specular{Color3(1.f, 1.f, 1.f)}
    , intensity{1.f}
    , range{std::numeric_limits<float>::max()}
    , includeOnlyWithLayerMask{0}
    , excludeWithLayerMask{0}
    , lightmapMode{0}
    , radius{0.00001f}
    , _shadowGenerator{nullptr}
    , _parentedWorldMatrix{nullptr}
    , _worldMatrix{std_util::make_unique<Matrix>(Matrix::Identity())}
{
}

Light::~Light()
{
}

IReflect::Type Light::type() const
{
  return IReflect::Type::LIGHT;
}

void Light::addToScene(std::unique_ptr<Light>&& newLight)
{
  getScene()->addLight(std::move(newLight));
}

std::string Light::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  oss << ", type: ";
  const std::array<std::string, 4> types{
    {"Point", "Directional", "Spot", "Hemispheric"}};
  oss << (getTypeID() < types.size() ? types[getTypeID()] : "Unknown");
  if (!animations.empty()) {
    for (auto& animation : animations) {
      oss << ", animation[0]: " + animation->toString(fullDetails);
    }
  }
  if (fullDetails) {
  }
  return oss.str();
}

ShadowGenerator* Light::getShadowGenerator()
{
  return _shadowGenerator;
}

Vector3 Light::getAbsolutePosition()
{
  return Vector3::Zero();
}

void Light::transferToEffect(Effect* /*effect*/,
                             const std::string& /*uniformName0*/)
{
}

void Light::transferToEffect(Effect* /*effect*/,
                             const std::string& /*uniformName0*/,
                             const std::string& /*uniformName1*/)
{
}

Matrix* Light::_getWorldMatrix()
{

  return _worldMatrix.get();
}

bool Light::canAffectMesh(AbstractMesh* mesh)
{
  if (!mesh) {
    return true;
  }

  auto it1
    = std::find(includedOnlyMeshes.begin(), includedOnlyMeshes.end(), mesh);
  if (includedOnlyMeshes.size() > 0 && it1 == includedOnlyMeshes.end()) {
    return false;
  }

  auto it2 = std::find(excludedMeshes.begin(), excludedMeshes.end(), mesh);
  if (excludedMeshes.size() > 0 && it2 != excludedMeshes.end()) {
    return false;
  }

  if (includeOnlyWithLayerMask != 0
      && (includeOnlyWithLayerMask & mesh->layerMask) == 0) {
    return false;
  }

  if (excludeWithLayerMask != 0 && excludeWithLayerMask & mesh->layerMask) {
    return false;
  }

  return true;
}

Matrix* Light::getWorldMatrix()
{
  _currentRenderId = getScene()->getRenderId();

  Matrix* worldMatrix = _getWorldMatrix();

  if (parent() && parent()->getWorldMatrix()) {
    if (!_parentedWorldMatrix) {
      _parentedWorldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
    }

    worldMatrix->multiplyToRef(*parent()->getWorldMatrix(),
                               *_parentedWorldMatrix);

    _markSyncedWithParent();

    return _parentedWorldMatrix.get();
  }

  return worldMatrix;
}

void Light::dispose(bool /*doNotRecurse*/)
{
  if (_shadowGenerator) {
    _shadowGenerator->dispose();
    _shadowGenerator = nullptr;
  }

  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeLight(this);

  Node::dispose();
}

unsigned int Light::getTypeID() const
{
  return 0;
}

std::unique_ptr<Light> Light::clone(const std::string& /*name*/)
{
  return nullptr;
}

Json::object Light::serialize() const
{
  return Json::object();
}

Light* Light::GetConstructorFromName(unsigned int type, const std::string& name,
                                     Scene* scene)
{
  switch (type) {
    case 0:
      return PointLight::New(name, Vector3::Zero(), scene);
    case 1:
      return DirectionalLight::New(name, Vector3::Zero(), scene);
    case 2:
      return SpotLight::New(name, Vector3::Zero(), Vector3::Zero(), 0.f, 0.f,
                            scene);
    case 3:
      return HemisphericLight::New(name, Vector3::Zero(), scene);
  }

  return nullptr;
}

Light* Light::Parse(const Json::value& parsedLight, Scene* scene)
{
  auto light = SerializationHelper::Parse(
    Light::GetConstructorFromName(Json::GetNumber(parsedLight, "type", 0u),
                                  Json::GetString(parsedLight, "name"), scene),
    parsedLight, scene);
  if (!light) {
    return nullptr;
  }

  // Inclusion / exclusions
  if (parsedLight.contains("excludedMeshesIds")) {
    light->_excludedMeshesIds
      = Json::ToStringVector(parsedLight, "excludedMeshesIds");
  }

  if (parsedLight.contains("includedOnlyMeshesIds")) {
    light->_includedOnlyMeshesIds
      = Json::ToStringVector(parsedLight, "includedOnlyMeshesIds");
  }

  // Parent
  if (parsedLight.contains("parentId")) {
    light->_waitingParentId = Json::GetString(parsedLight, "parentId");
  }

  // Animations
  if (parsedLight.contains("animations")) {
    for (auto parsedAnimation : Json::GetArray(parsedLight, "animations")) {
      light->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(light, parsedLight, scene);
  }

  if (parsedLight.contains("autoAnimate")) {
    scene->beginAnimation(
      light, Json::GetNumber(parsedLight, "autoAnimateFrom", 0.f),
      Json::GetNumber(parsedLight, "autoAnimateTo", 0.f),
      Json::GetBool(parsedLight, "autoAnimateLoop"),
      Json::GetNumber(parsedLight, "autoAnimateSpeed", 1.f));
  }

  return light;
}

} // end of namespace BABYLON
