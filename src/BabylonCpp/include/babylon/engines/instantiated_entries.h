#ifndef BABYLON_ENGINES_INSTANTIATED_ENTRIES_H
#define BABYLON_ENGINES_INSTANTIATED_ENTRIES_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(AnimationGroup)
FWD_CLASS_SPTR(Skeleton)
FWD_CLASS_SPTR(TransformNode)

/**
 * @brief Class used to store the output of the AssetContainer.instantiateAllMeshesToScene function.
 */
struct BABYLON_SHARED_EXPORT InstantiatedEntries {
  /**
   * List of new root nodes (eg. nodes with no parent)
   */
  std::vector<TransformNodePtr> rootNodes;

  /**
   * List of new skeletons
   */
  std::vector<SkeletonPtr> skeletons;

  /**
   * List of new animation groups
   */
  std::vector<AnimationGroupPtr> animationGroups;
}; // end of struct InstantiatedEntries

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_INSTANTIATED_ENTRIES_H
