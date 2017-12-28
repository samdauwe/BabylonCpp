#include <babylon/mesh/sub_mesh.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/intersection_info.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/standard_material_defines.h>
#include <babylon/math/plane.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SubMesh::SubMesh(unsigned int iMaterialIndex, unsigned int iVerticesStart,
                 size_t iVerticesCount, unsigned int iIndexStart,
                 size_t iIndexCount, AbstractMesh* mesh, Mesh* renderingMesh,
                 bool iCreateBoundingBox)
    : BaseSubMesh{}
    , materialIndex{iMaterialIndex}
    , verticesStart{iVerticesStart}
    , verticesCount{iVerticesCount}
    , indexStart{iIndexStart}
    , indexCount{iIndexCount}
    , createBoundingBox{iCreateBoundingBox}
    , _renderId{0}
    , _mesh{mesh}
    , _renderingMesh{renderingMesh}
    , _boundingInfo{nullptr}
    , _linesIndexBuffer{nullptr}
    , _currentMaterial{nullptr}
{
  if (!renderingMesh) {
    _renderingMesh = static_cast<Mesh*>(mesh);
  }

  _id = mesh->subMeshes.size() - 1;

  if (createBoundingBox) {
    refreshBoundingInfo();
    mesh->computeWorldMatrix(true);
  }
}

SubMesh::~SubMesh()
{
}

void SubMesh::addToMesh(unique_ptr_t<SubMesh>&& newSubMesh)
{
  _mesh->subMeshes.emplace_back(::std::move(newSubMesh));
}

SubMesh* SubMesh::AddToMesh(unsigned int materialIndex,
                            unsigned int verticesStart, size_t verticesCount,
                            unsigned int indexStart, size_t indexCount,
                            AbstractMesh* mesh, Mesh* renderingMesh,
                            bool createBoundingBox)
{
  return SubMesh::New(materialIndex, verticesStart, verticesCount, indexStart,
                      indexCount, mesh, renderingMesh, createBoundingBox);
}

bool SubMesh::isGlobal() const
{
  return (verticesStart == 0 && verticesCount == _mesh->getTotalVertices());
}

BoundingInfo* SubMesh::getBoundingInfo() const
{
  if (isGlobal()) {
    return _mesh->getBoundingInfo();
  }

  return _boundingInfo.get();
}

SubMesh& SubMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = ::std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

AbstractMesh* SubMesh::getMesh()
{
  return _mesh;
}

Mesh* SubMesh::getRenderingMesh()
{
  return _renderingMesh;
}

Material* SubMesh::getMaterial()
{
  auto rootMaterial = _renderingMesh->getMaterial();

  if (rootMaterial && (rootMaterial->type() == IReflect::Type::MULTIMATERIAL)) {
    auto multiMaterial     = static_cast<MultiMaterial*>(rootMaterial);
    auto effectiveMaterial = multiMaterial->getSubMaterial(materialIndex);
    if (_currentMaterial != effectiveMaterial) {
      _currentMaterial = effectiveMaterial;
      _materialDefines.reset(nullptr);
    }

    return effectiveMaterial;
  }

  if (!rootMaterial) {
    return _mesh->getScene()->defaultMaterial();
  }

  return rootMaterial;
}

// Methods
SubMesh& SubMesh::refreshBoundingInfo()
{
  _lastColliderWorldVertices.clear();

  if (isGlobal() || !_renderingMesh || !_renderingMesh->geometry()) {
    return *this;
  }

  auto data = _renderingMesh->getVerticesData(VertexBuffer::PositionKind);

  if (data.empty()) {
    _boundingInfo = ::std::make_unique<BoundingInfo>(*_mesh->_boundingInfo);
    return *this;
  }

  auto indices = _renderingMesh->getIndices();
  MinMax extend;

  // Is this the only submesh?
  if (indexStart == 0 && indexCount == indices.size()) {
    auto boundingInfo = _renderingMesh->getBoundingInfo();
    // the rendering mesh's bounding info can be used, it is the standard
    // submesh for all indices.
    extend.min = boundingInfo->minimum;
    extend.max = boundingInfo->maximum;
  }
  else {
    extend = Tools::ExtractMinAndMaxIndexed(
      data, indices, indexStart, indexCount,
      _renderingMesh->geometry()->boundingBias());
  }
  _boundingInfo = ::std::make_unique<BoundingInfo>(extend.min, extend.max);

  return *this;
}

bool SubMesh::_checkCollision(const Collider& collider)
{
  auto boundingInfo = _renderingMesh->getBoundingInfo();

  return boundingInfo->_checkCollision(collider);
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

bool SubMesh::isInFrustum(const array_t<Plane, 6>& frustumPlanes)
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    return false;
  }

  return boundingInfo->isInFrustum(frustumPlanes);
}

