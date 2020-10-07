#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_POST_PROCESS_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_POST_PROCESS_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT CommonPostProcessPropertyGridComponent {

  static void render(const PostProcessPtr& postProcess)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Class", postProcess->getClassName());
      TextLineComponent::render("Width", std::to_string(postProcess->width));
      TextLineComponent::render("Height", std::to_string(postProcess->height));
      if (CheckBoxLineComponent::render("Auto clear", postProcess->autoClear)) {
        postProcess->autoClear = !postProcess->autoClear;
      }
      if (postProcess->clearColor) {
        Color3LineComponent::render("Clear color", *postProcess->clearColor);
      }
      if (CheckBoxLineComponent::render("Pixel perfect", postProcess->enablePixelPerfectMode)) {
        postProcess->enablePixelPerfectMode = !postProcess->enablePixelPerfectMode;
      }
      if (CheckBoxLineComponent::render("Fullscreen viewport",
                                        postProcess->forceFullscreenViewport)) {
        postProcess->forceFullscreenViewport = !postProcess->forceFullscreenViewport;
      }
      auto sliderChange
        = SliderLineComponent::render("Samples", postProcess->samples(), 1.f, 8.f, 1.f, "%.0f");
      if (sliderChange) {
        postProcess->samples = static_cast<unsigned>(sliderChange.value());
      }
      if (ButtonLineComponent::render("Dispose")) {
        postProcess->dispose();
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
  }

}; // end of struct CommonPostProcessPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_POST_PROCESS_PROPERTY_GRID_COMPONENT_H
