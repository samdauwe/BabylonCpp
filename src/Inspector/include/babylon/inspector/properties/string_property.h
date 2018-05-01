#ifndef BABYLON_INSPECTOR_PROPERTIES_STRING_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_STRING_PROPERTY_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StringProperty {

public:
  StringProperty(const TStringGetter& getter, const TStringSetter& setter);
  StringProperty(const StringProperty& other) = delete;
  StringProperty(StringProperty&& other);
  virtual ~StringProperty();

  void render();
  void dispose();

public:
  string_t _id;
  TStringGetter _getter;
  TStringSetter _setter;

}; // end of class StringProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_STRING_PROPERTY_H
