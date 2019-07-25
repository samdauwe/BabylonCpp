#ifndef BABYLON_CULLING_OCTREES_OCTREE_H
#define BABYLON_CULLING_OCTREES_OCTREE_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/culling/octrees/ioctree_container.h>

namespace BABYLON {

class AbstractMesh;
class Plane;
class Ray;
class SubMesh;
class Vector3;

/**
 * @brief Octrees are a really powerful data structure that can quickly select
 * entities based on space coordinates.
 * @see https://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
 */
template <class T>
class BABYLON_SHARED_EXPORT Octree : public IOctreeContainer<T> {

public:
  /**
   * @brief Creates a octree.
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   * @param creationFunc function to be used to instatiate the octree
   * @param maxBlockCapacity defines the maximum number of meshes you want on
   * your octree's leaves (default: 64)
   * @param maxDepth defines the maximum depth (sub-levels) for your octree.
   * Default value is 2, which means 8 8 8 = 512 blocks :) (This parameter takes
   * precedence over capacity.)
   */
  Octree();
  Octree(
    const std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc,
    std::size_t maxBlockCapacity = 64, std::size_t maxDepth = 2);
  ~Octree();

  /** Methods **/

  /**
   * @brief Updates the octree by adding blocks for the passed in meshes within
   * the min and max world parameters.
   * @param worldMin worldMin for the octree blocks var blockSize = new
   * Vector3((worldMax.x - worldMin.x) / 2, (worldMax.y - worldMin.y) / 2,
   * (worldMax.z - worldMin.z) / 2);
   * @param worldMax worldMax for the octree blocks var blockSize = new
   * Vector3((worldMax.x - worldMin.x) / 2, (worldMax.y - worldMin.y) / 2,
   * (worldMax.z - worldMin.z) / 2);
   * @param entries meshes to be added to the octree blocks
   */
  void update(const Vector3& worldMin, const Vector3& worldMax,
              std::vector<T>& entries);

  /**
   * @brief Adds a mesh to the octree.
   * @param entry Mesh to add to the octree
   */
  void addMesh(T& entry);

  /**
   * @brief Remove an element from the octree.
   * @param entry defines the element to remove
   */
  void removeMesh(T& entry);

  /**
   * @brief Selects an array of meshes within the frustum.
   * @param frustumPlanes The frustum planes to use which will select all meshes
   * within it
   * @param allowDuplicate If duplicate objects are allowed in the resulting
   * object array
   * @returns array of meshes within the frustum
   */
  std::vector<T>& select(const std::array<Plane, 6>& frustumPlanes,
                         bool allowDuplicate = true);

  /**
   * @brief Test if the octree intersect with the given bounding sphere and if
   * yes, then add its content to the selection array.
   * @param sphereCenter defines the bounding sphere center
   * @param sphereRadius defines the bounding sphere radius
   * @param allowDuplicate defines if the selection array can contains
   * duplicated entries
   * @returns an array of objects that intersect the sphere
   */
  std::vector<T>& intersects(const Vector3& sphereCenter, float sphereRadius,
                             bool allowDuplicate = true);

  /**
   * @brief Test if the octree intersect with the given ray and if yes, then add
   * its content to resulting array.
   * @param ray defines the ray to test with
   * @returns array of intersected objects
   */
  std::vector<T>& intersectsRay(const Ray& ray);

  /** Statics **/

  /**
   * @brief Adds a mesh into the octree block if it intersects the block.
   */
  static void CreationFuncForMeshes(AbstractMesh* entry,
                                    OctreeBlock<AbstractMesh*>& block);

  /**
   * @brief Adds a submesh into the octree block if it intersects the block.
   */
  static void CreationFuncForSubMeshes(SubMesh* entry,
                                       OctreeBlock<SubMesh*>& block);

public:
  /**
   * Content stored in the octree
   */
  std::vector<T> dynamicContent;

  /**
   * Defines the maximum depth (sub-levels) for your octree. Default value is 2,
   * which means 8 8 8 = 512 blocks :) (This parameter takes precedence over
   * capacity.)
   */
  std::size_t maxDepth;

private:
  std::size_t _maxBlockCapacity;

  std::vector<T> _selectionContent;
  std::function<void(T&, OctreeBlock<T>&)> _creationFunc;

}; // end of class Octree

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_OCTREES_OCTREE_H
