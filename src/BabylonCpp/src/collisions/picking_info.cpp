#include <babylon/collisions/picking_info.h>

#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

PickingInfo::PickingInfo()
    : hit{false}
    , distance{0.f}
    , pickedMesh{nullptr}
    , bu{0.f}
    , bv{0.f}
    , faceId{0}
    , subMeshId{0}
    , pickedSprite{nullptr}
{
}

PickingInfo::~PickingInfo()
{
}

Vector3 PickingInfo::getNormal(bool useWorldCoordinates,
                               bool useVerticesNormals)
{
  if (!pickedMesh
      || !pickedMesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return Vector3();
  }

  Uint32Array indices = pickedMesh->getIndices();
  Vector3 result;

  if (useVerticesNormals) {
    Float32Array normals
      = pickedMesh->getVerticesData(VertexBuffer::NormalKind);

    Vector3 normal0 = Vector3::FromArray(normals, indices[faceId * 3] * 3);
    Vector3 normal1 = Vector3::FromArray(normals, indices[faceId * 3 + 1] * 3);
    Vector3 normal2 = Vector3::FromArray(normals, indices[faceId * 3 + 2] * 3);

    normal0 = normal0.scale(bu);
    normal1 = normal1.scale(bv);
    normal2 = normal2.scale(1.f - bu - bv);

    result = Vector3(normal0.x + normal1.x + normal2.x,
                     normal0.y + normal1.y + normal2.y,
                     normal0.z + normal1.z + normal2.z);
  }
  else {
    Float32Array positions
      = pickedMesh->getVerticesData(VertexBuffer::PositionKind);

    Vector3 vertex1 = Vector3::FromArray(positions, indices[faceId * 3] * 3);
    Vector3 vertex2
      = Vector3::FromArray(positions, indices[faceId * 3 + 1] * 3);
    Vector3 vertex3
      = Vector3::FromArray(positions, indices[faceId * 3 + 2] * 3);

    Vector3 p1p2 = vertex1.subtract(vertex2);
    Vector3 p3p2 = vertex3.subtract(vertex2);

    result = Vector3::Cross(p1p2, p3p2);
  }

  if (useWorldCoordinates) {
    result = Vector3::TransformNormal(result, *pickedMesh->getWorldMatrix());
  }

  return Vector3::Normalize(result);
}

Vector2 PickingInfo::getTextureCoordinates()
{
  if (!pickedMesh || !pickedMesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
    return Vector2();
  }

  Uint32Array indices = pickedMesh->getIndices();
  Float32Array uvs    = pickedMesh->getVerticesData(VertexBuffer::UVKind);

  Vector2 uv0 = Vector2::FromArray(uvs, indices[faceId * 3] * 2);
  Vector2 uv1 = Vector2::FromArray(uvs, indices[faceId * 3 + 1] * 2);
  Vector2 uv2 = Vector2::FromArray(uvs, indices[faceId * 3 + 2] * 2);

  uv0 = uv0.scale(1.f - bu - bv);
  uv1 = uv1.scale(bu);
  uv2 = uv2.scale(bv);

  return Vector2(uv0.x + uv1.x + uv2.x, uv0.y + uv1.y + uv2.y);
}

} // end of namespace BABYLON
