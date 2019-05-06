#ifndef BABYLON_SPRITES_ISPRITE_MANAGER_H
#define BABYLON_SPRITES_ISPRITE_MANAGER_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

class Camera;
class PickingInfo;
class Ray;
class Sprite;
using CameraPtr = std::shared_ptr<Camera>;
using SpritePtr = std::shared_ptr<Sprite>;

/**
 * @brief Defines the minimum interface to fullfil in order to be a sprite
 * manager.
 */
struct BABYLON_SHARED_EXPORT ISpriteManager : public IDisposable {

  /**
   * Restricts the camera to viewing objects with the same layerMask.
   * A camera with a layerMask of 1 will render spriteManager.layerMask &
   * camera.layerMask!== 0
   */
  unsigned int layerMask = 0x0FFFFFFF;

  /**
   * Gets or sets a boolean indicating if the mesh can be picked (by scene.pick
   * for instance or through actions). Default is true
   */
  bool isPickable = false;

  /**
   * Specifies the rendering group id for this mesh (0 by default).
   * @see
   * http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered#rendering-groups
   */
  unsigned int renderingGroupId = 0;

  /**
   * Defines the list of sprites managed by the manager.
   */
  std::vector<SpritePtr> sprites;

  /**
   * @brief Tests the intersection of a sprite with a specific ray.
   * @param ray The ray we are sending to test the collision
   * @param camera The camera space we are sending rays in
   * @param predicate A predicate allowing excluding sprites from the list of
   * object to test
   * @param fastCheck Is the hit test done in a OOBB or AOBB fashion the faster,
   * the less precise
   * @returns picking info or null.
   */
  virtual std::optional<PickingInfo>
  intersects(const Ray ray, const CameraPtr& camera,
             std::function<bool(Sprite* sprite)> predicate, bool fastCheck)
    = 0;

  /**
   * @brief Renders the list of sprites on screen.
   */
  virtual void render() = 0;

}; // end of struct ISpriteManager

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_ISPRITE_MANAGER_H
