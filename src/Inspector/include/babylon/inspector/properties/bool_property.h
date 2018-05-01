#ifndef BABYLON_INSPECTOR_PROPERTIES_BOOL_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_BOOL_PROPERTY_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoolProperty {

public:
  BoolProperty(const TBoolGetter& getter, const TBoolSetter& setter);
  BoolProperty(const BoolProperty& other) = delete;
  BoolProperty(BoolProperty&& other);
  virtual ~BoolProperty();

  void render();
  void dispose();

public:
  string_t _id;
  TBoolGetter _getter;
  TBoolSetter _setter;

}; // end of class BoolProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_BOOL_PROPERTY_H
