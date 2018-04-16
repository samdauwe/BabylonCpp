#include <babylon/lights/light.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/serialization_helper.h>

namespace BABYLON {

Light::Light(const string_t& iName, Scene* scene)
    : Node{iName, scene}
    , diffuse{Color3(1.f, 1.f, 1.f)}
    , specular{Color3(1.f, 1.f, 1.f)}
    , intensity{1.f}
    , range{numeric_limits_t<float>::max()}
    , shadowEnabled{true}
    , _shadowGenerator{nullptr}
    , _uniformBuffer{::std::make_unique<UniformBuffer>(scene->getEngine())}
    , _photometricScale{1.f}
    , _intensityMode{Light::INTENSITYMODE_AUTOMATIC()}
    , _radius{0.00001f}
    , _renderPriority{0}
    , _includedOnlyMeshes{}
    , _excludedMeshes{}
    , _includeOnlyWithLayerMask{0}
    , _excludeWithLayerMask{0}
    , _lightmapMode{0}
    , _parentedWorldMatrix{nullptr}
    , _worldMatrix{::std::make_unique<Matrix>(Matrix::Identity())}
{
}

Light::~Light()
{
}

IReflect::Type Light::type() const
{
  return IReflect::Type::LIGHT;
}

void Light::addToScene(unique_ptr_t<Light>&& newLight)
{
  getScene()->addLight(::std::move(newLight));
}

const string_t Light::getClassName() const
{
  return "Light";
}

string_t Light::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  oss << ", type: ";
  const array_t<string_t, 4> types{
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

void Light::_buildUniformLayout()
{
}

void Light::setEnabled(bool value)
{
  Node::setEnabled(value);
  _resyncMeshes();
}

unsigned int Light::intensityMode() const
{
  return _intensityMode;
}

void Light::setIntensityMode(unsigned int value)
{
  _intensityMode = value;
  _computePhotometricScale();
}

float Light::radius() const
{
  return _radius;
}

void Light::setRadius(float value)
{
  _radius = value;
  _computePhotometricScale();
}

int Light::renderPriority() const
{
  return _renderPriority;
}

vector_t<AbstractMesh*>& Light::includedOnlyMeshes()
{
  return _includedOnlyMeshes;
}

void Light::setIncludedOnlyMeshes(const vector_t<AbstractMesh*>& value)
{
  _includedOnlyMeshes = value;
  _hookArrayForIncludedOnly(value);
}

vector_t<AbstractMesh*>& Light::excludedMeshes()
{
  return _excludedMeshes;
}

void Light::setExcludedMeshes(const vector_t<AbstractMesh*>& value)
{
  _excludedMeshes = value;
  _hookArrayForExcluded(value);
}

unsigned int Light::includeOnlyWithLayerMask() const
{
  return _includeOnlyWithLayerMask;
}

void Light::setIncludeOnlyWithLayerMask(unsigned int value)
{
  _includeOnlyWithLayerMask = value;
  _resyncMeshes();
}

unsigned int Light::excludeWithLayerMask() const
{
  return _excludeWithLayerMask;
}

void Light::setExcludeWithLayerMask(unsigned int value)
{
  _excludeWithLayerMask = value;
  _resyncMeshes();
}

unsigned int Light::lightmapMode() const
{
  return _lightmapMode;
}

void Light::setLightmapMode(unsigned int value)
{
  if (_lightmapMode == value) {
    return;
  }

  _lightmapMode = value;
  _markMeshesAsLightDirty();
}

ShadowGenerator* Light::getShadowGenerator()
{
  return _shadowGenerator;
}

Vector3 Light::getAbsolutePosition()
{
  return Vector3::Zero();
}

void Light::transferToEffect(Effect* /*effect*/, const string_t& /*lightIndex*/)
{
}

void Light::transferToEffect(Effect* /*effect*/,
                             const string_t& /*uniformName0*/,
                             const string_t& /*uniformName1*/)
{
}

bool Light::canAffectMesh(AbstractMesh* mesh)
{
  if (!mesh) {
    return true;
  }

  auto it1
    = ::std::find(_includedOnlyMeshes.begin(), _includedOnlyMeshes.end(), mesh);
  if (_includedOnlyMeshes.size() > 0 && it1 == _includedOnlyMeshes.end()) {
    return false;
  }

  auto it2 = ::std::find(_excludedMeshes.begin(), _excludedMeshes.end(), mesh);
  if (_excludedMeshes.size() > 0 && it2 != _excludedMeshes.end()) {
    return false;
  }

  if (_includeOnlyWithLayerMask != 0
      && (_includeOnlyWithLayerMask & mesh->layerMask()) == 0) {
    return false;
  }

  if (_excludeWithLayerMask != 0 && _excludeWithLayerMask & mesh->layerMask()) {
    return false;
  }

  return true;
}

Matrix* Light::getWorldMatrix()
{
  _currentRenderId = getScene()->getRenderId();
  _childRenderId   = _currentRenderId;

  auto worldMatrix = _getWorldMatrix();

  if (parent() && parent()->getWorldMatrix()) {
    if (!_parentedWorldMatrix) {
      _parentedWorldMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
    }

    worldMatrix->multiplyToRef(*parent()->getWorldMatrix(),
                               *_parentedWorldMatrix);

    _markSyncedWithParent();

    return _parentedWorldMatrix.get();
  }

  return worldMatrix;
}

int Light::CompareLightsPriority(Light* a, Light* b)
{
  // shadow-casting lights have priority over non-shadow-casting lights
  // the renderPrioirty is a secondary sort criterion
  if (a->shadowEnabled != b->shadowEnabled) {
    return (b->shadowEnabled ? 1 : 0) - (a->shadowEnabled ? 1 : 0);
  }
  return b->renderPriority() - a->renderPriority();
}

void Light::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  if (_shadowGenerator) {
    _shadowGenerator->dispose();
    _shadowGenerator = nullptr;
  }

  // Animations
  getScene()->stopAnimation(this);

  // Remove from meshes
  for (auto& mesh : getScene()->meshes) {
    mesh->_removeLightSource(this);
  }

  _uniformBuffer->dispose();

  // Remove from scene
  getScene()->removeLight(this);

  Node::dispose(doNotRecurse, disposeMaterialAndTextures);
}

unsigned int Light::getTypeID() const
{
  return 0;
}

float Light::getScaledIntensity() const
{
  return _photometricScale * intensity;
}

unique_ptr_t<Light> Light::clone(const string_t& /*name*/)
{
  return nullptr;
}

Json::object Light::serialize() const
{
  return Json::object();
}

Light* Light::GetConstructorFromName(unsigned int type, const string_t& name,
                                     Scene* scene)
{
  switch (type) {
    case Light::LIGHTTYPEID_POINTLIGHT():
      return PointLight::New(name, Vector3::Zero(), scene);
    case Light::LIGHTTYPEID_DIRECTIONALLIGHT():
      return DirectionalLight::New(name, Vector3::Zero(), scene);
    case Light::LIGHTTYPEID_SPOTLIGHT():
      return SpotLight::New(name, Vector3::Zero(), Vector3::Zero(), 0.f, 0.f,
                            scene);
    case Light::LIGHTTYPEID_HEMISPHERICLIGHT():
      return HemisphericLight::New(name, Vector3::Zero(), scene);
  }

  return nullptr;
}

Light* Light::Parse(const Json::value& parsedLight, Scene* scene)
{
  auto constructor = Light::GetConstructorFromName(
    Json::GetNumber(parsedLight, "type", 0u),
    Json::GetString(parsedLight, "name"), scene);
  if (!constructor) {
    return nullptr;
  }

  auto light = SerializationHelper::Parse(constructor, parsedLight, scene);
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
      light, Json::GetNumber(parsedLight, "autoAnimateFrom", 0),
      Json::GetNumber(parsedLight, "autoAnimateTo", 0),
      Json::GetBool(parsedLight, "autoAnimateLoop"),
      Json::GetNumber(parsedLight, "autoAnimateSpeed", 1.f));
  }

  return light;
}

