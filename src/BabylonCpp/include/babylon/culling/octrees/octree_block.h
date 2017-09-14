#ifndef BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H
#define BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H

#include <babylon/babylon_global.h>
#include <babylon/culling/octrees/ioctree_container.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

template <class T>
class BABYLON_SHARED_EXPORT OctreeBlock : public IOctreeContainer<T> {

public:
  OctreeBlock(const Vector3& minPoint, const Vector3& maxPoint, size_t capacity,
              size_t depth, size_t maxDepth,
              const ::std::function<void(T&, OctreeBlock<T>&)>& creationFunc);
  ~OctreeBlock();

  /** Properties **/
  size_t capacity() const;
  Vector3& minPoint();
  Vector3& maxPoint();

  /** Methods **/
  void addEntry(T& entry);
  void addEntries(vector_t<T>& entries);
  void select(const array_t<Plane, 6>& frustumPlanes, vector_t<T>& selection,
              bool allowDuplicate = true);
  void intersects(const Vector3& sphereCenter, float sphereRadius,
                  vector_t<T>& selection, bool allowDuplicate = true);
  void intersectsRay(const Ray& ray, vector_t<T>& selection);
  void createInnerBlocks();

public:
  vector_t<T> entries;

private:
  size_t _depth;
  size_t _maxDepth;
  size_t _capacity;
  Vector3 _minPoint;
  Vector3 _maxPoint;
  vector_t<Vector3> _boundingVectors;
  ::std::function<void(T&, OctreeBlock<T>&)> _creationFunc;

}; // end of class OctreeBlock

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_OCTREES_OCTREE_BLOCK_H
