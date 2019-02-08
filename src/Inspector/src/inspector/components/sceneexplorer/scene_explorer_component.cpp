#include <babylon/inspector/components/sceneexplorer/scene_explorer_component.h>

#include <string>
#include <vector>

// ImGui
#include <imgui.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>

namespace BABYLON {

SceneExplorerComponent::SceneExplorerComponent(
  const ISceneExplorerComponentProps& iProps)
    : props{iProps}, _materialTreeItemComponent{nullptr}
{
  // Camera
  ICameraTreeItemComponentProps cameraProps;
  cameraProps.camera = props.scene->cameras.front();
  _cameraTreeItemComponent
    = std::make_unique<CameraTreeItemComponent>(cameraProps);
  // Light
  ILightTreeItemComponentProps lightProps;
  lightProps.light = props.scene->lights.front();
  _lightTreeItemComponent
    = std::make_unique<LightTreeItemComponent>(lightProps);
  // Light
  IMeshTreeItemComponentProps meshProps;
  meshProps.mesh         = props.scene->meshes.front();
  _meshTreeItemComponent = std::make_unique<MeshTreeItemComponent>(meshProps);
}

SceneExplorerComponent::~SceneExplorerComponent()
{
}

void SceneExplorerComponent::render()
{
  // Filter
  {
    static ImGuiTextFilter filter;
    filter.Draw();
    const std::vector<std::string> lines;
    for (const auto& line : lines) {
      if (filter.PassFilter(line.c_str())) {
        ImGui::BulletText("%s", line.c_str());
      }
    }
  }

  // Testing
  {
    // Camera
    if (_cameraTreeItemComponent) {
      _cameraTreeItemComponent->render();
    }
    // Light
    if (_lightTreeItemComponent) {
      _lightTreeItemComponent->render();
    }
    // Mesh
    if (_meshTreeItemComponent) {
      _meshTreeItemComponent->render();
    }
    // Material
    if (_materialTreeItemComponent) {
      _materialTreeItemComponent->render();
    }
    else {
      if (!props.scene->materials.empty()) {
        // Material
        IMaterialTreeItemComponentProps materialProps;
        materialProps.material = props.scene->materials.front();
        _materialTreeItemComponent
          = std::make_unique<MaterialTreeItemComponent>(materialProps);
      }
    }
  }

  // Tree
  {
    ImGui::SetNextTreeNodeOpen(true);
    ImGui::Unindent();
    if (ImGui::TreeNode("Scene")) {
      if (ImGui::TreeNode("Nodes")) {
        ImGui::Indent();
        // Render tree item
        static bool isActive = true;
        if (ImGui::Selectable("camera1", isActive)) {
          isActive = !isActive;
        }
        ImGui::Unindent();
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Materials")) {
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Textures")) {
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Render pipelines")) {
        ImGui::TreePop();
      }
      ImGui::TreePop();
    }
    ImGui::Indent();
  }

  {
    ImGui::Text("Scene");

    static int selection_mask
      = (1 << 2); // Dumb representation of what may be user-side selection
                  // state. You may carry selection state inside or outside your
                  // objects in whatever format you see fit.
    int node_clicked = -1; // Temporary storage of what node we have clicked to
                           // process selection at the end of the loop. May be a
                           // pointer to your own node type, etc.
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,
                        ImGui::GetFontSize()
                          * 3); // Increase spacing to differentiate leaves from
                                // expanded contents.
    const std::vector<std::string> nodes{"Nodes", "Materials", "Textures",
                                         "Render pipelines"};
    for (size_t i = 0; i < 4; i++) {
      // Disable the default open on single-click behavior and pass in Selected
      // flag according to our selection state.
      ImGuiTreeNodeFlags node_flags
        = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
          | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
      if (i < 2) {
        // Node
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s",
                                           nodes[i].c_str());
        if (ImGui::IsItemClicked())
          node_clicked = i;
        if (node_open) {
          ImGui::Text("A Node");
          ImGui::TreePop();
        }
      }
      else {
        // Leaf: The only reason we have a TreeNode at all is to allow selection
        // of the leaf. Otherwise we can use BulletText() or
        // TreeAdvanceToLabelPos()+Text().
        node_flags
          |= ImGuiTreeNodeFlags_Leaf
             | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
        ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s",
                          nodes[i].c_str());
        if (ImGui::IsItemClicked())
          node_clicked = i;
      }
    }
    if (node_clicked != -1) {
      // Update selection state. Process outside of tree loop to avoid visual
      // inconsistencies during the clicking-frame.
      if (ImGui::GetIO().KeyCtrl)
        selection_mask ^= (1 << node_clicked); // CTRL+click to toggle
      else // if (!(selection_mask & (1 << node_clicked))) // Depending on
           // selection behavior you want, this commented bit preserve selection
           // when clicking on item that is part of the selection
        selection_mask = (1 << node_clicked); // Click to single-select
    }
    ImGui::PopStyleVar();
  }
}

} // end of namespace BABYLON
