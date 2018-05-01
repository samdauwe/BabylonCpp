#ifndef BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
#define BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>

namespace BABYLON {

class BoolProperty;
class Color3Property;
class FloatProperty;
class StringProperty;

enum class PropertyTypeInsp {
  BOOL_PROPERTY   = 0,
  FLOAT_PROPERTY  = 1,
  STRING_PROPERTY = 2,
  COLOR3_PROPERTY = 3,
}; // end of enum class PropertyType

struct PropertyEntry {
  string_t name;
  size_t index;
  PropertyTypeInsp type;
}; // end of struct struct PropertyEntry

class BABYLON_SHARED_EXPORT PropertiesView {

public:
  PropertiesView();
  virtual ~PropertiesView();

  void addBoolProperty(const string_t& name, const TBoolGetter& getter,
                       const TBoolSetter& setter);
  void addFloatProperty(const string_t& name, const TFloatGetter& getter,
                        const TFloatSetter& setter);
  void addStringProperty(const string_t& name, const TStringGetter& getter,
                         const TStringSetter& setter);
  void addColor3Property(const string_t& name, const TColor3Getter& getter,
                         const TColor3Setter& setter);
  void sortPropertiesByName();

  void render();
  void dispose();

private:
  vector_t<BoolProperty> _boolProperties;
  vector_t<FloatProperty> _floatProperties;
  vector_t<Color3Property> _color3Properties;
  vector_t<StringProperty> _stringProperties;
  vector_t<PropertyEntry> _propertyEntries;

}; // end of class PropertiesView

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
