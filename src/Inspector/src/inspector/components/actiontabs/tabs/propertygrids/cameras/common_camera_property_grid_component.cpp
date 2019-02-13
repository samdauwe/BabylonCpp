#include <babylon/inspector/components/actiontabs/tabs/propertygrids/cameras/common_camera_property_grid_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>

namespace BABYLON {

CommonCameraPropertyGridComponent::CommonCameraPropertyGridComponent(
  const ICommonCameraPropertyGridComponentProps& iProps)
    : props{iProps}
{
  state.mode = props.camera->mode;
}

CommonCameraPropertyGridComponent::~CommonCameraPropertyGridComponent()
{
}

void CommonCameraPropertyGridComponent::render()
{
  const auto& camera = props.camera;

  // --- GENERAL ---
  static auto generalOpened = true;
  ImGui::SetNextTreeNodeOpen(generalOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("GENERAL")) {
    TextLineComponent::render("ID", camera->id);
    TextLineComponent::render("Unique ID", std::to_string(camera->uniqueId));
    TextLineComponent::render("Class", camera->getClassName());
    generalOpened = true;
  }
  else {
    generalOpened = false;
  }
}

} // end of namespace BABYLON
