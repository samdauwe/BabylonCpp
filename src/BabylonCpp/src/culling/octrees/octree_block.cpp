#include <babylon/culling/octrees/octree_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>

namespace BABYLON {

template <class T>
OctreeBlock<T>::OctreeBlock(
  const Vector3& iMinPoint, const Vector3& iMaxPoint, size_t iCapacity,
  size_t depth, size_t maxDepth,
  const std::function<void(T&, OctreeBlock<T>&)>& creationFunc)
{
  _capacity     = iCapacity;
  _depth        = depth;
  _maxDepth     = maxDepth;
  _creationFunc = creationFunc;

  _minPoint = iMinPoint;
  _maxPoint = iMaxPoint;

  _boundingVectors[0] = _minPoint;
  _boundingVectors[1] = _maxPoint;

  _boundingVectors[2]   = _minPoint;
  _boundingVectors[2].x = _maxPoint.x;

  _boundingVectors[3]   = _minPoint;
  _boundingVectors[3].y = _maxPoint.y;

  _boundingVectors[4]   = _minPoint;
  _boundingVectors[4].z = _maxPoint.z;

  _boundingVectors[5]   = _maxPoint;
  _boundingVectors[5].z = _minPoint.z;

  _boundingVectors[6]   = _maxPoint;
  _boundingVectors[6].x = _minPoint.x;

  _boundingVectors[7]   = _maxPoint;
  _boundingVectors[7].y = _minPoint.y;
}

template <class T>
OctreeBlock<T>::~OctreeBlock() = default;

template <class T>
size_t OctreeBlock<T>::capacity() const
{
  return _capacity;
}

template <class T>
Vector3& OctreeBlock<T>::minPoint()
{
  return _minPoint;
}

template <class T>
Vector3& OctreeBlock<T>::maxPoint()
{
  return _maxPoint;
}

template <class T>
void OctreeBlock<T>::addEntry(T& entry)
{
  if (!IOctreeContainer<T>::blocks.empty()) {
    for (auto& block : IOctreeContainer<T>::blocks) {
      block.addEntry(entry);
    }
    return;
  }

  _creationFunc(entry, *this);

  if (entries.size() > _capacity && _depth < _maxDepth) {
    createInnerBlocks();
  }
}

template <class T>
void OctreeBlock<T>::removeEntry(T& entry)
{
  if (!IOctreeContainer<T>::blocks.empty()) {
    for (auto& block : IOctreeContainer<T>::blocks) {
      block.removeEntry(entry);
    }
    return;
  }

  const auto entryIndex = std::find(entries.begin(), entries.end(), entry);

  if (entryIndex != entries.end()) {
    entries.erase(entryIndex);
  }
}

template <class T>
void OctreeBlock<T>::addEntries(std::vector<T>& _entries)
{
  for (auto& mesh : _entries) {
    addEntry(mesh);
  }
}

template <class T>
void OctreeBlock<T>::select(const std::array<Plane, 6>& frustumPlanes,
                            std::vector<T>& selection, bool allowDuplicate)
{
  if (BoundingBox::IsInFrustum(_boundingVectors, frustumPlanes)) {
    if (!IOctreeContainer<T>::blocks.empty()) {
      for (auto& block : IOctreeContainer<T>::blocks) {
        block.select(frustumPlanes, selection, allowDuplicate);
      }
      return;
    }

    if (allowDuplicate) {
      stl_util::concat(selection, entries);
    }
    else {
      stl_util::concat_with_no_duplicates(selection, entries);
    }
  }
}

template <class T>
void OctreeBlock<T>::intersects(const Vector3& sphereCenter, float sphereRadius,
                                std::vector<T>& selection, bool allowDuplicate)
{
  if (BoundingBox::IntersectsSphere(_minPoint, _maxPoint, sphereCenter,
                                    sphereRadius)) {
    if (!IOctreeContainer<T>::blocks.empty()) {
      for (auto& block : IOctreeContainer<T>::blocks) {
        block.intersects(sphereCenter, sphereRadius, selection, allowDuplicate);
      }
      return;
    }

    if (allowDuplicate) {
      stl_util::concat(selection, entries);
    }
    else {
      stl_util::concat_with_no_duplicates(selection, entries);
    }
  }
}

template <class T>
void OctreeBlock<T>::intersectsRay(const Ray& ray, std::vector<T>& selection)
{
  if (ray.intersectsBoxMinMax(_minPoint, _maxPoint)) {
    if (!IOctreeContainer<T>::blocks.empty()) {
      for (auto& block : IOctreeContainer<T>::blocks) {
        block.intersectsRay(ray, selection);
      }
      return;
    }
    stl_util::concat_with_no_duplicates(selection, entries);
  }
}

template <class T>
void OctreeBlock<T>::createInnerBlocks()
{
  OctreeBlock<T>::_CreateBlocks(_minPoint, _maxPoint, entries, _capacity,
                                _depth, _maxDepth, *this, _creationFunc);
}

template <class T>
void OctreeBlock<T>::_CreateBlocks(
  const Vector3& worldMin, const Vector3& worldMax, std::vector<T>& entries,
  size_t maxBlockCapacity, size_t currentDepth, size_t maxDepth,
  IOctreeContainer<T>& target,
  const std::function<void(T&, OctreeBlock<T>&)>& creationFunc)
{
  target.blocks.clear();
  Vector3 blockSize((worldMax.x - worldMin.x) / 2.f,
                    (worldMax.y - worldMin.y) / 2.f,
                    (worldMax.z - worldMin.z) / 2.f);

  // Segmenting space
  for (float x = 0.f; x < 2.f; ++x) {
    for (float y = 0.f; y < 2.f; ++y) {
      for (float z = 0.f; z < 2.f; ++z) {
        const auto& localMin
          = worldMin.add(blockSize.multiplyByFloats(x, y, z));
        const auto& localMax
          = worldMin.add(blockSize.multiplyByFloats(x + 1.f, y + 1.f, z + 1.f));

        OctreeBlock<T> block(localMin, localMax, maxBlockCapacity,
                             currentDepth + 1, maxDepth, creationFunc);
        block.addEntries(entries);
        target.blocks.emplace_back(block);
      }
    }
  }
}

template class OctreeBlock<AbstractMesh*>;
template class OctreeBlock<SubMesh*>;

} // end of namespace BABYLON
