#include <babylon/meshes/sub_mesh.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/intersection_info.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/standard_material_defines.h>
#include <babylon/math/functions.h>
#include <babylon/math/plane.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_lines_mesh.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/transform_node.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

SubMesh::SubMesh(unsigned int iMaterialIndex, unsigned int iVerticesStart, size_t iVerticesCount,
                 unsigned int iIndexStart, size_t iIndexCount, const AbstractMeshPtr& mesh,
                 const MeshPtr& renderingMesh, bool iCreateBoundingBox)
    : materialIndex{iMaterialIndex}
    , verticesStart{iVerticesStart}
    , verticesCount{iVerticesCount}
    , indexStart{iIndexStart}
    , indexCount{iIndexCount}
    , createBoundingBox{iCreateBoundingBox}
    , _linesIndexCount{0}
    , _lastColliderTransformMatrix{nullptr}
    , _renderId{0}
    , _alphaIndex{0}
    , _distanceToCamera{0.f}
    , _mesh{mesh}
    , _boundingInfo{nullptr}
    , _linesIndexBuffer{nullptr}
    , _currentMaterial{nullptr}
{
  _renderingMesh
    = renderingMesh ? renderingMesh : std::static_pointer_cast<Mesh>(mesh);

  _id = mesh->subMeshes.size() /*- 1*/; // Submesh is not yet to the list

  if (createBoundingBox) {
    refreshBoundingInfo();
    mesh->computeWorldMatrix(true);
  }
}

SubMesh::~SubMesh() = default;

void SubMesh::addToMesh(const std::shared_ptr<SubMesh>& newSubMesh)
{
  _mesh->subMeshes.emplace_back(newSubMesh);
}

SubMeshPtr SubMesh::AddToMesh(unsigned int materialIndex,
                              unsigned int verticesStart, size_t verticesCount,
                              unsigned int indexStart, size_t indexCount,
                              const AbstractMeshPtr& mesh,
                              const MeshPtr& renderingMesh,
                              bool createBoundingBox)
{
  return SubMesh::New(materialIndex, verticesStart, verticesCount, indexStart,
                      indexCount, mesh, renderingMesh, createBoundingBox);
}

bool SubMesh::isGlobal() const
{
  return (verticesStart == 0 && verticesCount == _mesh->getTotalVertices());
}

BoundingInfoPtr& SubMesh::getBoundingInfo()
{
  if (isGlobal()) {
    return _mesh->getBoundingInfo();
  }

  return _boundingInfo;
}

SubMesh& SubMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

AbstractMeshPtr& SubMesh::getMesh()
{
  return _mesh;
}

MeshPtr& SubMesh::getRenderingMesh()
{
  return _renderingMesh;
}

MaterialPtr SubMesh::getMaterial()
{
  auto rootMaterial = _renderingMesh->getMaterial();

  if (!rootMaterial) {
    return _mesh->getScene()->defaultMaterial();
  }
  else if (rootMaterial && (rootMaterial->type() == Type::MULTIMATERIAL)) {
    auto multiMaterial = std::static_pointer_cast<MultiMaterial>(rootMaterial);
    auto effectiveMaterial = multiMaterial->getSubMaterial(materialIndex);

    if (_currentMaterial != effectiveMaterial) {
      _currentMaterial = effectiveMaterial;
      _materialDefines = nullptr;
    }

    return effectiveMaterial;
  }

  return rootMaterial;
}

// Methods
SubMesh& SubMesh::refreshBoundingInfo(const Float32Array& iData)
{
  _lastColliderWorldVertices.clear();

  if (isGlobal() || !_renderingMesh || !_renderingMesh->geometry()) {
    return *this;
  }

  auto data = !iData.empty() ?
                iData :
                _renderingMesh->getVerticesData(VertexBuffer::PositionKind);

  if (data.empty()) {
    _boundingInfo = std::make_unique<BoundingInfo>(*_mesh->_boundingInfo);
    return *this;
  }

  auto indices = _renderingMesh->getIndices();
  MinMax extend;

  // Is this the only submesh?
  if (indexStart == 0 && indexCount == indices.size()) {
    const auto& boundingInfo = *_renderingMesh->getBoundingInfo();

    // the rendering mesh's bounding info can be used, it is the standard
    // submesh for all indices.
    extend = {
      boundingInfo.minimum, // minimum
      boundingInfo.maximum  // maximum
    };
  }
  else {
    extend
      = extractMinAndMaxIndexed(data, indices, indexStart, indexCount,
                                *_renderingMesh->geometry()->boundingBias());
  }

  if (_boundingInfo) {
    _boundingInfo->reConstruct(extend.min, extend.max);
  }
  else {
    _boundingInfo = std::make_shared<BoundingInfo>(extend.min, extend.max);
  }

  return *this;
}

