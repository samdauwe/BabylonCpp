#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINE_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/texture_reserved_data_store.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/postprocesses/pass_cube_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

class BaseTexture;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

struct BABYLON_SHARED_EXPORT TextureLineComponent {

  static void renderImage(const Uint8Array& /*imageData*/, int /*width*/,
                          int /*height*/)
  {
    // TODO IMPLEMENT
  }

  static void updatePreview(const BaseTexturePtr& texture,
                            TextureReservedDataStore& textureReservedDataStore)
  {
    auto scene  = texture->getScene();
    auto engine = scene->getEngine();
    auto size   = texture->getSize();
    auto ratio  = size.width / size.height;
    auto width  = textureReservedDataStore.width;
    auto height = width / (ratio > 0 ? static_cast<int>(ratio) : 1);

    PostProcessPtr passPostProcess = nullptr;

    if (!texture->isCube) {
      passPostProcess = PassPostProcess::New(
        "pass", 1.f, nullptr, TextureConstants::NEAREST_SAMPLINGMODE, engine,
        false, Constants::TEXTURETYPE_UNSIGNED_INT);
    }
    else {
      auto passCubePostProcess = PassCubePostProcess::New(
        "pass", 1.f, nullptr, TextureConstants::NEAREST_SAMPLINGMODE, engine,
        false, Constants::TEXTURETYPE_UNSIGNED_INT);
      passCubePostProcess->face = textureReservedDataStore.face;

      passPostProcess = passCubePostProcess;
    }

    if (!passPostProcess->getEffect()->isReady()) {
      // Try again later
      passPostProcess->dispose();
      return;
    }

    auto rtt = RenderTargetTexture::New("temp",
                                        ISize{
                                          width, // width,
                                          height // height
                                        },
                                        scene, false);

    passPostProcess->onApply = [&texture](Effect* effect, EventState&) {
      effect->setTexture("textureSampler", texture);
    };

    auto internalTexture = rtt->getInternalTexture();

    if (internalTexture) {
      scene->postProcessManager->directRender({passPostProcess},
                                              internalTexture);

      // Read the contents of the framebuffer
      auto numberOfChannelsByLine = static_cast<size_t>(width * 4);
      auto halfHeight             = height / 2;

      // Reading datas from WebGL
      auto data = engine->readPixels(0, 0, width, height);

      if (!texture->isCube) {
        if (!textureReservedDataStore.displayRed
            || !textureReservedDataStore.displayGreen
            || !textureReservedDataStore.displayBlue) {
          for (size_t i = 0; i < static_cast<size_t>(width * height * 4);
               i += 4) {

            if (!textureReservedDataStore.displayRed) {
              data[i] = 0;
            }

            if (!textureReservedDataStore.displayGreen) {
              data[i + 1] = 0;
            }

            if (!textureReservedDataStore.displayBlue) {
              data[i + 2] = 0;
            }

            if (textureReservedDataStore.displayAlpha) {
              auto alpha  = data[i + 2];
              data[i]     = alpha;
              data[i + 1] = alpha;
              data[i + 2] = alpha;
              data[i + 2] = 0;
            }
          }
        }
      }

      // To flip image on Y axis.
      auto _texture = std::static_pointer_cast<Texture>(texture);
      if ((_texture && _texture->invertY()) || texture->isCube) {
        for (size_t i = 0; i < static_cast<unsigned>(halfHeight); i++) {
          for (size_t j = 0; j < numberOfChannelsByLine; j++) {
            auto currentCell = j + i * numberOfChannelsByLine;
            auto targetLine  = static_cast<size_t>(height) - i - 1;
            auto targetCell  = j + targetLine * numberOfChannelsByLine;

            std::swap(data[currentCell], data[targetCell]);
          }
        }
      }

      renderImage(data, width, height);

      // Unbind
      engine->unBindFramebuffer(internalTexture);
    }

    rtt->dispose();
    passPostProcess->dispose();
  }

  static void render(const BaseTexturePtr& texture,
                     TextureReservedDataStore& textureReservedDataStore)
  {
    if (texture->isCube) {
      // Control3D
      // PX
      bool commandSelected = (textureReservedDataStore.face == 0);
      if (ImGui::Checkbox("PX", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 0;
        }
      }
      ImGui::SameLine();
      // NX
      commandSelected = (textureReservedDataStore.face == 1);
      if (ImGui::Checkbox("NX", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 1;
        }
      }
      ImGui::SameLine();
      // PY
      commandSelected = (textureReservedDataStore.face == 2);
      if (ImGui::Checkbox("PY", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 2;
        }
      }
      ImGui::SameLine();
      // NY
      commandSelected = (textureReservedDataStore.face == 3);
      if (ImGui::Checkbox("NY", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 3;
        }
      }
      ImGui::SameLine();
      // PZ
      commandSelected = (textureReservedDataStore.face == 4);
      if (ImGui::Checkbox("PZ", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 4;
        }
      }
      ImGui::SameLine();
      // NZ
      commandSelected = (textureReservedDataStore.face == 5);
      if (ImGui::Checkbox("NZ", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.face = 5;
        }
      }
    }
    else {
      // control
      // R
      bool commandSelected = (textureReservedDataStore.displayRed
                              && !textureReservedDataStore.displayGreen);
      if (ImGui::Checkbox("R", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.displayRed   = true;
          textureReservedDataStore.displayGreen = false;
          textureReservedDataStore.displayBlue  = false;
          textureReservedDataStore.displayAlpha = false;
        }
      }
      ImGui::SameLine();
      // G
      commandSelected = (textureReservedDataStore.displayGreen
                         && !textureReservedDataStore.displayBlue);
      if (ImGui::Checkbox("G", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.displayRed   = false;
          textureReservedDataStore.displayGreen = true;
          textureReservedDataStore.displayBlue  = false;
          textureReservedDataStore.displayAlpha = false;
        }
      }
      ImGui::SameLine();
      // B
      commandSelected = (textureReservedDataStore.displayBlue
                         && !textureReservedDataStore.displayAlpha);
      if (ImGui::Checkbox("B", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.displayRed   = false;
          textureReservedDataStore.displayGreen = false;
          textureReservedDataStore.displayBlue  = true;
          textureReservedDataStore.displayAlpha = false;
        }
      }
      ImGui::SameLine();
      // A
      commandSelected = (textureReservedDataStore.displayAlpha
                         && !textureReservedDataStore.displayRed);
      if (ImGui::Checkbox("A", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.displayRed   = false;
          textureReservedDataStore.displayGreen = false;
          textureReservedDataStore.displayBlue  = false;
          textureReservedDataStore.displayAlpha = true;
        }
      }
      ImGui::SameLine();
      // ALL
      commandSelected = (textureReservedDataStore.displayRed
                         && textureReservedDataStore.displayGreen);
      if (ImGui::Checkbox("ALL", &commandSelected)) {
        if (commandSelected) {
          textureReservedDataStore.displayRed   = true;
          textureReservedDataStore.displayGreen = true;
          textureReservedDataStore.displayBlue  = true;
          textureReservedDataStore.displayAlpha = true;
        }
      }
    }
  }

}; // end of struct TextureLineComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_LINES_TEXTURE_LINE_COMPONENT_H
