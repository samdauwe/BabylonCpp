#include <babylon/inspector/components/actiontabs/tabs/propertygrids/lights/common_light_property_grid_component.h>

#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/lights/light.h>

namespace BABYLON {

CommonLightPropertyGridComponent::CommonLightPropertyGridComponent(
  const ICommonLightPropertyGridComponentProps& iProps)
    : props{iProps}
{
}

CommonLightPropertyGridComponent::~CommonLightPropertyGridComponent()
{
}

void CommonLightPropertyGridComponent::render()
{
  const auto& light = props.light;

  // --- GENERAL ---
  static auto generalOpened = true;
  ImGui::SetNextTreeNodeOpen(generalOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("GENERAL")) {
    TextLineComponent::render("ID", light->id);
    TextLineComponent::render("Unique ID", std::to_string(light->uniqueId));
    TextLineComponent::render("Class", light->getClassName());
    FloatLineComponent::render("Intensity", light->intensity);
    generalOpened = true;
  }
  else {
    generalOpened = false;
  }
}

} // end of namespace BABYLON
