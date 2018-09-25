#ifndef BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
#define BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H

#include <babylon/babylon_api.h>

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
  COLOR4_PROPERTY                         = 6,
  VECTOR2_PROPERTY                        = 7,
  VECTOR3_PROPERTY                        = 8,
  COLOR_CURVES_PROPERTY                   = 9,
  IMAGE_PROCESSING_CONFIGURATION_PROPERTY = 10,
}; // end of enum class PropertyType

struct PropertyEntry {
  std::string name;
  size_t index;
  PropertyTypeInsp type;
}; // end of struct struct PropertyEntry

class BABYLON_SHARED_EXPORT PropertiesView {

public:
  PropertiesView();
  virtual ~PropertiesView();

  void addBoolProperty(const std::string& name,
                       const TPrimitiveGetter<bool>& getter,
                       const TPrimitiveSetter<bool>& setter);
  void addFloatProperty(const std::string& name,
                        const TPrimitiveGetter<float>& getter,
                        const TPrimitiveSetter<float>& setter);
  void addIntProperty(const std::string& name,
                      const TPrimitiveGetter<int>& getter,
                      const TPrimitiveSetter<int>& setter);
  void addSizeTProperty(const std::string& name,
                        const TPrimitiveGetter<size_t>& getter,
                        const TPrimitiveSetter<size_t>& setter);
  void addStringProperty(const std::string& name,
                         const TPrimitiveGetter<std::string>& getter,
                         const TPrimitiveSetter<std::string>& setter);
  void addColor3Property(const std::string& name,
                         const TBabylonGetter<Color3>& getter,
                         const TBabylonSetter<Color3>& setter);
  void addColor4Property(const std::string& name,
                         const TBabylonGetter<Color4>& getter,
                         const TBabylonSetter<Color4>& setter);
  void addVector2Property(const std::string& name,
                          const TBabylonGetter<Vector2>& getter,
                          const TBabylonSetter<Vector2>& setter);
  void addVector3Property(const std::string& name,
                          const TBabylonGetter<Vector3>& getter,
                          const TBabylonSetter<Vector3>& setter);
  void addColorCurvesProperty(
    const std::string& name,
    const TBabylonNonConstGetter<ColorCurves>& getterNonConst);
  void addImageProcessingConfigurationProperty(
    const std::string& name,
    const TBabylonNonConstGetter<ImageProcessingConfiguration>& getterNonConst);
  void sortPropertiesByName();

  void render();
  void dispose();

private:
  std::vector<PrimitiveProperty<bool>> _boolProperties;
  std::vector<PrimitiveProperty<float>> _floatProperties;
  std::vector<PrimitiveProperty<int>> _intProperties;
  std::vector<PrimitiveProperty<size_t>> _sizeTProperties;
  std::vector<PrimitiveProperty<std::string>> _stringProperties;
  std::vector<BabylonProperty<Color3>> _color3Properties;
  std::vector<BabylonProperty<Color4>> _color4Properties;
  std::vector<BabylonProperty<Vector2>> _vector2Properties;
  std::vector<BabylonProperty<Vector3>> _vector3Properties;
  std::vector<BabylonProperty<ColorCurves>> _colorCurvesProperties;
  std::vector<BabylonProperty<ImageProcessingConfiguration>>
    _imageProcessingConfigurationProperties;
  std::vector<PropertyEntry> _propertyEntries;

}; // end of class PropertiesView

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_PROPERTIES_VIEW_H