bool SubMesh::_checkCollision(const Collider& collider)
{
  const auto& boundingInfo = *getBoundingInfo();

  return boundingInfo._checkCollision(collider);
}

SubMesh& SubMesh::updateBoundingInfo(const Matrix& world)
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    refreshBoundingInfo();
    boundingInfo = getBoundingInfo();
  }
  boundingInfo->update(world);

  return *this;
}

bool SubMesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                          unsigned int /*strategy*/)
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    return false;
  }

  return boundingInfo->isInFrustum(frustumPlanes, _mesh->cullingStrategy);
}

bool SubMesh::isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    return false;
  }

  return boundingInfo->isCompletelyInFrustum(frustumPlanes);
}

SubMesh& SubMesh::render(bool enableAlphaMode)
{
  _renderingMesh->render(this, enableAlphaMode);
  return *this;
}

std::unique_ptr<GL::IGLBuffer>&
SubMesh::_getLinesIndexBuffer(const IndicesArray& indices, Engine* engine)
{
  if (!_linesIndexBuffer) {
    Uint32Array linesIndices;

    for (auto index = indexStart; index < indexStart + indexCount; index += 3) {
      stl_util::concat(linesIndices, {indices[index + 0], indices[index + 1],
                                      indices[index + 1], indices[index + 2],
                                      indices[index + 2], indices[index + 0]});
    }

    _linesIndexBuffer = engine->createIndexBuffer(linesIndices);
    _linesIndexCount  = linesIndices.size();
  }
  return _linesIndexBuffer;
}

bool SubMesh::canIntersects(const Ray& ray)
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    return false;
  }

  return ray.intersectsBox(boundingInfo->boundingBox);
}

std::optional<IntersectionInfo>
SubMesh::intersects(Ray& ray, const std::vector<Vector3>& positions,
                    const Uint32Array& indices, bool fastCheck,
                    const TrianglePickingPredicate& trianglePredicate)
{
  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  const auto material = getMaterial();
  if (!material) {
    return intersectInfo;
  }

  switch (material->fillMode()) {
    case Constants::MATERIAL_PointListDrawMode:
    case Constants::MATERIAL_LineListDrawMode:
    case Constants::MATERIAL_LineLoopDrawMode:
    case Constants::MATERIAL_LineStripDrawMode:
    case Constants::MATERIAL_TriangleFanDrawMode:
    case Constants::MATERIAL_TriangleStripDrawMode:
      return std::nullopt;
  }

  // LineMesh first as it's also a Mesh...
  if (_mesh->getClassName() == "InstancedLinesMesh"
      || _mesh->getClassName() == "LinesMesh") {
    auto intersectionThreshold
      = std::static_pointer_cast<LinesMesh>(_mesh)->intersectionThreshold;
    // Check if mesh is unindexed
    if (indices.empty()) {
      return _intersectUnIndexedLines(ray, positions, indices,
                                      intersectionThreshold, fastCheck);
    }
    return _intersectLines(ray, positions, indices, intersectionThreshold,
                           fastCheck);
  }
  else {
    // Check if mesh is unindexed
    if (indices.empty() && _mesh->_unIndexed) {
      return _intersectUnIndexedTriangles(ray, positions, indices, fastCheck,
                                          trianglePredicate);
    }

    return _intersectTriangles(ray, positions, indices, fastCheck,
                               trianglePredicate);
  }
}

std::optional<IntersectionInfo>
SubMesh::_intersectLines(Ray& ray, const std::vector<Vector3>& positions,
                         const IndicesArray& indices,
                         float intersectionThreshold, bool fastCheck)
{
  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  // Line test
  for (auto index = indexStart; index < indexStart + indexCount; index += 2) {
    const auto& p0 = positions[indices[index]];
    const auto& p1 = positions[indices[index + 1]];

    const auto length = ray.intersectionSegment(p0, p1, intersectionThreshold);
    if (length < 0.f) {
      continue;
    }

    if (fastCheck || !intersectInfo || length < intersectInfo->distance) {
      intersectInfo         = IntersectionInfo(0.f, 0.f, length);
      intersectInfo->faceId = index / 2;
      if (fastCheck) {
        break;
      }
    }
  }

  return intersectInfo;
}

std::optional<IntersectionInfo> SubMesh::_intersectUnIndexedLines(
  Ray& ray, const std::vector<Vector3>& positions,
  const IndicesArray& /*indices*/, float intersectionThreshold, bool fastCheck)
{
  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  // Line test
  for (auto index = verticesStart; index < verticesStart + verticesCount;
       index += 2) {
    const auto& p0 = positions[index];
    const auto& p1 = positions[index + 1];

    const auto length = ray.intersectionSegment(p0, p1, intersectionThreshold);
    if (length < 0.f) {
      continue;
    }

    if (fastCheck || !intersectInfo || length < intersectInfo->distance) {
      intersectInfo         = IntersectionInfo(0.f, 0.f, length);
      intersectInfo->faceId = index / 2;
      if (fastCheck) {
        break;
      }
    }
  }

  return intersectInfo;
}