void Light::_hookArrayForExcluded(const vector_t<AbstractMesh*>& /*array*/)
{
}

void Light::_hookArrayForIncludedOnly(const vector_t<AbstractMesh*>& /*array*/)
{
}

void Light::_resyncMeshes()
{
  for (auto& mesh : getScene()->meshes) {
    mesh->_resyncLighSource(this);
  }
}

void Light::_markMeshesAsLightDirty()
{
  for (auto& mesh : getScene()->meshes) {
    if (stl_util::contains(mesh->_lightSources, this)) {
      mesh->_markSubMeshesAsLightDirty();
    }
  }
}

void Light::_computePhotometricScale()
{
  _photometricScale = _getPhotometricScale();
  getScene()->resetCachedMaterial();
}

float Light::_getPhotometricScale()
{
  float photometricScale = 0.f;
  auto lightTypeID       = getTypeID();

  // get photometric mode
  auto photometricMode = intensityMode();
  if (photometricMode == Light::INTENSITYMODE_AUTOMATIC()) {
    if (lightTypeID == Light::LIGHTTYPEID_DIRECTIONALLIGHT()) {
      photometricMode = Light::INTENSITYMODE_ILLUMINANCE();
    }
    else {
      photometricMode = Light::INTENSITYMODE_LUMINOUSINTENSITY();
    }
  }

  // compute photometric scale
  switch (lightTypeID) {
    case Light::LIGHTTYPEID_POINTLIGHT():
    case Light::LIGHTTYPEID_SPOTLIGHT():
      switch (photometricMode) {
        case Light::INTENSITYMODE_LUMINOUSPOWER():
          photometricScale = 1.f / (4.f * Math::PI);
          break;
        case Light::INTENSITYMODE_LUMINOUSINTENSITY():
          photometricScale = 1.f;
          break;
        case Light::INTENSITYMODE_LUMINANCE():
          photometricScale = radius() * radius();
          break;
      }
      break;

    case Light::LIGHTTYPEID_DIRECTIONALLIGHT():
      switch (photometricMode) {
        case Light::INTENSITYMODE_ILLUMINANCE():
          photometricScale = 1.f;
          break;
        case Light::INTENSITYMODE_LUMINANCE():
          // When radius (and therefore solid angle) is non-zero a directional
          // lights brightness can be specified via central (peak) luminance.
          // For a directional light the 'radius' defines the angular radius (in
          // radians) rather than world-space radius (e.g. in metres).
          auto apexAngleRadians = radius();
          // Impose a minimum light angular size to avoid the light becoming an
          // infinitely small angular light source (i.e. a dirac delta
          // function).
          apexAngleRadians = ::std::max(apexAngleRadians, 0.001f);
          auto solidAngle
            = 2.f * Math::PI * (1.f - ::std::cos(apexAngleRadians));
          photometricScale = solidAngle;
          break;
      }
      break;

    case Light::LIGHTTYPEID_HEMISPHERICLIGHT():
      // No fall off in hemisperic light.
      photometricScale = 1.f;
      break;
  }
  return photometricScale;
}

void Light::_reorderLightsInScene()
{
  auto scene = getScene();
  if (_renderPriority != 0) {
    scene->requireLightSorting = true;
  }
  scene->sortLightsByPriority();
}

} // end of namespace BABYLON
