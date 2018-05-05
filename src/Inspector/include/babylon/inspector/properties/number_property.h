#ifndef BABYLON_INSPECTOR_PROPERTIES_NUMBER_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_NUMBER_PROPERTY_H

#include <babylon/babylon_global.h>

#include <babylon/inspector/properties/callbacks.h>
#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

template <typename T>
class BABYLON_SHARED_EXPORT NumberProperty {

public:
  NumberProperty(const TNumberGetter<T>& getter, const TNumberSetter<T>& setter)
      : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
  {
  }

  NumberProperty(const NumberProperty& other) = delete;

  NumberProperty(NumberProperty&& other)
      : _id{::std::move(other._id)}
      , _getter{::std::move(other._getter)}
      , _setter{::std::move(other._setter)}
  {
  }

  virtual ~NumberProperty()
  {
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, float>::value>::type render()
  {
    float value = _getter();
    if (ImGui::InputFloat("", &value)) {
      _setter(value);
    }
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, int>::value>::type render()
  {
    int value = _getter();
    if (ImGui::InputInt("", &value)) {
      _setter(value);
    }
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, size_t>::value>::type render()
  {
    int value = static_cast<int>(_getter());
    if (ImGui::InputInt("", &value)) {
      if (value >= 0) {
        _setter(static_cast<size_t>(value));
      }
    }
  }

  void dispose()
  {
  }

public:
  string_t _id;
  TNumberGetter<T> _getter;
  TNumberSetter<T> _setter;

}; // end of class NumberProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_NUMBER_PROPERTY_H
