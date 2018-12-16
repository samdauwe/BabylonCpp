#include <babylon/culling/octrees/octree_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

template <class T>
OctreeBlock<T>::OctreeBlock(
  const Vector3& iMinPoint, const Vector3& iMaxPoint, size_t iCapacity,
  size_t depth, size_t maxDepth,
  const std::function<void(T&, OctreeBlock<T>&)>& creationFunc)
    : _depth{depth}
    , _maxDepth{maxDepth}
    , _capacity{iCapacity}
    , _minPoint{iMinPoint}
    , _maxPoint{iMaxPoint}
    , _creationFunc{creationFunc}
{
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
OctreeBlock<T>::~OctreeBlock()
{
}

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
  Octree<T>::_CreateBlocks(_minPoint, _maxPoint, entries, _capacity, _depth,
                           _maxDepth, *this, _creationFunc);
}

template class OctreeBlock<AbstractMesh*>;
template class OctreeBlock<SubMesh*>;

} // end of namespace BABYLON
