#ifndef BABYLON_COLLISIONS_PICKING_INFO_H
#define BABYLON_COLLISIONS_PICKING_INFO_H

#include <babylon/babylon_global.h>
#include <babylon/culling/ray.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Information about the result of picking within a scene.
 * See https://doc.babylonjs.com/babylon101/picking_collisions
 */
class BABYLON_SHARED_EXPORT PickingInfo {

public:
  PickingInfo();
  ~PickingInfo();

  /**
   * @brief Gets the normal corresponding to the face the pick collided with
   * @param useWorldCoordinates If the resulting normal should be relative to
   * the world (default: false)
   * @param useVerticesNormals If the vertices normals should be used to
   * calculate the normal instead of the normal map
   * @returns The normal corresponding to the face the pick collided with
   */
  nullable_t<Vector3> getNormal(bool useWorldCoordinates = false,
                                bool useVerticesNormals  = true);

  /**
   * @brief Gets the texture coordinates of where the pick occured.
   * @returns the vector containing the coordnates of the texture
   */
  nullable_t<Vector2> getTextureCoordinates();

public:
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
  nullable_t<Vector3> pickedPoint;

  /**
   * The mesh corresponding the the pick collision
   */
  AbstractMeshPtr pickedMesh = nullptr;

  /**
   * (See getTextureCoordinates) The barycentric U coordinate that is used when
   * calulating the texture coordinates of the collision.
   */
  float bu;

  /**
   * (See getTextureCoordinates) The barycentric V coordinate that is used when
   * calulating the texture coordinates of the collision.
   */
  float bv;

  /**
   * The id of the face on the mesh that was picked
   */
  unsigned int faceId;

  /**
   * Id of the the submesh that was picked
   */
  unsigned int subMeshId;

  /**
   * If a sprite was picked, this will be the sprite the pick collided with
   */
  Sprite* pickedSprite;

  /**
   * If a mesh was used to do the picking (eg. 6dof controller) this will be
   * populated.
   */
  AbstractMesh* originMesh;

  /**
   * The ray that was used to perform the picking.
   */
  nullable_t<Ray> ray;

}; // end of class PickingInfo

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_PICKING_INFO_H
