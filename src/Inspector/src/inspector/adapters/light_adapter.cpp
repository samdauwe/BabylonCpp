#include <babylon/inspector/adapters/light_adapter.h>

#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/light.h>

#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/checkbox.h>

namespace BABYLON {

LightAdapter::LightAdapter(const LightPtr& light)
    : _light{light}, _hemispericLight{nullptr}
{
  _tools.emplace_back(std::make_unique<Checkbox>(this));
  // Cast light
  if (_light->type() == IReflect::Type::HEMISPHERICLIGHT) {
    _hemispericLight = std::static_pointer_cast<HemisphericLight>(_light);
  }
  // Build properties view
  _properties = std::make_unique<PropertiesView>();
  _buildPropertiesView();
}

LightAdapter::~LightAdapter()
{
}

std::string LightAdapter::id()
{
  return _light->name;
}

std::string LightAdapter::type()
{
  return _light->getClassName();
}

std::unique_ptr<PropertiesView>& LightAdapter::getProperties()
{
  return _properties;
}

std::vector<std::unique_ptr<AbstractTreeTool>>& LightAdapter::getTools()
{
  return _tools;
}

void LightAdapter::setVisible(bool b)
{
  _light->setEnabled(b);
}

bool LightAdapter::isVisible() const
{
  return _light->isEnabled();
}

void LightAdapter::_buildPropertiesView()
{
  // -- Add properties -- //
  auto& view = *_properties;
  /** Node properties **/
  // - id
  view.addStringProperty("id", [&]() -> std::string& { return _light->id; },
                         [&](const std::string& value) { _light->id = value; });
  // - name
  view.addStringProperty("name", [&]() -> std::string& { return _light->name; },
                         [&](const std::string& value) { _light->name = value; });
  // - doNotSerialize
  view.addBoolProperty(
    "doNotSerialize", [&]() -> bool { return _light->doNotSerialize; },
    [&](const bool& value) { _light->doNotSerialize = value; });
  // - uniqueId
  view.addSizeTProperty("uniqueId",
                        [&]() -> size_t { return _light->uniqueId; },
                        [&](const size_t& value) { _light->uniqueId = value; });
  /** Light properties **/
  // - intensity
  view.addFloatProperty("intensity",
                        [&]() -> float { return _light->intensity; },
                        [&](const float& value) { _light->intensity = value; });
  // - radius
  view.addFloatProperty("radius", [&]() -> float { return _light->radius(); },
                        [&](const float& value) { _light->radius = value; });
  // - range
  view.addFloatProperty("range", [&]() -> float { return _light->range; },
                        [&](const float& value) { _light->range = value; });
  // - excludeWithLayerMask
  view.addSizeTProperty(
    "excludeWithLayerMask",
    [&]() -> size_t { return _light->excludeWithLayerMask; },
    [&](const size_t& value) {
      _light->excludeWithLayerMask = static_cast<unsigned>(value);
    });
  // - includeOnlyWithLayerMask
  view.addSizeTProperty(
    "includeOnlyWithLayerMask",
    [&]() -> size_t { return _light->includeOnlyWithLayerMask; },
    [&](const size_t& value) {
      _light->includeOnlyWithLayerMask = static_cast<unsigned>(value);
    });
  // - intensityMode
  view.addSizeTProperty("intensityMode",
                        [&]() -> size_t { return _light->intensityMode; },
                        [&](const size_t& value) {
                          _light->intensityMode = static_cast<unsigned>(value);
                        });
  // - lightmapMode
  view.addSizeTProperty("lightmapMode",
                        [&]() -> size_t { return _light->lightmapMode; },
                        [&](const size_t& value) {
                          _light->lightmapMode = static_cast<unsigned>(value);
                        });
  // renderPriority
  view.addSizeTProperty(
    "renderPriority", [&]() -> int { return _light->renderPriority; },
    [&](const int& value) { _light->renderPriority = value; });
  // - shadowEnabled
  view.addBoolProperty(
    "shadowEnabled", [&]() -> bool { return _light->shadowEnabled; },
    [&](const bool& value) { _light->shadowEnabled = value; });
  // - diffuse color
  view.addColor3Property("diffuse",
                         [&]() -> Color3& { return _light->diffuse; },
                         [&](const Color3& value) { _light->diffuse = value; });
  // - specular color
  view.addColor3Property(
    "specular", [&]() -> Color3 const& { return _light->specular; },
    [&](const Color3& value) { _light->specular = value; });
  /** HemisphericLight properties **/
  if (_hemispericLight) {
    // direction
    view.addVector3Property(
      "direction",
      [&]() -> Vector3 const& { return _hemispericLight->direction; },
      [&](const Vector3& value) { _hemispericLight->direction = value; });
    // - ground color
    view.addColor3Property(
      "groundColor",
      [&]() -> Color3 const& { return _hemispericLight->groundColor; },
      [&](const Color3& value) { _hemispericLight->groundColor = value; });
  }
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

} // end of namespace BABYLON
