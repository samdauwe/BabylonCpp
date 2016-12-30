#include <babylon/culling/octrees/octree.h>

#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/octrees/octree_block.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

template <class T>
Octree<T>::Octree()
{
}

template <class T>
Octree<T>::Octree(
  const std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc,
  size_t maxBlockCapacity, size_t maxDepth)
    : _maxBlockCapacity{maxBlockCapacity}
    , _maxDepth{maxDepth}
    , _creationFunc{creationFunc}
{
  _selectionContent.resize(1024);
}

template <class T>
Octree<T>::~Octree()
{
}

template <class T>
void Octree<T>::update(const Vector3& worldMin, const Vector3& worldMax,
                       std::vector<T>& entries)
{
  Octree<T>::_CreateBlocks(worldMin, worldMax, entries, _maxBlockCapacity, 0,
                           _maxDepth, *this, _creationFunc);
}

template <class T>
void Octree<T>::addMesh(T& entry)
{
  for (auto& block : IOctreeContainer<T>::blocks) {
    block.addEntry(entry);
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
    std_util::concat(_selectionContent, dynamicContent);
  }
  else {
    std_util::concat_with_no_duplicates(_selectionContent, dynamicContent);
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
    std_util::concat(_selectionContent, dynamicContent);
  }
  else {
    std_util::concat_with_no_duplicates(_selectionContent, dynamicContent);
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

  std_util::concat_with_no_duplicates(_selectionContent, dynamicContent);

  return _selectionContent;
}

template <class T>
void Octree<T>::_CreateBlocks(
  const Vector3& worldMin, const Vector3& worldMax, std::vector<T>& entries,
  size_t maxBlockCapacity, size_t currentDepth, size_t maxDepth,
  IOctreeContainer<T>& target,
  std::function<void(T& entry, OctreeBlock<T>& block)>& creationFunc)
{
  target.blocks.clear();
  Vector3 blockSize((worldMax.x - worldMin.x) / 2.f,
                    (worldMax.y - worldMin.y) / 2.f,
                    (worldMax.z - worldMin.z) / 2.f);
  // Segmenting space
  for (unsigned int x = 0; x < 2; ++x) {
    for (unsigned int y = 0; y < 2; ++y) {
      for (unsigned int z = 0; z < 2; ++z) {
        Vector3 localMin = worldMin.add(blockSize.multiplyByFloats(x, y, z));
        Vector3 localMax
          = worldMin.add(blockSize.multiplyByFloats(x + 1, y + 1, z + 1));

        OctreeBlock<T> block(localMin, localMax, maxBlockCapacity,
                             currentDepth + 1, maxDepth, creationFunc);
        block.addEntries(entries);
        target.blocks.emplace_back(block);
      }
    }
  }
}

template <class T>
void Octree<T>::CreationFuncForMeshes(AbstractMesh* entry,
                                      OctreeBlock<AbstractMesh*>& block)
{
  if (!entry->isBlocked()
      && entry->getBoundingInfo()->boundingBox.intersectsMinMax(
           block.minPoint(), block.maxPoint())) {
    block.entries.emplace_back(entry);
  }
}

template <class T>
void Octree<T>::CreationFuncForSubMeshes(SubMesh* entry,
                                         OctreeBlock<SubMesh*>& block)
{
  if (entry->getBoundingInfo()->boundingBox.intersectsMinMax(
        block.minPoint(), block.maxPoint())) {
    block.entries.emplace_back(entry);
  }
}

template class Octree<AbstractMesh*>;
template class Octree<SubMesh*>;

} // end of namespace BABYLON
