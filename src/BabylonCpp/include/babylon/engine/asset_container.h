#ifndef BABYLON_ENGINE_ASSET_CONTAINER_H
#define BABYLON_ENGINE_ASSET_CONTAINER_H

#include <babylon/babylon_api.h>
#include <babylon/engine/abstract_scene.h>

namespace BABYLON {

class AssetContainer;
class Mesh;
using AssetContainerPtr = std::shared_ptr<AssetContainer>;
using MeshPtr           = std::shared_ptr<Mesh>;

/**
 * @brief Container with a set of assets that can be added or removed from a
 * scene.
 */
class BABYLON_SHARED_EXPORT AssetContainer : public AbstractScene {

public:
  template <typename... Ts>
  static AssetContainerPtr New(Ts&&... args)
  {
    return std::shared_ptr<AssetContainer>(
      new AssetContainer(std::forward<Ts>(args)...));
  }
  ~AssetContainer();

  /**
   * @brief Adds all the assets from the container to the scene.
   */
  void addAllToScene();

  /**
   * @brief Removes all the assets in the container from the scene.
   */
  void removeAllFromScene();

  /**
   * @brief Disposes all the assets in the container.
   */
  void dispose();

  /**
   * @brief Adds all meshes in the asset container to a root mesh that can be
   * used to position all the contained meshes. The root mesh is then added to
   * the front of the meshes in the assetContainer.
   * @returns the root mesh
   */
  MeshPtr createRootMesh();

protected:
  /**
   * @brief Instantiates an AssetContainer.
   * @param scene The scene the AssetContainer belongs to.
   */
  AssetContainer(Scene* scene);

public:
  /**
   * @brief The scene the AssetContainer belongs to.
   */
  Scene* scene;

}; // end of class AssetContainer

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ASSET_CONTAINER_H
