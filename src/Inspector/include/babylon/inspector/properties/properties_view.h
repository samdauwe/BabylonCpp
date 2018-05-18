#ifndef BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
#define BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/babylon_property.h>
#include <babylon/inspector/properties/callbacks.h>
#include <babylon/inspector/properties/primitive_property.h>

namespace BABYLON {

enum class PropertyTypeInsp {
  BOOL_PROPERTY                           = 0,
  FLOAT_PROPERTY                          = 1,
  INT_PROPERTY                            = 2,
  SIZE_T_PROPERT                          = 3,
  STRING_PROPERTY                         = 4,
  COLOR3_PROPERTY                         = 5,
  VECTOR2_PROPERTY                        = 6,
  VECTOR3_PROPERTY                        = 7,
  COLOR_CURVES_PROPERTY                   = 8,
  IMAGE_PROCESSING_CONFIGURATION_PROPERTY = 9,
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

  void addBoolProperty(const string_t& name,
                       const TPrimitiveGetter<bool>& getter,
                       const TPrimitiveSetter<bool>& setter);
  void addFloatProperty(const string_t& name,
                        const TPrimitiveGetter<float>& getter,
                        const TPrimitiveSetter<float>& setter);
  void addIntProperty(const string_t& name, const TPrimitiveGetter<int>& getter,
                      const TPrimitiveSetter<int>& setter);
  void addSizeTProperty(const string_t& name,
                        const TPrimitiveGetter<size_t>& getter,
                        const TPrimitiveSetter<size_t>& setter);
  void addStringProperty(const string_t& name,
                         const TPrimitiveGetter<string_t>& getter,
                         const TPrimitiveSetter<string_t>& setter);
  void addColor3Property(const string_t& name,
                         const TBabylonGetter<Color3>& getter,
                         const TBabylonSetter<Color3>& setter);
  void addVector2Property(const string_t& name,
                          const TBabylonGetter<Vector2>& getter,
                          const TBabylonSetter<Vector2>& setter);
  void addVector3Property(const string_t& name,
                          const TBabylonGetter<Vector3>& getter,
                          const TBabylonSetter<Vector3>& setter);
  void addColorCurvesConfigurationProperty(
    const string_t& name, const TBabylonGetter<ColorCurves>& getter,
    const TBabylonSetter<ColorCurves>& setter);
  void addImageProcessingConfigurationProperty(
    const string_t& name,
    const TBabylonGetter<ImageProcessingConfiguration>& getter,
    const TBabylonSetter<ImageProcessingConfiguration>& setter);
  void sortPropertiesByName();

  void render();
  void dispose();

private:
  vector_t<PrimitiveProperty<bool>> _boolProperties;
  vector_t<PrimitiveProperty<float>> _floatProperties;
  vector_t<PrimitiveProperty<int>> _intProperties;
  vector_t<PrimitiveProperty<size_t>> _sizeTProperties;
  vector_t<PrimitiveProperty<string_t>> _stringProperties;
  vector_t<BabylonProperty<Color3>> _color3Properties;
  vector_t<BabylonProperty<Vector2>> _vector2Properties;
  vector_t<BabylonProperty<Vector3>> _vector3Properties;
  vector_t<BabylonProperty<ColorCurves>> _colorCurvesProperties;
  vector_t<BabylonProperty<ImageProcessingConfiguration>>
    _imageProcessingConfigurationProperties;
  vector_t<PropertyEntry> _propertyEntries;

}; // end of class PropertiesView

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
