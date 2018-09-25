#ifndef BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H
#define BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H

#include <babylon/babylon_api.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/image_processing_configuration.h>
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
  BabylonProperty(const TBabylonNonConstGetter<T>& getterNonConst)
      : _id{Geometry::RandomId()}
      , _getter{nullptr}
      , _getterNonConst{getterNonConst}
      , _setter{nullptr}
  {
  }

  BabylonProperty(const TBabylonGetter<T>& getter,
                  const TBabylonSetter<T>& setter)
      : _id{Geometry::RandomId()}
      , _getter{getter}
      , _getterNonConst{nullptr}
      , _setter{setter}
  {
  }

  BabylonProperty(const BabylonProperty& other)
      : _id{other._id}
      , _getter{other._getter}
      , _getterNonConst{other._getterNonConst}
      , _setter{other._setter}
  {
  }

  BabylonProperty(BabylonProperty&& other)
      : _id{std::move(other._id)}
      , _getter{std::move(other._getter)}
      , _getterNonConst{std::move(other._getterNonConst)}
      , _setter{std::move(other._setter)}
  {
  }

  BabylonProperty& operator=(const BabylonProperty& other)
  {
    if (&other != this) {
      _id             = other._id;
      _getter         = other._getter;
      _getterNonConst = other._getterNonConst;
      _setter         = other._setter;
    }

    return *this;
  }

  BabylonProperty& operator=(BabylonProperty&& other)
  {
    if (&other != this) {
      _id             = std::move(other._id);
      _getter         = std::move(other._getter);
      _getterNonConst = std::move(other._getterNonConst);
      _setter         = std::move(other._setter);
    }

    return *this;
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
    std::array<float, 3> _colorArray{{color.r, color.g, color.b}};
    if (_render(_colorArray)) {
      _setter(Color3{_colorArray[0], _colorArray[1], _colorArray[2]});
    }
  }

  template <class U                                                = T,
            typename std::enable_if<std::is_same<
              Color4, typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    const auto& color = _getter();
    std::array<float, 4> _colorArray{{color.r, color.g, color.b, color.a}};
    if (_render(_colorArray)) {
      _setter(
        Color4{_colorArray[0], _colorArray[1], _colorArray[2], _colorArray[3]});
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
        int i = 0;
        // x
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("x");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        x = vector.x;
        if (ImGui::InputFloat(" ", &x)) {
          changed = true;
        }
        ImGui::PopID();
        // y
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("y");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        y = vector.y;
        if (ImGui::InputFloat(" ", &y)) {
          changed = true;
        }
        ImGui::PopID();
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
        int i = 0;
        // isNonUniform
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("isNonUniform");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        isNonUniform = vector.isNonUniform();
        if (ImGui::Checkbox("", &isNonUniform)) {
        }
        ImGui::PopID();
        // x
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("x");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        x = vector.x;
        if (ImGui::InputFloat(" ", &x)) {
          changed = true;
        }
        ImGui::PopID();
        // y
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("y");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        y = vector.y;
        if (ImGui::InputFloat(" ", &y)) {
          changed = true;
        }
        ImGui::PopID();
        // z
        ImGui::PushID(i++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("z");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        z = vector.z;
        if (ImGui::InputFloat(" ", &z)) {
          changed = true;
        }
        ImGui::PopID();
      }
      // Update
      if (changed) {
        _setter(Vector3{x, y, z});
      }
      ImGui::TreePop();
    }
  }

  template <
    class U                                                     = T,
    typename std::enable_if<std::is_same<
      ColorCurves, typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    auto& colorCurves = _getterNonConst();
    _render(colorCurves);
  }

  template <class U                                        = T,
            typename std::enable_if<std::is_same<
              ImageProcessingConfiguration,
              typename std::decay<U>::type>::value>::type* = nullptr>
  void render()
  {
    auto& imageProcessingConfiguration = _getterNonConst();
    _render(imageProcessingConfiguration);
  }

  void dispose()
  {
  }

