#include <babylon/culling/octrees/octree.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/octrees/octree_block.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>

namespace BABYLON {

template <class T>
Octree<T>::Octree() = default;

template <class T>
Octree<T>::Octree(
  const std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc,
  size_t maxBlockCapacity, size_t iMaxDepth)
    : maxDepth{iMaxDepth}
    , _maxBlockCapacity{maxBlockCapacity}
    , _creationFunc{creationFunc}
{
  _selectionContent.resize(1024);
}

template <class T>
Octree<T>::~Octree() = default;

template <class T>
void Octree<T>::update(const Vector3& worldMin, const Vector3& worldMax,
                       std::vector<T>& entries)
{
  OctreeBlock<T>::_CreateBlocks(worldMin, worldMax, entries, _maxBlockCapacity,
                                0, maxDepth, *this, _creationFunc);
}

template <class T>
void Octree<T>::addMesh(T& entry)
{
  for (auto& block : IOctreeContainer<T>::blocks) {
    block.addEntry(entry);
  }
}

template <class T>
void Octree<T>::removeMesh(T& entry)
{
  for (auto& block : IOctreeContainer<T>::blocks) {
    block.removeEntry(entry);
  }
}

template <class T>
std::vector<T>& Octree<T>::select(const std::array<Plane, 6>& frustumPlanes,
                                  bool allowDuplicate)
{
  _selectionContent.clear();

  for (auto& block : IOctreeContainer<T>::blocks) {
    block.select(frustumPlanes, _selectionContent, allowDuplicate);
  }

  if (allowDuplicate) {
    stl_util::concat(_selectionContent, dynamicContent);
  }
  else {
    stl_util::concat_with_no_duplicates(_selectionContent, dynamicContent);
  }

  return _selectionContent;
}

template <class T>
std::vector<T>& Octree<T>::intersects(const Vector3& sphereCenter,
                                      float sphereRadius, bool allowDuplicate)
{
  _selectionContent.clear();

  for (auto& block : IOctreeContainer<T>::blocks) {
    block.intersects(sphereCenter, sphereRadius, _selectionContent,
                     allowDuplicate);
  }

  if (allowDuplicate) {
    stl_util::concat(_selectionContent, dynamicContent);
  }
  else {
    stl_util::concat_with_no_duplicates(_selectionContent, dynamicContent);
  }

  return _selectionContent;
}

template <class T>
std::vector<T>& Octree<T>::intersectsRay(const Ray& ray)
{
  _selectionContent.clear();

  for (auto& block : IOctreeContainer<T>::blocks) {
    block.intersectsRay(ray, _selectionContent);
  }

  stl_util::concat_with_no_duplicates(_selectionContent, dynamicContent);

  return _selectionContent;
}

template <class T>
void Octree<T>::CreationFuncForMeshes(AbstractMesh* entry,
                                      OctreeBlock<AbstractMesh*>& block)
{
  const auto boundingInfo = entry->getBoundingInfo();
  if (!entry->isBlocked()
      && boundingInfo->boundingBox.intersectsMinMax(block.minPoint(),
                                                    block.maxPoint())) {
    block.entries.emplace_back(entry);
  }
}

template <class T>
void Octree<T>::CreationFuncForSubMeshes(SubMesh* entry,
                                         OctreeBlock<SubMesh*>& block)
{
  const auto boundingInfo = entry->getBoundingInfo();
  if (boundingInfo->boundingBox.intersectsMinMax(block.minPoint(),
                                                 block.maxPoint())) {
    block.entries.emplace_back(entry);
  }
}

template class Octree<AbstractMesh*>;
template class Octree<SubMesh*>;

} // end of namespace BABYLON
