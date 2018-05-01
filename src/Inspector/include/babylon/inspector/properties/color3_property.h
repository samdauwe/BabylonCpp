#ifndef BABYLON_INSPECTOR_PROPERTIES_COLOR3_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_COLOR3_PROPERTY_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Color3Property {

public:
  Color3Property(const TColor3Getter& getter, const TColor3Setter& setter);
  Color3Property(const Color3Property& other) = delete;
  Color3Property(Color3Property&& other);
  virtual ~Color3Property();

  void render();
  void dispose();

public:
  string_t _id;
  array_t<float, 3> _colorArray;
  TColor3Getter _getter;
  TColor3Setter _setter;

}; // end of class Color3Property

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_COLOR3_PROPERTY_H
