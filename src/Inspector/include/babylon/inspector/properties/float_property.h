#ifndef BABYLON_INSPECTOR_PROPERTIES_FLOAT_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_FLOAT_PROPERTY_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FloatProperty {

public:
  FloatProperty(const TFloatGetter& getter, const TFloatSetter& setter);
  FloatProperty(const FloatProperty& other) = delete;
  FloatProperty(FloatProperty&& other);
  virtual ~FloatProperty();

  void render();
  void dispose();

public:
  string_t _id;
  TFloatGetter _getter;
  TFloatSetter _setter;

}; // end of class FloatProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_FLOAT_PROPERTY_H
