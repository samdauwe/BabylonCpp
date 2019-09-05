#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_line_component.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TexturePropertyGridComponent {

  static void render(const BaseTexturePtr& texture,
                     TextureReservedDataStore& textureReservedDataStore)
  {
    static std::vector<std::pair<const char*, unsigned int>> samplingMode{
      {"Nearest", TextureConstants::NEAREST_NEAREST},
      {"Nearest & linear mip", TextureConstants::NEAREST_LINEAR},
      {"Linear", TextureConstants::LINEAR_LINEAR_MIPLINEAR}};

    static std::vector<std::pair<const char*, unsigned int>> coordinatesMode{
      {"Explicit", TextureConstants::EXPLICIT_MODE},
      {"Cubic", TextureConstants::CUBIC_MODE},
      {"Inverse cubic", TextureConstants::INVCUBIC_MODE},
      {"Equirectangular", TextureConstants::EQUIRECTANGULAR_MODE},
      {"Fixed equirectangular", TextureConstants::FIXED_EQUIRECTANGULAR_MODE},
      {"Fixed equirectangular mirrored",
       TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE},
      {"Planar", TextureConstants::PLANAR_MODE},
      {"Projection", TextureConstants::PROJECTION_MODE},
      {"Skybox", TextureConstants::SKYBOX_MODE},
      {"Spherical", TextureConstants::SPHERICAL_MODE},
    };

    // Cast to texture
    auto _texture = std::dynamic_pointer_cast<Texture>(texture);

    // --- PREVIEW ---
    static auto previewContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(previewContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("PREVIEW")) {
      TextureLineComponent::render(texture, textureReservedDataStore);
      previewContainerOpened = true;
    }
    else {
      previewContainerOpened = false;
    }
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Width",
                                std::to_string(texture->getSize().width));
      TextLineComponent::render("Height",
                                std::to_string(texture->getSize().height));
      TextLineComponent::render("Unique ID", std::to_string(texture->uniqueId));
      TextLineComponent::render("Class", texture->getClassName());
      TextLineComponent::render("Has alpha",
                                texture->hasAlpha() ? "Yes" : "No");
      TextLineComponent::render("Is 3D", texture->is3D ? "Yes" : "No");
      TextLineComponent::render("Is cube", texture->isCube ? "Yes" : "No");
      TextLineComponent::render("Is render target",
                                texture->isRenderTarget ? "Yes" : "No");
      auto sliderChange = SliderLineComponent::render(
        "UV set", static_cast<float>(texture->coordinatesIndex), 0.f, 3.f, 1.f,
        "%.3f");
      if (sliderChange) {
        texture->coordinatesIndex
          = static_cast<unsigned int>(sliderChange.value());
      }
      if (_texture) {
        auto optionChange = OptionsLineComponent::render(
          "Mode", _texture->coordinatesMode(), coordinatesMode);
        if (optionChange) {
          _texture->updateSamplingMode(optionChange.value());
        }
      }
      sliderChange = SliderLineComponent::render("Level", texture->level, 0.f,
                                                 2.f, 0.01f, "%.2f");
      if (sliderChange) {
        texture->level = sliderChange.value();
      }
      if (_texture) {
        auto optionChange = OptionsLineComponent::render(
          "Sampling", _texture->samplingMode(), samplingMode);
        if (optionChange) {
          _texture->updateSamplingMode(optionChange.value());
        }
      }
    }
    else {
      generalContainerOpened = false;
    }
    // --- TRANSFORM ---
    static auto transformContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORM")) {
      if (!texture->isCube && _texture) {
        auto valueChange
          = FloatLineComponent::render("U offset", _texture->uOffset);
        if (valueChange) {
          _texture->uOffset = valueChange.value();
        }
        valueChange = FloatLineComponent::render("V offset", _texture->vOffset);
        if (valueChange) {
          _texture->vOffset = valueChange.value();
        }
        valueChange = FloatLineComponent::render("U scale", _texture->uScale);
        if (valueChange) {
          _texture->uScale = valueChange.value();
        }
        valueChange = FloatLineComponent::render("V scale", _texture->vScale);
        if (valueChange) {
          _texture->vScale = valueChange.value();
        }
        valueChange = FloatLineComponent::render("U angle", _texture->uAng);
        if (valueChange) {
          _texture->uAng = valueChange.value();
        }
        valueChange = FloatLineComponent::render("V angle", _texture->vAng);
        if (valueChange) {
          _texture->vAng = valueChange.value();
        }
        valueChange = FloatLineComponent::render("W angle", _texture->wAng);
        if (valueChange) {
          _texture->wAng = valueChange.value();
        }
        if (CheckBoxLineComponent::render(
              "Clamp U",
              _texture->wrapU == TextureConstants::CLAMP_ADDRESSMODE)) {
          if (_texture->wrapU == TextureConstants::CLAMP_ADDRESSMODE) {
            _texture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
          }
          else {
            _texture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
          }
        }
        if (CheckBoxLineComponent::render(
              "Clamp V",
              _texture->wrapV == TextureConstants::CLAMP_ADDRESSMODE)) {
          if (_texture->wrapV == TextureConstants::CLAMP_ADDRESSMODE) {
            _texture->wrapV = TextureConstants::WRAP_ADDRESSMODE;
          }
          else {
            _texture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
          }
        }
      }
      else {
        auto cubeTexture = std::static_pointer_cast<CubeTexture>(texture);
        if (cubeTexture) {
          auto sliderChange = SliderLineComponent::render(
            "Rotation Y", cubeTexture->rotationY(), 0.f, Math::PI2, 0.1f,
            "%.1f");
          if (sliderChange) {
            cubeTexture->rotationY = static_cast<float>(sliderChange.value());
          }
        }
        else {
          auto hdrCubeTexture
            = std::static_pointer_cast<HDRCubeTexture>(texture);
          if (hdrCubeTexture) {
            auto sliderChange = SliderLineComponent::render(
              "Rotation Y", hdrCubeTexture->rotationY(), 0, Math::PI2, 0.1f,
              "%.1f");
            if (sliderChange) {
              hdrCubeTexture->rotationY
                = static_cast<float>(sliderChange.value());
            }
          }
        }
      }
      transformContainerOpened = true;
    }
    else {
      transformContainerOpened = false;
    }
  }

}; // end of struct TexturePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H
