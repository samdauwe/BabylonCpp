#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_BONE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_BONE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/bones/bone.h>
#include <babylon/inspector/components/actiontabs/lines/quaternion_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BonePropertyGridComponent {

  static void render(const BonePtr& bone)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Name", bone->name);
      TextLineComponent::render("Index", std::to_string(bone->getIndex()));
      TextLineComponent::render("Unique ID", std::to_string(bone->uniqueId));
      {
        if (bone->getParent()) {
          TextLineComponent::render("Parent", bone->getParent()->name);
        }
      }
      {
        if (bone->getTransformNode()) {
          TextLineComponent::render("Linked node", bone->getTransformNode()->name);
        }
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // --- TRANSFORMATIONS ---
    static auto transformationsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformationsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORMATIONS")) {
      Vector3LineComponent::render("Position", bone->position());
      if (!bone->rotationQuaternion()) {
        Vector3LineComponent::render("Rotation", bone->rotation());
      }
      else if (bone->rotationQuaternion()) {
        QuaternionLineComponent::render("Rotation", *bone->rotationQuaternion());
      }
      Vector3LineComponent::render("Scaling", *bone->scaling());
      transformationsContainerOpened = true;
    }
    else {
      transformationsContainerOpened = false;
    }
  }

}; // end of struct BonePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_BONE_PROPERTY_GRID_COMPONENT_H
