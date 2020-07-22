#ifndef BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H
#define BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/culling/octrees/ioctree_container.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class Plane;
class Ray;

/**
 * @brief Class used to store a cell in an octree.
 * @see https://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
 */
template <class T>
class BABYLON_SHARED_EXPORT OctreeBlock : public IOctreeContainer<T> {

public:
  /**
   * @brief Creates a new block.
   * @param minPoint defines the minimum vector (in world space) of the block's
   * bounding box.
   * @param maxPoint defines the maximum vector (in world space) of the block's
   * bounding box
   * @param capacity defines the maximum capacity of this block (if capacity is
   * reached the block will be split into sub blocks)
   * @param depth defines the current depth of this block in the octree
   * @param maxDepth defines the maximal depth allowed (beyond this value, the
   * capacity is ignored)
   * @param creationFunc defines a callback to call when an element is added to
   * the block
   */
  OctreeBlock(const Vector3& minPoint, const Vector3& maxPoint, size_t capacity, size_t depth,
              size_t maxDepth, const std::function<void(T&, OctreeBlock<T>&)>& creationFunc);
  ~OctreeBlock(); // = default

  /** Properties **/
  /**
   * @brief Gets the maximum capacity of this block (if capacity is reached the
   * block will be split into sub blocks).
   */
  [[nodiscard]] size_t capacity() const;

  /**
   * @brief Gets the minimum vector (in world space) of the block's bounding
   * box.
   */
  Vector3& minPoint();

  /**
   * @brief Gets the maximum vector (in world space) of the block's bounding
   * box.
   */
  Vector3& maxPoint();

  /** Methods **/

  /**
   * @brief Add a new element to this block.
   * @param entry defines the element to add
   */
  void addEntry(T& entry);

  /**
   * @brief Remove an element from this block.
   * @param entry defines the element to remove
   */
  void removeEntry(T& entry);

  /**
   * @brief Add an array of elements to this block.
   * @param entries defines the array of elements to add
   */
  void addEntries(std::vector<T>& entries);

  /**
   * @brief Test if the current block intersects the furstum planes and if yes,
   * then add its content to the selection array.
   * @param frustumPlanes defines the frustum planes to test
   * @param selection defines the array to store current content if selection is
   * positive
   * @param allowDuplicate defines if the selection array can contains
   * duplicated entries
   */
  void select(const std::array<Plane, 6>& frustumPlanes, std::vector<T>& selection,
              bool allowDuplicate = true);

  /**
   * @brief Test if the current block intersect with the given bounding sphere
   * and if yes, then add its content to the selection array.
   * @param sphereCenter defines the bounding sphere center
   * @param sphereRadius defines the bounding sphere radius
   * @param selection defines the array to store current content if selection is
   * positive
   * @param allowDuplicate defines if the selection array can contains
   * duplicated entries
   */
  void intersects(const Vector3& sphereCenter, float sphereRadius, std::vector<T>& selection,
                  bool allowDuplicate = true);

  /**
   * @brief Test if the current block intersect with the given ray and if yes,
   * then add its content to the selection array.
   * @param ray defines the ray to test with
   * @param selection defines the array to store current content if selection is
   * positive
   */
  void intersectsRay(const Ray& ray, std::vector<T>& selection);

  /**
   * @brief Subdivide the content into child blocks (this block will then be
   * empty).
   */
  void createInnerBlocks();

  /**
   * @brief Hidden
   */
  static void _CreateBlocks(const Vector3& worldMin, const Vector3& worldMax,
                            std::vector<T>& entries, size_t maxBlockCapacity, size_t currentDepth,
                            size_t maxDepth, IOctreeContainer<T>& target,
                            const std::function<void(T&, OctreeBlock<T>&)>& creationFunc);

public:
  /**
   * Gets the content of the current block
   */
  std::vector<T> entries;

private:
  size_t _depth;
  size_t _maxDepth;
  size_t _capacity;
  Vector3 _minPoint;
  Vector3 _maxPoint;
  std::array<Vector3, 8> _boundingVectors;
  std::function<void(T&, OctreeBlock<T>&)> _creationFunc;

}; // end of class OctreeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H
