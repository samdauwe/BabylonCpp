#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/hemispheric_light_property_grid_component.h>

#include <imgui.h>

#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_light_property_grid_component.h>
#include <babylon/lights/hemispheric_light.h>

namespace BABYLON {

HemisphericLightPropertyGridComponent::HemisphericLightPropertyGridComponent(
  const IHemisphericLightPropertyGridComponentProps& iProps)
    : props{iProps}
{
  ICommonLightPropertyGridComponentProps commonLightProps;
  commonLightProps.light = std::static_pointer_cast<Light>(props.light);
  _commonLightPropertyGridComponent
    = std::make_unique<CommonLightPropertyGridComponent>(commonLightProps);
}

HemisphericLightPropertyGridComponent::~HemisphericLightPropertyGridComponent()
{
}

void HemisphericLightPropertyGridComponent::render()
{
  const auto& light = props.light;

  if (_commonLightPropertyGridComponent) {
    _commonLightPropertyGridComponent->render();
  }
  // --- SETUP ---
  static auto setupOpened = true;
  ImGui::SetNextTreeNodeOpen(setupOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("SETUP")) {
    Color3LineComponent::render("Diffuse", light->diffuse);
    Color3LineComponent::render("Ground", light->groundColor);
    setupOpened = true;
  }
  else {
    setupOpened = false;
  }
}

} // end of namespace BABYLON
