#include <babylon/inspector/properties/properties_view.h>

#include <babylon/imgui/imgui_utils.h>
#include <imgui.h>

namespace BABYLON {

PropertiesView::PropertiesView()
{
}

PropertiesView::~PropertiesView()
{
}

void PropertiesView::addBoolProperty(const std::string& name,
                                     const TPrimitiveGetter<bool>& getter,
                                     const TPrimitiveSetter<bool>& setter)
{
  // Create property
  _boolProperties.emplace_back(PrimitiveProperty<bool>{getter, setter});
  // Store mapping
  auto index = _boolProperties.size() - 1;
  auto type  = PropertyTypeInsp::BOOL_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addFloatProperty(const std::string& name,
                                      const TPrimitiveGetter<float>& getter,
                                      const TPrimitiveSetter<float>& setter)
{
  // Create property
  _floatProperties.emplace_back(PrimitiveProperty<float>{getter, setter});
  // Store mapping
  auto index = _floatProperties.size() - 1;
  auto type  = PropertyTypeInsp::FLOAT_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addIntProperty(const std::string& name,
                                    const TPrimitiveGetter<int>& getter,
                                    const TPrimitiveSetter<int>& setter)
{
  // Create property
  _intProperties.emplace_back(PrimitiveProperty<int>{getter, setter});
  // Store mapping
  auto index = _intProperties.size() - 1;
  auto type  = PropertyTypeInsp::INT_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addSizeTProperty(const std::string& name,
                                      const TPrimitiveGetter<size_t>& getter,
                                      const TPrimitiveSetter<size_t>& setter)
{
  // Create property
  _sizeTProperties.emplace_back(PrimitiveProperty<size_t>{getter, setter});
  // Store mapping
  auto index = _sizeTProperties.size() - 1;
  auto type  = PropertyTypeInsp::SIZE_T_PROPERT;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addStringProperty(
  const std::string& name, const TPrimitiveGetter<std::string>& getter,
  const TPrimitiveSetter<std::string>& setter)
{
  // Create property
  _stringProperties.emplace_back(
    PrimitiveProperty<std::string>{getter, setter});
  // Store mapping
  auto index = _stringProperties.size() - 1;
  auto type  = PropertyTypeInsp::STRING_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addColor3Property(const std::string& name,
                                       const TBabylonGetter<Color3>& getter,
                                       const TBabylonSetter<Color3>& setter)
{
  // Create property
  _color3Properties.emplace_back(BabylonProperty<Color3>{getter, setter});
  // Store mapping
  auto index = _color3Properties.size() - 1;
  auto type  = PropertyTypeInsp::COLOR3_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addColor4Property(const std::string& name,
                                       const TBabylonGetter<Color4>& getter,
                                       const TBabylonSetter<Color4>& setter)
{
  // Create property
  _color4Properties.emplace_back(BabylonProperty<Color4>{getter, setter});
  // Store mapping
  auto index = _color4Properties.size() - 1;
  auto type  = PropertyTypeInsp::COLOR4_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addVector2Property(const std::string& name,
                                        const TBabylonGetter<Vector2>& getter,
                                        const TBabylonSetter<Vector2>& setter)
{
  // Create property
  _vector2Properties.emplace_back(BabylonProperty<Vector2>{getter, setter});
  // Store mapping
  auto index = _vector2Properties.size() - 1;
  auto type  = PropertyTypeInsp::VECTOR2_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addVector3Property(const std::string& name,
                                        const TBabylonGetter<Vector3>& getter,
                                        const TBabylonSetter<Vector3>& setter)
{
  // Create property
  _vector3Properties.emplace_back(BabylonProperty<Vector3>{getter, setter});
  // Store mapping
  auto index = _vector3Properties.size() - 1;
  auto type  = PropertyTypeInsp::VECTOR3_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addColorCurvesProperty(
  const std::string& name,
  const TBabylonNonConstGetter<ColorCurves>& getterNonConst)
{
  // Create property
  _colorCurvesProperties.emplace_back(
    BabylonProperty<ColorCurves>{getterNonConst});
  // Store mapping
  auto index = _colorCurvesProperties.size() - 1;
  auto type  = PropertyTypeInsp::COLOR_CURVES_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addImageProcessingConfigurationProperty(
  const std::string& name,
  const TBabylonNonConstGetter<ImageProcessingConfiguration>& getterNonConst)
{
  // Create property
  _imageProcessingConfigurationProperties.emplace_back(
    BabylonProperty<ImageProcessingConfiguration>{getterNonConst});
  // Store mapping
  auto index = _imageProcessingConfigurationProperties.size() - 1;
  auto type  = PropertyTypeInsp::IMAGE_PROCESSING_CONFIGURATION_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::sortPropertiesByName()
{
  std::sort(_propertyEntries.begin(), _propertyEntries.end(),
            [](const PropertyEntry& a, const PropertyEntry& b) {
              return a.name < b.name;
            });
}

void PropertiesView::render()
{
  ImGui::Columns(2, "properties");
  // Table header
  ImGui::TextWrapped("Name");
  ImGui::NextColumn();
  ImGui::TextWrapped("Value");
  ImGui::NextColumn();
  ImGui::Separator();
  int i = 0;
  for (auto& property : _propertyEntries) {
    // Render label
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
    ImGui::TextWrapped("%s", property.name.c_str());
    ImGui::PopStyleColor();
    ImGui::NextColumn();
    // Render value
    ImGui::PushID(i++);
    switch (property.type) {
      case PropertyTypeInsp::BOOL_PROPERTY:
        _boolProperties[property.index].render();
        break;
      case PropertyTypeInsp::FLOAT_PROPERTY:
        _floatProperties[property.index].render();
        break;
      case PropertyTypeInsp::INT_PROPERTY:
        _intProperties[property.index].render();
        break;
      case PropertyTypeInsp::SIZE_T_PROPERT:
        _sizeTProperties[property.index].render();
        break;
      case PropertyTypeInsp::STRING_PROPERTY:
        _stringProperties[property.index].render();
        break;
      case PropertyTypeInsp::COLOR3_PROPERTY:
        _color3Properties[property.index].render();
        break;
      case PropertyTypeInsp::COLOR4_PROPERTY:
        _color4Properties[property.index].render();
        break;
      case PropertyTypeInsp::VECTOR2_PROPERTY:
        _vector2Properties[property.index].render();
        break;
      case PropertyTypeInsp::VECTOR3_PROPERTY:
        _vector3Properties[property.index].render();
        break;
      case PropertyTypeInsp::COLOR_CURVES_PROPERTY:
        _colorCurvesProperties[property.index].render();
        break;
      case PropertyTypeInsp::IMAGE_PROCESSING_CONFIGURATION_PROPERTY:
        _imageProcessingConfigurationProperties[property.index].render();
        break;
    }
    ImGui::PopID();
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
}

void PropertiesView::dispose()
{
}

} // end of namespace BABYLON
