#include <babylon/inspector/properties/properties_view.h>

#include <babylon/math/color3.h>

#include <babylon/inspector/properties/bool_property.h>
#include <babylon/inspector/properties/color3_property.h>
#include <babylon/inspector/properties/float_property.h>
#include <babylon/inspector/properties/string_property.h>

#include <imgui.h>

namespace BABYLON {

PropertiesView::PropertiesView()
{
}

PropertiesView::~PropertiesView()
{
}

void PropertiesView::addBoolProperty(const string_t& name,
                                     const TBoolGetter& getter,
                                     const TBoolSetter& setter)
{
  // Create property
  _boolProperties.emplace_back(BoolProperty{getter, setter});
  // Store mapping
  auto index = _boolProperties.size() - 1;
  auto type  = PropertyTypeInsp::BOOL_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addFloatProperty(const string_t& name,
                                      const TFloatGetter& getter,
                                      const TFloatSetter& setter)
{
  // Create property
  _floatProperties.emplace_back(FloatProperty{getter, setter});
  // Store mapping
  auto index = _floatProperties.size() - 1;
  auto type  = PropertyTypeInsp::FLOAT_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addStringProperty(const string_t& name,
                                       const TStringGetter& getter,
                                       const TStringSetter& setter)
{
  // Create property
  _stringProperties.emplace_back(StringProperty{getter, setter});
  // Store mapping
  auto index = _stringProperties.size() - 1;
  auto type  = PropertyTypeInsp::STRING_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::addColor3Property(const string_t& name,
                                       const TColor3Getter& getter,
                                       const TColor3Setter& setter)
{
  // Create property
  _color3Properties.emplace_back(Color3Property{getter, setter});
  // Store mapping
  auto index = _color3Properties.size() - 1;
  auto type  = PropertyTypeInsp::COLOR3_PROPERTY;
  _propertyEntries.emplace_back(PropertyEntry{name, index, type});
}

void PropertiesView::sortPropertiesByName()
{
  ::std::sort(_propertyEntries.begin(), _propertyEntries.end(),
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
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.59f, 0.4f, 1.0f));
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
      case PropertyTypeInsp::STRING_PROPERTY:
        _stringProperties[property.index].render();
        break;
      case PropertyTypeInsp::COLOR3_PROPERTY:
        _color3Properties[property.index].render();
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
