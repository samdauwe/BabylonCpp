#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/bones/skeleton.h>
#include <babylon/debug/skeleton_viewer.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/animation_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/animation_group_grid_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/animation_reserved_data_store.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/skeleton_reserved_data_store.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

class Skeleton;
using SkeletonPtr = std::shared_ptr<Skeleton>;

struct BABYLON_SHARED_EXPORT SkeletonPropertyGridComponent {

  static void
  switchSkeletonViewers(const SkeletonPtr& skeleton,
                        SkeletonReservedDataStore& skeletonReservedDataStore)
  {
    skeletonReservedDataStore.skeletonViewersEnabled
      = !skeletonReservedDataStore.skeletonViewersEnabled;
    auto scene = skeleton->getScene();

    if (skeletonReservedDataStore.skeletonViewersEnabled) {
      for (const auto& mesh : scene->meshes) {
        if (mesh->skeleton() == skeleton) {
          auto found = false;
          for (const auto& skeletonViewer :
               skeletonReservedDataStore.skeletonViewers) {
            if (skeletonViewer->skeleton == mesh->skeleton()) {
              found = true;
              break;
            }
          }
          if (found) {
            continue;
          }
          auto viewer = std::make_shared<Debug::SkeletonViewer>(
            mesh->skeleton(), mesh, scene, true, 0);
          viewer->isEnabled = true;
          skeletonReservedDataStore.skeletonViewers.emplace_back(viewer);
          skeletonReservedDataStore.skeletonViewer = viewer;
        }
      }
    }
    else {
      for (const auto& skeletonViewer :
           skeletonReservedDataStore.skeletonViewers) {
        skeletonReservedDataStore.skeletonViewer = nullptr;
        skeletonViewer->dispose();
      }
      skeletonReservedDataStore.skeletonViewers.clear();
    }
  }

  static void render(const SkeletonPtr& skeleton,
                     SkeletonReservedDataStore& skeletonReservedDataStore,
                     AnimationReservedDataStore& animationReservedDataStore)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", skeleton->id);
      TextLineComponent::render("Bone count",
                                std::to_string(skeleton->bones.size()));
      if (CheckBoxLineComponent::render(
            "Use texture to store matrices",
            skeleton->useTextureToStoreBoneMatrices())) {
        skeleton->useTextureToStoreBoneMatrices
          = !skeleton->useTextureToStoreBoneMatrices();
      }
      if (CheckBoxLineComponent::render(
            "Debug mode", skeletonReservedDataStore.skeletonViewersEnabled)) {
        switchSkeletonViewers(skeleton, skeletonReservedDataStore);
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // -- Animations --
    AnimationGridComponent::render(skeleton, animationReservedDataStore);
  }

}; // end of struct SkeletonPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_PROPERTY_GRID_COMPONENT_H
