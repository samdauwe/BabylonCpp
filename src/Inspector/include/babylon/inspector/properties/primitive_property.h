#ifndef BABYLON_INSPECTOR_PROPERTIES_PRIMITIVE_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_PRIMITIVE_PROPERTY_H

#include <babylon/babylon_api.h>

#include <babylon/inspector/properties/callbacks.h>
#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

template <typename T>
class BABYLON_SHARED_EXPORT PrimitiveProperty {

public:
  PrimitiveProperty(const TPrimitiveGetter<T>& getter,
                    const TPrimitiveSetter<T>& setter)
      : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
  {
  }

  PrimitiveProperty(const PrimitiveProperty& other)
      : _id{other._id}, _getter{other._getter}, _setter{other._setter}
  {
  }

  PrimitiveProperty(PrimitiveProperty&& other)
      : _id{std::move(other._id)}
      , _getter{std::move(other._getter)}
      , _setter{std::move(other._setter)}
  {
  }

  PrimitiveProperty& operator=(const PrimitiveProperty& other)
  {
    if (&other != this) {
      _id     = other._id;
      _getter = other._getter;
      _setter = other._setter;
    }

    return *this;
  }

  PrimitiveProperty& operator=(PrimitiveProperty&& other)
  {
    if (&other != this) {
      _id     = std::move(other._id);
      _getter = std::move(other._getter);
      _setter = std::move(other._setter);
    }

    return *this;
  }

  virtual ~PrimitiveProperty()
  {
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, bool>::value>::type render()
  {
    bool boolValue = _getter();
    if (ImGui::Checkbox("", &boolValue)) {
      _setter(boolValue);
    }
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

  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::string>::value>::type render()
  {
    std::string stringValue = _getter();
    char* cstr              = &stringValue[0u];
    if (ImGui::InputText("", cstr, stringValue.size())) {
    }
  }

  void dispose()
  {
  }

public:
  std::string _id;
  TPrimitiveGetter<T> _getter;
  TPrimitiveSetter<T> _setter;

}; // end of class PrimitiveProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_PRIMITIVE_PROPERTY_H