private:
  bool _render(std::array<float, 3>& colorArray)
  {
    if (ImGui::ColorEdit3("", colorArray.data())) {
      return true;
    }
    return false;
  }

  bool _render(std::array<float, 4>& colorArray)
  {
    if (ImGui::ColorEdit4("", colorArray.data())) {
      return true;
    }
    return false;
  }

  void _render(ColorCurves& colorCurves)
  {
    using FloatProperty = Property<ColorCurves, float> ColorCurves::*;

    if (ImGui::TreeNode(_id.c_str(), "ColorCurves")) {
      // Set attribute names
      static std::array<std::string, 15> attributeNames{
        "globalDensity",        //
        "globalHue",            //
        "globalSaturation",     //
        "highlightsDensity",    //
        "highlightsExposure",   //
        "highlightsHue",        //
        "highlightsSaturation", //
        "midtonesDensity",      //
        "midtonesExposure",     //
        "midtonesHue",          //
        "midtonesSaturation",   //
        "shadowsDensity",       //
        "shadowsExposure",      //
        "shadowsHue",           //
        "shadowsSaturation",    //
      };
      // Set attributes references
      std::array<FloatProperty, 15> attributes{
        &ColorCurves::globalDensity,        //
        &ColorCurves::globalHue,            //
        &ColorCurves::globalSaturation,     //
        &ColorCurves::highlightsDensity,    //
        &ColorCurves::highlightsExposure,   //
        &ColorCurves::highlightsHue,        //
        &ColorCurves::highlightsSaturation, //
        &ColorCurves::midtonesDensity,      //
        &ColorCurves::midtonesExposure,     //
        &ColorCurves::midtonesHue,          //
        &ColorCurves::midtonesSaturation,   //
        &ColorCurves::shadowsDensity,       //
        &ColorCurves::shadowsExposure,      //
        &ColorCurves::shadowsHue,           //
        &ColorCurves::shadowsSaturation,    //
      };
      // Render attributes
      for (size_t i = 0; i < attributeNames.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped(attributeNames[i].c_str());
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        float propertyValue = colorCurves.*attributes[i];
        if (ImGui::InputFloat(" ", &propertyValue)) {
          colorCurves.*attributes[i] = propertyValue;
        }
        ImGui::PopID();
      }
      ImGui::TreePop();
    }
  }

  void _render(ImageProcessingConfiguration& ipc)
  {
    using FloatAttribute = float ImageProcessingConfiguration::*;
    using BoolProperty   = Property<ImageProcessingConfiguration, bool>
      ImageProcessingConfiguration::*;
    using FloatProperty = Property<ImageProcessingConfiguration, float>
      ImageProcessingConfiguration::*;

    if (ImGui::TreeNode(_id.c_str(), "ImageProcessingConfiguration")) {
      int id = 0;
      // applyByPostProcess
      {
        ImGui::PushID(id++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("applyByPostProcess");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        bool propertyValue = ipc.vignetteEnabled();
        if (ImGui::Checkbox("", &propertyValue)) {
          ipc.vignetteEnabled = propertyValue;
        }
        ImGui::PopID();
      }
      // colorCurves
      if (ipc.colorCurves) {
        auto& colorCurves = *ipc.colorCurves;
        _render(colorCurves);
      }
      {
        // Set attribute names
        std::array<std::string, 4> attributeNames{
          "colorCurvesEnabled",         //
          "colorGradingBGR",            //
          "colorGradingEnabled",        //
          "colorGradingWithGreenDepth", //
        };
        // Set attributes references
        std::array<BoolProperty, 4> attributes{
          &ImageProcessingConfiguration::colorCurvesEnabled,         //
          &ImageProcessingConfiguration::colorGradingBGR,            //
          &ImageProcessingConfiguration::colorGradingEnabled,        //
          &ImageProcessingConfiguration::colorGradingWithGreenDepth, //
        };
        // Render attributes
        for (size_t i = 0; i < attributeNames.size(); ++i) {
          ImGui::PushID(id++);
          ImGui::NextColumn();
          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
          ImGui::TextWrapped(attributeNames[i].c_str());
          ImGui::PopStyleColor();
          ImGui::NextColumn();
          bool propertyValue = ipc.*attributes[i];
          if (ImGui::Checkbox("", &propertyValue)) {
            ipc.*attributes[i] = propertyValue;
          }
          ImGui::PopID();
        }
      }
      {
        // Set attribute names
        std::array<std::string, 2> attributeNames{
          "contrast", //
          "exposure", //
        };
        // Set attributes references
        std::array<FloatProperty, 2> attributes{
          &ImageProcessingConfiguration::contrast, //
          &ImageProcessingConfiguration::exposure, //
        };
        // Render attributes
        for (size_t i = 0; i < attributeNames.size(); ++i) {
          ImGui::PushID(id++);
          ImGui::NextColumn();
          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
          ImGui::TextWrapped(attributeNames[i].c_str());
          ImGui::PopStyleColor();
          ImGui::NextColumn();
          float propertyValue = ipc.*attributes[i];
          if (ImGui::InputFloat(" ", &propertyValue)) {
            ipc.*attributes[i] = propertyValue;
          }
          ImGui::PopID();
        }
      }
      {
        // Set attribute names
        std::array<std::string, 2> attributeNames{
          "isEnabled",          //
          "toneMappingEnabled", //
        };
        // Set attributes references
        std::array<BoolProperty, 2> attributes{
          &ImageProcessingConfiguration::isEnabled,          //
          &ImageProcessingConfiguration::toneMappingEnabled, //
        };
        // Render attributes
        for (size_t i = 0; i < attributeNames.size(); ++i) {
          ImGui::PushID(id++);
          ImGui::NextColumn();
          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
          ImGui::TextWrapped(attributeNames[i].c_str());
          ImGui::PopStyleColor();
          ImGui::NextColumn();
          bool propertyValue = ipc.*attributes[i];
          if (ImGui::Checkbox("", &propertyValue)) {
            ipc.*attributes[i] = propertyValue;
          }
          ImGui::PopID();
        }
      }
      // vignetteBlendMode
      {
        ImGui::PushID(id++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("vignetteBlendMode");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        int value = static_cast<int>(ipc.vignetteBlendMode());
        if (ImGui::InputInt("", &value)) {
          if (value >= 0) {
            ipc.vignetteBlendMode = static_cast<size_t>(value);
          }
        }
        ImGui::PopID();
      }
      {
        // Set attribute names
        std::array<std::string, 3> attributeNames{
          "vignetteCameraFov", //
          "vignetteCentreX",   //
          "vignetteCentreY",   //
        };
        // Set attributes references
        std::array<FloatAttribute, 3> attributes{
          &ImageProcessingConfiguration::vignetteCameraFov, //
          &ImageProcessingConfiguration::vignetteCentreX,   //
          &ImageProcessingConfiguration::vignetteCentreY,   //
        };
        // Render attributes
        for (size_t i = 0; i < attributeNames.size(); ++i) {
          ImGui::PushID(id++);
          ImGui::NextColumn();
          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
          ImGui::TextWrapped(attributeNames[i].c_str());
          ImGui::PopStyleColor();
          ImGui::NextColumn();
          float propertyValue = ipc.*attributes[i];
          if (ImGui::InputFloat(" ", &propertyValue)) {
            ipc.*attributes[i] = propertyValue;
          }
          ImGui::PopID();
        }
      }
      // vignetteColor
      {
        ImGui::PushID(id++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("vignetteColor");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        const auto& color = ipc.vignetteColor;
        std::array<float, 4> _colorArray{{color.r, color.g, color.b, color.a}};
        if (_render(_colorArray)) {
          ipc.vignetteColor = Color4{_colorArray[0], _colorArray[1],
                                     _colorArray[2], _colorArray[3]};
        }
        ImGui::PopID();
      }
      {
        ImGui::PushID(id++);
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
        ImGui::TextWrapped("vignetteEnabled");
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        bool propertyValue = ipc.vignetteEnabled();
        if (ImGui::Checkbox("", &propertyValue)) {
          ipc.vignetteEnabled = propertyValue;
        }
        ImGui::PopID();
      }
      {
        // Set attribute names
        std::array<std::string, 2> attributeNames{
          "vignetteStretch", //
          "vignetteWeight",  //
        };
        // Set attributes references
        std::array<FloatAttribute, 2> attributes{
          &ImageProcessingConfiguration::vignetteStretch, //
          &ImageProcessingConfiguration::vignetteWeight,  //
        };
        // Render attributes
        for (size_t i = 0; i < attributeNames.size(); ++i) {
          ImGui::PushID(id++);
          ImGui::NextColumn();
          ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorTop());
          ImGui::TextWrapped(attributeNames[i].c_str());
          ImGui::PopStyleColor();
          ImGui::NextColumn();
          float propertyValue = ipc.*attributes[i];
          if (ImGui::InputFloat(" ", &propertyValue)) {
            ipc.*attributes[i] = propertyValue;
          }
          ImGui::PopID();
        }
      }

      ImGui::TreePop();
    }
  }

public:
  std::string _id;
  TBabylonGetter<T> _getter;
  TBabylonNonConstGetter<T> _getterNonConst;
  TBabylonSetter<T> _setter;
}; // end of class BabylonProperty

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_PROPERTIES_BABYLON_PROPERTY_H
