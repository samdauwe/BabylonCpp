#ifndef BABYLON_SPRITES_ISPRITE_MANAGER_H
#define BABYLON_SPRITES_ISPRITE_MANAGER_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

class Camera;
class PickingInfo;
class Ray;
class Scene;
class Sprite;
class Texture;
using CameraPtr  = std::shared_ptr<Camera>;
using SpritePtr  = std::shared_ptr<Sprite>;
using TexturePtr = std::shared_ptr<Texture>;

/**
 * @brief Defines the minimum interface to fullfil in order to be a sprite manager.
 */
class BABYLON_SHARED_EXPORT ISpriteManager : public IDisposable {

public:
  ISpriteManager();
  ~ISpriteManager();

  /**
   * Gets manager's name
   */
  std::string name;

  /**
   * Restricts the camera to viewing objects with the same layerMask.
   * A camera with a layerMask of 1 will render spriteManager.layerMask & camera.layerMask!== 0
   */
  unsigned int layerMask = 0x0FFFFFFF;

  /**
   * Gets or sets a boolean indicating if the mesh can be picked (by scene.pick for instance or
   * through actions). Default is true
   */
  bool isPickable = false;

  /**
   * Gets the hosting scene
   */
  ReadOnlyProperty<ISpriteManager, Scene*> scene;

  /**
   * Specifies the rendering group id for this mesh (0 by default)
   * @see
   * https://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered#rendering-groups
   */
  unsigned int renderingGroupId = 0;

  /**
   * Defines the list of sprites managed by the manager.
   */
  std::vector<SpritePtr> sprites;

  /**
   * Gets or sets the spritesheet texture
   */
  Property<ISpriteManager, TexturePtr> texture;

  /**
   * Defines the default width of a cell in the spritesheet
   */
  int cellWidth = 0;

  /**
   * Defines the default height of a cell in the spritesheet
   */
  int cellHeight = 0;

  /**
   * @brief Tests the intersection of a sprite with a specific ray.
   * @param ray The ray we are sending to test the collision
   * @param camera The camera space we are sending rays in
   * @param predicate A predicate allowing excluding sprites from the list of object to test
   * @param fastCheck defines if the first intersection will be used (and not the closest)
   * @returns picking info or null.
   */
  virtual std::optional<PickingInfo>
  intersects(const Ray& ray, const CameraPtr& camera,
             const std::function<bool(Sprite* sprite)>& predicate, bool fastCheck)
    = 0;

  /**
   * @brief Intersects the sprites with a ray.
   * @param ray defines the ray to intersect with
   * @param camera defines the current active camera
   * @param predicate defines a predicate used to select candidate sprites
   * @returns null if no hit or a PickingInfo array
   */
  virtual std::vector<PickingInfo>
  multiIntersects(const Ray& ray, const CameraPtr& camera,
                  const std::function<bool(Sprite* sprite)>& predicate)
    = 0;

  /**
   * @brief Renders the list of sprites on screen.
   */
  virtual void render() = 0;

protected:
  /**
   * @brief Gets the hosting scene.
   */
  virtual Scene*& get_scene() = 0;

  /**
   * @brief Gets the spritesheet texture.
   */
  virtual TexturePtr& get_texture() = 0;

  /**
   * @brief Sets the spritesheet texture.
   */
  virtual void set_texture(const TexturePtr& value) = 0;

}; // end of class ISpriteManager

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_ISPRITE_MANAGER_H
