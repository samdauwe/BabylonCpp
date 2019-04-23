#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_TRANSFORM_NODE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_TRANSFORM_NODE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/quaternion_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/mesh/transform_node.h>

namespace BABYLON {

class TransformNode;
using TransformNodePtr = std::shared_ptr<TransformNode>;

struct BABYLON_SHARED_EXPORT TransformNodePropertyGridComponent {

  static void render(const TransformNodePtr& transformNode)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", transformNode->id);
      TextLineComponent::render("Unique ID",
                                std::to_string(transformNode->uniqueId));
      TextLineComponent::render("Class", transformNode->getClassName());
      if (CheckBoxLineComponent::render("IsEnabled",
                                        transformNode->isEnabled())) {
        transformNode->setEnabled(!transformNode->isEnabled());
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // --- TRANSFORMATIONS ---
    static auto transformationsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformationsContainerOpened,
                               ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMATIONS")) {
      Vector3LineComponent::render("Position", transformNode->position());
      if (!transformNode->rotationQuaternion()) {
        Vector3LineComponent::render("Rotation", transformNode->rotation());
      }
      if (transformNode->rotationQuaternion()) {
        QuaternionLineComponent::render(
          "Rotation", transformNode->rotationQuaternion().value());
      }
      Vector3LineComponent::render("Scaling", transformNode->scaling());
      transformationsContainerOpened = true;
    }
    else {
      transformationsContainerOpened = false;
    }
  }

}; // end of struct TransformNodePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_TRANSFORM_NODE_PROPERTY_GRID_COMPONENT_H
