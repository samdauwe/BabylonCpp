#include <babylon/inspector/adapters/material_adapter.h>

#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>

#include <babylon/inspector/properties/properties_view.h>

namespace BABYLON {

MaterialAdapter::MaterialAdapter(Material* material)
    : _material{material}, _standardMaterial{nullptr}, _properties{nullptr}
{
  // Cast material
  if (_material->type() == IReflect::Type::STANDARDMATERIAL) {
    _standardMaterial = static_cast<StandardMaterial*>(_material);
  }
  // Build properties view
  _properties = ::std::make_unique<PropertiesView>();
  _buildPropertiesView();
}

MaterialAdapter::~MaterialAdapter()
{
}

string_t MaterialAdapter::id()
{
  return _material->name;
}

string_t MaterialAdapter::type()
{
  return _material->getClassName();
}

unique_ptr_t<PropertiesView>& MaterialAdapter::getProperties()
{
  return _properties;
}

vector_t<unique_ptr_t<AbstractTreeTool>>& MaterialAdapter::getTools()
{
  return _tools;
}

void MaterialAdapter::_buildPropertiesView()
{
  // -- Add properties -- //
  auto& view = *_properties;
  /** Material properties **/
  // - id
  view.addStringProperty("id", [&]() -> string_t& { return _material->id; },
                         [&](const string_t& value) { _material->id = value; });
  // - name
  view.addStringProperty(
    "name", [&]() -> string_t& { return _material->name; },
    [&](const string_t& value) { _material->name = value; });
  // - checkReadyOnEveryCall
  view.addBoolProperty(
    "checkReadyOnEveryCall",
    [&]() -> bool { return _material->checkReadyOnEveryCall; },
    [&](const bool& value) { _material->checkReadyOnEveryCall = value; });
  // - checkReadyOnlyOnce
  view.addBoolProperty(
    "checkReadyOnlyOnce",
    [&]() -> bool { return _material->checkReadyOnlyOnce; },
    [&](const bool& value) { _material->checkReadyOnlyOnce = value; });
  // - disableDepthWrite
  view.addBoolProperty(
    "disableDepthWrite", [&]() -> bool { return _material->disableDepthWrite; },
    [&](const bool& value) { _material->disableDepthWrite = value; });
  // - doNotSerialize
  view.addBoolProperty(
    "doNotSerialize", [&]() -> bool { return _material->doNotSerialize; },
    [&](const bool& value) { _material->doNotSerialize = value; });
  // - forceDepthWrite
  view.addBoolProperty(
    "forceDepthWrite", [&]() -> bool { return _material->forceDepthWrite; },
    [&](const bool& value) { _material->forceDepthWrite = value; });
  // - pointSize
  view.addFloatProperty(
    "radius", [&]() -> float { return _material->pointSize; },
    [&](const float& value) { _material->pointSize = value; });
  // - separateCullingPass
  view.addBoolProperty(
    "separateCullingPass",
    [&]() -> bool { return _material->separateCullingPass; },
    [&](const bool& value) { _material->separateCullingPass = value; });
  // - sideOrientation
  view.addIntProperty(
    "sideOrientation", [&]() -> int { return _material->sideOrientation; },
    [&](const int& value) { _material->sideOrientation = value; });
  // - storeEffectOnSubMeshes
  view.addBoolProperty(
    "storeEffectOnSubMeshes",
    [&]() -> bool { return _material->storeEffectOnSubMeshes; },
    [&](const bool& value) { _material->storeEffectOnSubMeshes = value; });
  // - zOffset
  view.addFloatProperty(
    "zOffset", [&]() -> float { return _material->zOffset; },
    [&](const float& value) { _material->zOffset = value; });
  /** StandardMaterial properties **/
  if (_standardMaterial) {
    // - ambientColor
    view.addColor3Property(
      "ambientColor",
      [&]() -> Color3 const& { return _standardMaterial->ambientColor; },
      [&](const Color3& value) { _standardMaterial->ambientColor = value; });
    // - diffuseColor
    view.addColor3Property(
      "diffuseColor",
      [&]() -> Color3 const& { return _standardMaterial->diffuseColor; },
      [&](const Color3& value) { _standardMaterial->diffuseColor = value; });
    // - emissiveColor
    view.addColor3Property(
      "emissiveColor",
      [&]() -> Color3 const& { return _standardMaterial->emissiveColor; },
      [&](const Color3& value) { _standardMaterial->emissiveColor = value; });
    // - indexOfRefraction
    view.addFloatProperty(
      "indexOfRefraction",
      [&]() -> float { return _standardMaterial->indexOfRefraction; },
      [&](const float& value) {
        _standardMaterial->indexOfRefraction = value;
      });
    // - parallaxScaleBias
    view.addFloatProperty(
      "parallaxScaleBias",
      [&]() -> float { return _standardMaterial->parallaxScaleBias; },
      [&](const float& value) {
        _standardMaterial->parallaxScaleBias = value;
      });
    // - separateCullingPass
    view.addBoolProperty(
      "invertRefractionY",
      [&]() -> bool { return _standardMaterial->invertRefractionY; },
      [&](const bool& value) { _standardMaterial->invertRefractionY = value; });
    // - specularColor
    view.addColor3Property(
      "specularColor",
      [&]() -> Color3 const& { return _standardMaterial->specularColor; },
      [&](const Color3& value) { _standardMaterial->specularColor = value; });
    // - specularPower
    view.addFloatProperty(
      "specularPower",
      [&]() -> float { return _standardMaterial->specularPower; },
      [&](const float& value) { _standardMaterial->specularPower = value; });
  }
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

} // end of namespace BABYLON