bool SubMesh::isCompletelyInFrustum(
  const array_t<Plane, 6>& frustumPlanes) const
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

GL::IGLBuffer* SubMesh::getLinesIndexBuffer(const Uint32Array& indices,
                                            Engine* engine)
{
  if (!_linesIndexBuffer) {
    Uint32Array linesIndices;

    for (size_t index = indexStart; index < indexStart + indexCount;
         index += 3) {
      stl_util::concat(linesIndices,
                       {indices[index + 0], indices[index + 1],
                        indices[index + 1], indices[index + 2],
                        indices[index + 2], indices[index + 0]});
    }

    _linesIndexBuffer = engine->createIndexBuffer(linesIndices);
    linesIndexCount   = linesIndices.size();
  }
  return _linesIndexBuffer.get();
}

bool SubMesh::canIntersects(const Ray& ray) const
{
  auto boundingInfo = getBoundingInfo();

  if (!boundingInfo) {
    return false;
  }

  return ray.intersectsBox(boundingInfo->boundingBox);
}

unique_ptr_t<IntersectionInfo>
SubMesh::intersects(Ray& ray, const vector_t<Vector3>& positions,
                    const Uint32Array& indices, bool fastCheck)
{

  unique_ptr_t<IntersectionInfo> intersectInfo = nullptr;

  // LineMesh first as it's also a Mesh...
  if (_mesh->type() == IReflect::Type::LINESMESH) {
    auto lineMesh = static_cast<LinesMesh*>(_mesh);

    // Line test
    float length;
    for (size_t index = indexStart; index < indexStart + indexCount;
         index += 2) {
      const auto& p0 = positions[indices[index]];
      const auto& p1 = positions[indices[index + 1]];

      length
        = ray.intersectionSegment(p0, p1, lineMesh->intersectionThreshold());
      if (length < 0.f) {
        continue;
      }

      if (fastCheck || !intersectInfo || length < intersectInfo->distance) {
        intersectInfo = ::std::make_unique<IntersectionInfo>(length);

        if (fastCheck) {
          break;
        }
      }
    }
  }
  else {
    // Triangles test
    for (size_t index = indexStart; index < indexStart + indexCount;
         index += 3) {
      const auto& p0 = positions[indices[index]];
      const auto& p1 = positions[indices[index + 1]];
      const auto& p2 = positions[indices[index + 2]];

      auto currentIntersectInfo = ray.intersectsTriangle(p0, p1, p2);

      if (currentIntersectInfo) {
        if (currentIntersectInfo->distance < 0.f) {
          continue;
        }

        if (fastCheck || !intersectInfo
            || currentIntersectInfo->distance < intersectInfo->distance) {
          intersectInfo         = ::std::move(currentIntersectInfo);
          intersectInfo->faceId = index / 3;

          if (fastCheck) {
            break;
          }
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
SubMesh* SubMesh::clone(AbstractMesh* newMesh, Mesh* newRenderingMesh) const
{
  auto result
    = SubMesh::New(materialIndex, verticesStart, verticesCount, indexStart,
                   indexCount, newMesh, newRenderingMesh, false);

  if (!isGlobal()) {
    auto boundingInfo = getBoundingInfo();

    if (!boundingInfo) {
      return result;
    }

    result->_boundingInfo = ::std::make_unique<BoundingInfo>(
      boundingInfo->minimum, boundingInfo->maximum);
  }

  return result;
}

// Dispose
void SubMesh::dispose(bool /*doNotRecurse*/)
{
  if (_linesIndexBuffer) {
    _mesh->getScene()->getEngine()->_releaseBuffer(_linesIndexBuffer.get());
    _linesIndexBuffer.reset(nullptr);
  }

  // Remove from mesh
  _mesh->subMeshes.erase(
    ::std::remove_if(_mesh->subMeshes.begin(), _mesh->subMeshes.end(),
                     [this](const unique_ptr_t<SubMesh>& subMesh) {
                       return subMesh.get() == this;
                     }),
    _mesh->subMeshes.end());
}

// Statics
SubMesh* SubMesh::CreateFromIndices(unsigned int materialIndex,
                                    unsigned int startIndex, size_t indexCount,
                                    AbstractMesh* mesh, Mesh* renderingMesh)
{
  unsigned int minVertexIndex = numeric_limits_t<unsigned>::max();
  unsigned int maxVertexIndex = numeric_limits_t<unsigned>::min();

  auto _renderingMesh
    = renderingMesh ? renderingMesh : static_cast<Mesh*>(mesh);
  auto indices = _renderingMesh->getIndices();

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
                      indexCount, mesh, _renderingMesh);
}

} // end of namespace BABYLON