std::optional<IntersectionInfo>
SubMesh::_intersectTriangles(Ray& ray, const std::vector<Vector3>& positions,
                             const IndicesArray& indices, bool fastCheck,
                             const TrianglePickingPredicate& trianglePredicate)
{
  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  // Triangles test
  for (auto index = indexStart; index < indexStart + indexCount; index += 3) {
    const auto& p0 = positions[indices[index]];
    const auto& p1 = positions[indices[index + 1]];
    const auto& p2 = positions[indices[index + 2]];

    if (trianglePredicate && !trianglePredicate(p0, p1, p2, ray)) {
      continue;
    }

    const auto currentIntersectInfo = ray.intersectsTriangle(p0, p1, p2);

    if (currentIntersectInfo) {
      if (currentIntersectInfo->distance < 0.f) {
        continue;
      }

      if (fastCheck || !intersectInfo
          || currentIntersectInfo->distance < intersectInfo->distance) {
        intersectInfo         = currentIntersectInfo;
        intersectInfo->faceId = index / 3;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  return intersectInfo;
}

std::optional<IntersectionInfo> SubMesh::_intersectUnIndexedTriangles(
  Ray& ray, const std::vector<Vector3>& positions,
  const IndicesArray& /*indices*/, bool fastCheck,
  const TrianglePickingPredicate& trianglePredicate)
{
  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  // Triangles test
  for (auto index = indexStart; index < indexStart + indexCount; index += 3) {
    const auto& p0 = positions[index];
    const auto& p1 = positions[index + 1];
    const auto& p2 = positions[index + 2];

    if (trianglePredicate && !trianglePredicate(p0, p1, p2, ray)) {
      continue;
    }

    const auto currentIntersectInfo = ray.intersectsTriangle(p0, p1, p2);

    if (currentIntersectInfo) {
      if (currentIntersectInfo->distance < 0.f) {
        continue;
      }

      if (fastCheck || !intersectInfo
          || currentIntersectInfo->distance < intersectInfo->distance) {
        intersectInfo         = currentIntersectInfo;
        intersectInfo->faceId = index / 3;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  return intersectInfo;
}

void SubMesh::_rebuild()
{
  if (_linesIndexBuffer) {
    _linesIndexBuffer = nullptr;
  }
}

// Clone
SubMeshPtr SubMesh::clone(const AbstractMeshPtr& newMesh,
                          const MeshPtr& newRenderingMesh)
{
  auto result
    = SubMesh::New(materialIndex, verticesStart, verticesCount, indexStart,
                   indexCount, newMesh, newRenderingMesh, false);

  if (!isGlobal()) {
    auto boundingInfo = getBoundingInfo();

    if (!boundingInfo) {
      return result;
    }

    result->_boundingInfo = std::make_shared<BoundingInfo>(
      boundingInfo->minimum, boundingInfo->maximum);
  }

  return result;
}

// Dispose
void SubMesh::dispose()
{
  if (_linesIndexBuffer) {
    _mesh->getScene()->getEngine()->_releaseBuffer(_linesIndexBuffer.get());
    _linesIndexBuffer.reset(nullptr);
  }

  // Remove from mesh
  stl_util::remove_vector_elements_equal_sharedptr(_mesh->subMeshes, this);
}

std::string SubMesh::getClassName() const
{
  return "SubMesh";
}

SubMeshPtr SubMesh::CreateFromIndices(unsigned int materialIndex,
                                      unsigned int startIndex,
                                      size_t indexCount,
                                      const AbstractMeshPtr& mesh,
                                      const MeshPtr& renderingMesh)
{
  auto minVertexIndex = std::numeric_limits<unsigned>::max();
  auto maxVertexIndex = std::numeric_limits<unsigned>::lowest();

  auto whatWillRender
    = renderingMesh ? renderingMesh : std::static_pointer_cast<Mesh>(mesh);
  auto indices = whatWillRender->getIndices();

  for (size_t index = startIndex; index < startIndex + indexCount; ++index) {
    auto& vertexIndex = indices[index];

    if (vertexIndex < minVertexIndex) {
      minVertexIndex = vertexIndex;
    }
    if (vertexIndex > maxVertexIndex) {
      maxVertexIndex = vertexIndex;
    }
  }

  return SubMesh::New(materialIndex, minVertexIndex,
                      maxVertexIndex - minVertexIndex + 1, startIndex,
                      indexCount, mesh, renderingMesh);
}

} // end of namespace BABYLON
