#include <babylon/inspector/adapters/light_adapter.h>

#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/light.h>

#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/checkbox.h>

namespace BABYLON {

LightAdapter::LightAdapter(Light* light)
    : _light{light}, _hemispericLight{nullptr}
{
  _tools.emplace_back(::std::make_unique<Checkbox>(this));
  // Cast light
  if (_light->type() == IReflect::Type::HEMISPHERICLIGHT) {
    _hemispericLight = static_cast<HemisphericLight*>(light);
  }
  // Build properties view
  _properties = ::std::make_unique<PropertiesView>();
  _buildPropertiesView();
}

LightAdapter::~LightAdapter()
{
}

string_t LightAdapter::id()
{
  return _light->name;
}

string_t LightAdapter::type()
{
  return _light->getClassName();
}

unique_ptr_t<PropertiesView>& LightAdapter::getProperties()
{
  return _properties;
}

vector_t<unique_ptr_t<AbstractTreeTool>>& LightAdapter::getTools()
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
  // Node properties
  // - id
  view.addStringProperty("id", [&]() -> string_t& { return _light->id; },
                         [&](const string_t& value) { _light->id = value; });
  // - name
  view.addStringProperty("name", [&]() -> string_t& { return _light->name; },
                         [&](const string_t& value) { _light->name = value; });
  // - doNotSerialize
  view.addBoolProperty(
    "doNotSerialize", [&]() -> bool { return _light->doNotSerialize; },
    [&](const bool& value) { _light->doNotSerialize = value; });
  // - intensity
  view.addFloatProperty("intensity",
                        [&]() -> float { return _light->intensity; },
                        [&](const float& value) { _light->intensity = value; });
  // - radius
  view.addFloatProperty("radius", [&]() -> float { return _light->radius(); },
                        [&](const float& value) { _light->setRadius(value); });
  // - range
  view.addFloatProperty("range", [&]() -> float { return _light->range; },
                        [&](const float& value) { _light->range = value; });
  // Light properties
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
    "specular", [&]() -> Color3& { return _light->specular; },
    [&](const Color3& value) { _light->specular = value; });
  // HemisphericLight properties
  if (_hemispericLight) {
    // - ground color
    view.addColor3Property(
      "groundColor", [&]() -> Color3& { return _hemispericLight->groundColor; },
      [&](const Color3& value) { _hemispericLight->groundColor = value; });
  }
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

} // end of namespace BABYLON
