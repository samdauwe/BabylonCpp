#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_RESERVED_DATA_STORE_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_RESERVED_DATA_STORE_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Debug {
class SkeletonViewer;
using SkeletonViewerPtr = std::shared_ptr<SkeletonViewer>;
} // end of namespace Debug

struct BABYLON_SHARED_EXPORT SkeletonReservedDataStore {
  bool skeletonViewersEnabled                           = false;
  std::vector<Debug::SkeletonViewerPtr> skeletonViewers = {};
  Debug::SkeletonViewerPtr skeletonViewer               = nullptr;
}; // end of struct SkeletonReservedDataStore

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_SKELETON_RESERVED_DATA_STORE_H
