#ifndef BABYLON_CULLING_OCTREES_OCTREE_H
#define BABYLON_CULLING_OCTREES_OCTREE_H

#include <babylon/babylon_global.h>
#include <babylon/culling/octrees/ioctree_container.h>

namespace BABYLON {

template <class T>
class BABYLON_SHARED_EXPORT Octree : public IOctreeContainer<T> {

public:
  Octree();
  Octree(
    const ::std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc,
    std::size_t maxBlockCapacity = 64, std::size_t maxDepth = 2);
  ~Octree();

  /** Methods **/
  void update(const Vector3& worldMin, const Vector3& worldMax,
              vector_t<T>& entries);
  void addMesh(T& entry);
  vector_t<T>& select(const array_t<Plane, 6>& frustumPlanes,
                      bool allowDuplicate = true);
  vector_t<T>& intersects(const Vector3& sphereCenter, float sphereRadius,
                          bool allowDuplicate = true);
  vector_t<T>& intersectsRay(const Ray& ray);

  /** Statics **/
  static void _CreateBlocks(
    const Vector3& worldMin, const Vector3& worldMax, vector_t<T>& entries,
    std::size_t maxBlockCapacity, std::size_t currentDepth,
    std::size_t maxDepth, IOctreeContainer<T>& target,
    ::std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc);
  static void CreationFuncForMeshes(AbstractMesh* entry,
                                    OctreeBlock<AbstractMesh*>& block);
  static void CreationFuncForSubMeshes(SubMesh* entry,
                                       OctreeBlock<SubMesh*>& block);

public:
  vector_t<T> dynamicContent;

private:
  std::size_t _maxBlockCapacity;
  std::size_t _maxDepth;
  vector_t<T> _selectionContent;
  ::std::function<void(T&, OctreeBlock<T>&)> _creationFunc;

}; // end of class Octree

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_OCTREES_OCTREE_H
