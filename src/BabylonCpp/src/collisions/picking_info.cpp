#include <babylon/collisions/picking_info.h>

#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

PickingInfo::PickingInfo()
    : hit{false}
    , distance{0.f}
    , pickedPoint{nullptr}
    , pickedMesh{nullptr}
    , bu{0.f}
    , bv{0.f}
    , faceId{0}
    , subMeshId{0}
    , pickedSprite{nullptr}
    , originMesh{nullptr}
    , ray{nullptr}
{
}

PickingInfo::~PickingInfo()
{
}

Nullable<Vector3> PickingInfo::getNormal(bool useWorldCoordinates,
                                         bool useVerticesNormals)
{
  if (!pickedMesh
      || !pickedMesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return nullptr;
  }

  auto indices = pickedMesh->getIndices();

  if (indices.empty()) {
    return nullptr;
  }

  Vector3 result;

  if (useVerticesNormals) {
    auto normals = pickedMesh->getVerticesData(VertexBuffer::NormalKind);

    auto normal0 = Vector3::FromArray(normals, indices[faceId * 3] * 3);
    auto normal1 = Vector3::FromArray(normals, indices[faceId * 3 + 1] * 3);
    auto normal2 = Vector3::FromArray(normals, indices[faceId * 3 + 2] * 3);

    normal0 = normal0.scale(bu);
    normal1 = normal1.scale(bv);
    normal2 = normal2.scale(1.f - bu - bv);

    result = Vector3(normal0.x + normal1.x + normal2.x,
                     normal0.y + normal1.y + normal2.y,
                     normal0.z + normal1.z + normal2.z);
  }
  else {
    auto positions = pickedMesh->getVerticesData(VertexBuffer::PositionKind);

    auto vertex1 = Vector3::FromArray(positions, indices[faceId * 3] * 3);
    auto vertex2 = Vector3::FromArray(positions, indices[faceId * 3 + 1] * 3);
    auto vertex3 = Vector3::FromArray(positions, indices[faceId * 3 + 2] * 3);

    auto p1p2 = vertex1.subtract(vertex2);
    auto p3p2 = vertex3.subtract(vertex2);

    result = Vector3::Cross(p1p2, p3p2);
  }

  if (useWorldCoordinates) {
    result = Vector3::TransformNormal(result, *pickedMesh->getWorldMatrix());
  }

  return Vector3::Normalize(result);
}

Nullable<Vector2> PickingInfo::getTextureCoordinates()
{
  if (!pickedMesh || !pickedMesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
    return nullptr;
  }

  const auto indices = pickedMesh->getIndices();
  if (indices.empty()) {
    return nullptr;
  }

  const auto uvs = pickedMesh->getVerticesData(VertexBuffer::UVKind);
  if (uvs.empty()) {
    return nullptr;
  }

  auto uv0 = Vector2::FromArray(uvs, indices[faceId * 3] * 2);
  auto uv1 = Vector2::FromArray(uvs, indices[faceId * 3 + 1] * 2);
  auto uv2 = Vector2::FromArray(uvs, indices[faceId * 3 + 2] * 2);

  uv0 = uv0.scale(1.f - bu - bv);
  uv1 = uv1.scale(bu);
  uv2 = uv2.scale(bv);

  return Vector2(uv0.x + uv1.x + uv2.x, uv0.y + uv1.y + uv2.y);
}

} // end of namespace BABYLON
