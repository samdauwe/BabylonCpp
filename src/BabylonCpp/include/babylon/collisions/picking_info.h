#ifndef BABYLON_COLLISIONS_PICKING_INFO_H
#define BABYLON_COLLISIONS_PICKING_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/culling/ray.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class Vector2;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Sprite)

/**
 * @brief Information about the result of picking within a scene.
 * @see https://doc.babylonjs.com/babylon101/picking_collisions
 */
class BABYLON_SHARED_EXPORT PickingInfo {

public:
  PickingInfo();
  PickingInfo(const PickingInfo& other);
  PickingInfo(PickingInfo&& other);
  PickingInfo& operator=(const PickingInfo& other);
  PickingInfo& operator=(PickingInfo&& other);
  ~PickingInfo(); // = default

  /**
   * @brief Gets the normal corresponding to the face the pick collided with
   * @param useWorldCoordinates If the resulting normal should be relative to
   * the world (default: false)
   * @param useVerticesNormals If the vertices normals should be used to
   * calculate the normal instead of the normal map
   * @returns The normal corresponding to the face the pick collided with
   */
  std::optional<Vector3> getNormal(bool useWorldCoordinates = false,
                                   bool useVerticesNormals  = true);

  /**
   * @brief Gets the texture coordinates of where the pick occured.
   * @returns the vector containing the coordnates of the texture
   */
  std::optional<Vector2> getTextureCoordinates();

public:
  /**
   * Hidden
   */
  bool _pickingUnavailable;

  /**
   * If the pick collided with an object
   */
  bool hit;

  /**
   * Distance away where the pick collided
   */
  float distance;

  /**
   * The location of pick collision
   */
  std::optional<Vector3> pickedPoint;

  /**
   * The mesh corresponding the the pick collision
   */
  AbstractMeshPtr pickedMesh;

  /**
   * (See getTextureCoordinates) The barycentric U coordinate that is used when
   * calculating the texture coordinates of the collision.
   */
  float bu;

  /**
   * (See getTextureCoordinates) The barycentric V coordinate that is used when
   * calculating the texture coordinates of the collision.
   */
  float bv;

  /**
   * The index of the face on the mesh that was picked, or the index of the Line
   * if the picked Mesh is a LinesMesh
   */
  int faceId;

  /**
   * The index of the face on the subMesh that was picked, or the index of the Line if the picked
   * Mesh is a LinesMesh
   */
  int subMeshFaceId;

  /**
   * Id of the the submesh that was picked
   */
  int subMeshId;

  /**
   * If a sprite was picked, this will be the sprite the pick collided with
   */
  SpritePtr pickedSprite;

  /**
   * If we are picking a mesh with thin instance, this will give you the picked thin instance
   */
  int thinInstanceIndex;

  /**
   * If a mesh was used to do the picking (eg. 6dof controller) this will be
   * populated.
   */
  AbstractMeshPtr originMesh;

  /**
   * The ray that was used to perform the picking.
   */
  std::optional<Ray> ray;

}; // end of class PickingInfo

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_PICKING_INFO_H
