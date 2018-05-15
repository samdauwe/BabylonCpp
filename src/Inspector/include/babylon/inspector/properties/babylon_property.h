#ifndef BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/geometry.h>

#include <babylon/inspector/properties/callbacks.h>

#include <babylon/imgui/imgui_utils.h>
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

  template <class U                                                 = T,
            typename std::enable_if<std::is_same<
              Vector2, typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    const auto& vector = _getter();
    bool changed       = false;
    float x = 0.f, y = 0.f;
    if (ImGui::TreeNode(_id.c_str(), "x:%.3f, y:%.3f", vector.x, vector.y)) {
      // Vector2 properties
      {
        // x
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("x");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        x = vector.x;
        if (ImGui::InputFloat(" ", &x)) {
          changed = true;
        }
        // y
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("y");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        y = vector.y;
        if (ImGui::InputFloat(" ", &y)) {
          changed = true;
        }
      }
      // Update
      if (changed) {
        _setter(Vector2{x, y});
      }
      ImGui::TreePop();
    }
  }

  template <class U                                                 = T,
            typename std::enable_if<std::is_same<
              Vector3, typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    const auto& vector = _getter();
    bool isNonUniform = false, changed = false;
    float x = 0.f, y = 0.f, z = 0.f;
    if (ImGui::TreeNode(_id.c_str(), "x:%.3f, y:%.3f, z:%.3f", vector.x,
                        vector.y, vector.z)) {
      // Vector3 properties
      {
        // isNonUniform
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("isNonUniform");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        isNonUniform = vector.isNonUniform();
        if (ImGui::Checkbox("", &isNonUniform)) {
        }
        // x
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("x");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        x = vector.x;
        if (ImGui::InputFloat(" ", &x)) {
          changed = true;
        }
        // y
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("y");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        y = vector.y;
        if (ImGui::InputFloat(" ", &y)) {
          changed = true;
        }
        // z
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("z");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        z = vector.z;
        if (ImGui::InputFloat(" ", &z)) {
          changed = true;
        }
      }
      // Update
      if (changed) {
        _setter(Vector3{x, y, z});
      }
      ImGui::TreePop();
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
