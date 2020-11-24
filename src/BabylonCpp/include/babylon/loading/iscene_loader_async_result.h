#ifndef BABYLON_LOADING_ISCENE_LOADER_ASYNC_RESULT_H
#define BABYLON_LOADING_ISCENE_LOADER_ASYNC_RESULT_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(IParticleSystem)
FWD_CLASS_SPTR(Skeleton)
FWD_CLASS_SPTR(AnimationGroup)
FWD_CLASS_SPTR(TransformNode)
FWD_CLASS_SPTR(Geometry)
FWD_CLASS_SPTR(Light)

/**
 * @brief Interface used for the result of ImportMeshAsync.
 */
struct BABYLON_SHARED_EXPORT ISceneLoaderAsyncResult {
  /**
   * The array of loaded meshes
   */
  std::vector<AbstractMeshPtr> meshes;

  /**
   * The array of loaded particle systems
   */
  std::vector<IParticleSystemPtr> particleSystems;

  /**
   * The array of loaded skeletons
   */
  std::vector<SkeletonPtr> skeletons;

  /**
   * The array of loaded animation groups
   */
  std::vector<AnimationGroupPtr> animationGroups;

  /**
   * The array of loaded transform nodes
   */
  std::vector<TransformNodePtr> transformNodes;

  /**
   * The array of loaded geometries
   */
  std::vector<GeometryPtr> geometries;

  /**
   * The array of loaded lights
   */
  std::vector<LightPtr> lights;
}; // end of struct ISceneLoaderAsyncResult

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_ISCENE_LOADER_ASYNC_RESULT_H
