#ifndef BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>
#include <babylon/mesh/geometry.h>

#include <babylon/inspector/properties/callbacks.h>

#include <imgui.h>

namespace BABYLON {

template <typename T>
class BABYLON_SHARED_EXPORT BabylonProperty {

public:
  BabylonProperty(const TBabylonGetter<T>& getter,
                  const TBabylonSetter<T>& setter)
      : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
  {
  }

  BabylonProperty(const BabylonProperty& other) = delete;

  BabylonProperty(BabylonProperty&& other)
      : _id{::std::move(other._id)}
      , _getter{::std::move(other._getter)}
      , _setter{::std::move(other._setter)}
  {
  }

  virtual ~BabylonProperty()
  {
  }

  template <class U                                                = T,
            typename std::enable_if<std::is_same<
              Color3, typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    const auto& color = _getter();
    array_t<float, 3> _colorArray{{color.r, color.g, color.b}};
    if (ImGui::ColorEdit3("", _colorArray.data())) {
      _setter(Color3{_colorArray[0], _colorArray[1], _colorArray[2]});
    }
  }

  void dispose()
  {
  }

public:
  string_t _id;
  TBabylonGetter<T> _getter;
  TBabylonSetter<T> _setter;
}; // end of class BabylonProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H
