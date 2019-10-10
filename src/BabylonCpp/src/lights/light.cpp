#include <babylon/lights/light.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/serialization_helper.h>

namespace BABYLON {

Light::Light(const std::string& iName, Scene* scene)
    : Node{iName, scene}
    , diffuse{Color3(1.f, 1.f, 1.f)}
    , specular{Color3(1.f, 1.f, 1.f)}
    , range{this, &Light::get_range, &Light::set_range}
    , falloffType{Light::FALLOFF_DEFAULT}
    , intensity{1.f}
    , _shadowGenerator{nullptr}
    , _uniformBuffer{std::make_unique<UniformBuffer>(scene->getEngine())}
    , intensityMode{this, &Light::get_intensityMode, &Light::set_intensityMode}
    , radius{this, &Light::get_radius, &Light::set_radius}
    , shadowEnabled{this, &Light::get_shadowEnabled, &Light::set_shadowEnabled}
    , renderPriority{this, &Light::get_renderPriority,
                     &Light::set_renderPriority}
    , includedOnlyMeshes{this, &Light::get_includedOnlyMeshes,
                         &Light::set_includedOnlyMeshes}
    , excludedMeshes{this, &Light::get_excludedMeshes,
                     &Light::set_excludedMeshes}
    , excludeWithLayerMask{this, &Light::get_excludeWithLayerMask,
                           &Light::set_excludeWithLayerMask}
    , includeOnlyWithLayerMask{this, &Light::get_includeOnlyWithLayerMask,
                               &Light::set_includeOnlyWithLayerMask}
    , lightmapMode{this, &Light::get_lightmapMode, &Light::set_lightmapMode}
    , _isLight{true}
    , _inverseSquaredRange{0.f}
    , _range{std::numeric_limits<float>::max()}
    , _photometricScale{1.f}
    , _intensityMode{Light::INTENSITYMODE_AUTOMATIC}
    , _radius{0.00001f}
    , _renderPriority{0}
    , _shadowEnabled{true}
    , _includedOnlyMeshes{}
    , _excludedMeshes{}
    , _includeOnlyWithLayerMask{0}
    , _excludeWithLayerMask{0}
    , _lightmapMode{0}
{
}

Light::~Light()
{
}

Type Light::type() const
{
  return Type::LIGHT;
}

AnimationValue
Light::getProperty(const std::vector<std::string>& targetPropertyPath)
{
  if (targetPropertyPath.size() == 1) {
    const auto& target = targetPropertyPath[0];
    if (target == "intensity") {
      return intensity;
    }
  }

  return AnimationValue();
}

void Light::setProperty(const std::vector<std::string>& targetPropertyPath,
                        const AnimationValue& value)
{
  const auto animationType = value.animationType();
  if (animationType.has_value()) {
    if (targetPropertyPath.size() == 1) {
      const auto& target = targetPropertyPath[0];
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT()) {
        auto floatValue = value.get<float>();
        if (target == "intensity") {
          intensity = floatValue;
        }
      }
    }
  }
}

void Light::addToScene(const LightPtr& newLight)
{
  getScene()->addLight(newLight); // Need to add light first to the scene!
  newLight->addToRootNodes();
  newLight->_buildUniformLayout();
  newLight->_resyncMeshes();
}

LightPtr Light::_this() const
{
  const auto& lights = getScene()->lights;
  auto it
    = std::find_if(lights.begin(), lights.end(), [this](const LightPtr& light) {
        return light.get() == this;
      });
  return (it != lights.end()) ? (*it) : nullptr;
}

