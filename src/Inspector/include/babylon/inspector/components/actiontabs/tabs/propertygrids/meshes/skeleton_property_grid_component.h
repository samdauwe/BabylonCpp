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
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/meshes/skeleton_reserved_data_store.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

class Skeleton;
using SkeletonPtr = std::shared_ptr<Skeleton>;

struct BABYLON_SHARED_EXPORT SkeletonPropertyGridComponent {

  static void switchSkeletonViewers(
    const SkeletonPtr& /*skeleton*/,
    SkeletonReservedDataStore& /*skeletonReservedDataStore*/)
  {
  }

  static void render(const SkeletonPtr& /*skeleton*/,
                     SkeletonReservedDataStore& /*skeletonReservedDataStore*/)
  {
  }

}; // end of struct SkeletonPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_PROPERTY_GRID_COMPONENT_H
