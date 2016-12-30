#include <babylon/lights/light.h>

#include <babylon/animations/animation.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/mesh/abstract_mesh.h>

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

Node::Type Light::type() const
{
  return Node::Type::LIGHT;
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

Light* Light::Parse(const Json::value& /*parsedLight*/, Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