const std::string Light::getClassName() const
{
  return "Light";
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

void Light::_buildUniformLayout()
{
}

void Light::_syncParentEnabledState()
{
  Node::_syncParentEnabledState();
  _resyncMeshes();
}

void Light::setEnabled(bool value)
{
  Node::setEnabled(value);
  _resyncMeshes();
}

float Light::get_range() const
{
  return _range;
}

void Light::set_range(float value)
{
  _range               = value;
  _inverseSquaredRange = 1.f / (range * range);
}

unsigned int Light::get_intensityMode() const
{
  return _intensityMode;
}

void Light::set_intensityMode(unsigned int value)
{
  _intensityMode = value;
  _computePhotometricScale();
}

float Light::get_radius() const
{
  return _radius;
}

void Light::set_radius(float value)
{
  _radius = value;
  _computePhotometricScale();
}

bool Light::get_shadowEnabled() const
{
  return _shadowEnabled;
}

void Light::set_shadowEnabled(bool value)
{
  if (_shadowEnabled == value) {
    return;
  }

  _shadowEnabled = value;
  _markMeshesAsLightDirty();
}

int Light::get_renderPriority() const
{
  return _renderPriority;
}

void Light::set_renderPriority(int value)
{
  _renderPriority = value;
  _reorderLightsInScene();
}

std::vector<AbstractMeshPtr>& Light::get_includedOnlyMeshes()
{
  return _includedOnlyMeshes;
}

void Light::set_includedOnlyMeshes(const std::vector<AbstractMeshPtr>& value)
{
  _includedOnlyMeshes = value;
  _hookArrayForIncludedOnly(value);
}

std::vector<AbstractMeshPtr>& Light::get_excludedMeshes()
{
  return _excludedMeshes;
}

void Light::set_excludedMeshes(const std::vector<AbstractMeshPtr>& value)
{
  _excludedMeshes = value;
  _hookArrayForExcluded(value);
}

unsigned int Light::get_excludeWithLayerMask() const
{
  return _excludeWithLayerMask;
}

void Light::set_excludeWithLayerMask(unsigned int value)
{
  _excludeWithLayerMask = value;
  _resyncMeshes();
}

unsigned int Light::get_includeOnlyWithLayerMask() const
{
  return _includeOnlyWithLayerMask;
}

void Light::set_includeOnlyWithLayerMask(unsigned int value)
{
  _includeOnlyWithLayerMask = value;
  _resyncMeshes();
}

unsigned int Light::get_lightmapMode() const
{
  return _lightmapMode;
}

void Light::set_lightmapMode(unsigned int value)
{
  if (_lightmapMode == value) {
    return;
  }

  _lightmapMode = value;
  _markMeshesAsLightDirty();
}

IShadowGeneratorPtr Light::getShadowGenerator()
{
  return _shadowGenerator;
}

Vector3 Light::getAbsolutePosition()
{
  return Vector3::Zero();
}

void Light::transferToEffect(const EffectPtr& /*effect*/,
                             const std::string& /*lightIndex*/)
{
}

void Light::transferToEffect(const EffectPtr& /*effect*/,
                             const std::string& /*uniformName0*/,
                             const std::string& /*uniformName1*/)
{
}

bool Light::canAffectMesh(AbstractMesh* mesh)
{
  if (!mesh) {
    return true;
  }

  auto it1
    = std::find_if(_includedOnlyMeshes.begin(), _includedOnlyMeshes.end(),
                   [mesh](const AbstractMeshPtr& includedOnlyMesh) {
                     return includedOnlyMesh.get() == mesh;
                   });
  if (_includedOnlyMeshes.size() > 0 && it1 == _includedOnlyMeshes.end()) {
    return false;
  }

  auto it2 = std::find_if(_excludedMeshes.begin(), _excludedMeshes.end(),
                          [mesh](const AbstractMeshPtr& excludedMesh) {
                            return excludedMesh.get() == mesh;
                          });
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
    mesh->_removeLightSource(this, true);
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

std::unique_ptr<Light> Light::clone(const std::string& /*name*/)
{
  return nullptr;
}

json Light::serialize() const
{
  return nullptr;
}

std::function<LightPtr()>
Light::GetConstructorFromName(unsigned int type, const std::string& iName,
                              Scene* scene)
{
  auto constructorFunc
    = Node::Construct("Light_Type_" + std::to_string(type), iName, scene);

  if (constructorFunc) {
    return [constructorFunc]() {
      return std::static_pointer_cast<Light>(constructorFunc());
    };
  }

  // Default to no light for none present once.
  return nullptr;
}

LightPtr Light::Parse(const json& parsedLight, Scene* scene)
{
  auto constructor = Light::GetConstructorFromName(
    json_util::get_number<unsigned>(parsedLight, "type"),
    json_util::get_string(parsedLight, "name"), scene);

  if (!constructor) {
    return nullptr;
  }

  auto light = SerializationHelper::Parse(constructor, parsedLight, scene);

  // Inclusion / exclusions
  if (json_util::has_valid_key_value(parsedLight, "excludedMeshesIds")) {
    light->_excludedMeshesIds
      = json_util::get_array<std::string>(parsedLight, "excludedMeshesIds");
  }

  if (json_util::has_valid_key_value(parsedLight, "includedOnlyMeshesIds")) {
    light->_includedOnlyMeshesIds
      = json_util::get_array<std::string>(parsedLight, "includedOnlyMeshesIds");
  }

  // Parent
  if (json_util::has_valid_key_value(parsedLight, "parentId")) {
    light->_waitingParentId = json_util::get_string(parsedLight, "parentId");
  }

  // Falloff
  if (json_util::has_valid_key_value(parsedLight, "falloffType")) {
    light->falloffType
      = json_util::get_number<unsigned int>(parsedLight, "falloffType");
  }

  // Lightmaps
  if (json_util::has_valid_key_value(parsedLight, "lightmapMode")) {
    light->lightmapMode
      = json_util::get_number<unsigned int>(parsedLight, "lightmapMode");
  }

  // Animations
  if (json_util::has_key(parsedLight, "animations")) {
    for (const auto& parsedAnimation :
         json_util::get_array<json>(parsedLight, "animations")) {
      light->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(*light, parsedLight, scene);
  }

  if (json_util::has_key(parsedLight, "autoAnimate")) {
    scene->beginAnimation(
      light, json_util::get_number(parsedLight, "autoAnimateFrom", 0.f),
      json_util::get_number(parsedLight, "autoAnimateTo", 0.f),
      json_util::get_bool(parsedLight, "autoAnimateLoop"),
      json_util::get_number(parsedLight, "autoAnimateSpeed", 1.f));
  }

  return light;
}

void Light::_hookArrayForExcluded(const std::vector<AbstractMeshPtr>& /*array*/)
{
}

void Light::_hookArrayForIncludedOnly(
  const std::vector<AbstractMeshPtr>& /*array*/)
{
}

void Light::_resyncMeshes()
{
  for (auto& mesh : getScene()->meshes) {
    mesh->_resyncLighSource(_this());
  }
}

void Light::_markMeshesAsLightDirty()
{
  for (auto& mesh : getScene()->meshes) {
    if (std::find_if(
          mesh->lightSources().begin(), mesh->lightSources().end(),
          [this](const LightPtr& light) { return light.get() == this; })
        != mesh->lightSources().end()) {
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
  if (photometricMode == Light::INTENSITYMODE_AUTOMATIC) {
    if (lightTypeID == Light::LIGHTTYPEID_DIRECTIONALLIGHT) {
      photometricMode = Light::INTENSITYMODE_ILLUMINANCE;
    }
    else {
      photometricMode = Light::INTENSITYMODE_LUMINOUSINTENSITY;
    }
  }

  // compute photometric scale
  switch (lightTypeID) {
    case Light::LIGHTTYPEID_POINTLIGHT:
    case Light::LIGHTTYPEID_SPOTLIGHT:
      switch (photometricMode) {
        case Light::INTENSITYMODE_LUMINOUSPOWER:
          photometricScale = 1.f / (4.f * Math::PI);
          break;
        case Light::INTENSITYMODE_LUMINOUSINTENSITY:
          photometricScale = 1.f;
          break;
        case Light::INTENSITYMODE_LUMINANCE:
          photometricScale = radius() * radius();
          break;
      }
      break;

    case Light::LIGHTTYPEID_DIRECTIONALLIGHT:
      switch (photometricMode) {
        case Light::INTENSITYMODE_ILLUMINANCE:
          photometricScale = 1.f;
          break;
        case Light::INTENSITYMODE_LUMINANCE:
          // When radius (and therefore solid angle) is non-zero a directional
          // lights brightness can be specified via central (peak) luminance.
          // For a directional light the 'radius' defines the angular radius
          // (in radians) rather than world-space radius (e.g. in metres).
          auto apexAngleRadians = radius();
          // Impose a minimum light angular size to avoid the light becoming
          // an infinitely small angular light source (i.e. a dirac delta
          // function).
          apexAngleRadians = std::max(apexAngleRadians, 0.001f);
          auto solidAngle = 2.f * Math::PI * (1.f - std::cos(apexAngleRadians));
          photometricScale = solidAngle;
          break;
      }
      break;

    case Light::LIGHTTYPEID_HEMISPHERICLIGHT:
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
