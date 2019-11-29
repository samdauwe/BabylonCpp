#include <babylon/collisions/picking_info.h>

#include <babylon/maths/matrix.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/maths/vector2.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

PickingInfo::PickingInfo()
    : _pickingUnavailable{false}
    , hit{false}
    , distance{0.f}
    , pickedPoint{std::nullopt}
    , pickedMesh{nullptr}
    , bu{0.f}
    , bv{0.f}
    , faceId{0}
    , subMeshId{0}
    , pickedSprite{nullptr}
    , originMesh{nullptr}
    , ray{std::nullopt}
{
}

PickingInfo::PickingInfo(const PickingInfo& other) = default;

PickingInfo::PickingInfo(PickingInfo&& other) = default;

PickingInfo& PickingInfo::operator=(const PickingInfo& other) = default;

PickingInfo& PickingInfo::operator=(PickingInfo&& other) = default;

PickingInfo::~PickingInfo() = default;

std::optional<Vector3> PickingInfo::getNormal(bool useWorldCoordinates,
                                              bool useVerticesNormals)
{
  if (!pickedMesh
      || !pickedMesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return std::nullopt;
  }

  auto indices = pickedMesh->getIndices();

  if (indices.empty()) {
    return std::nullopt;
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
    auto wm = pickedMesh->getWorldMatrix();

    if (pickedMesh->nonUniformScaling()) {
      TmpVectors::MatrixArray[0].copyFrom(wm);
      wm = TmpVectors::MatrixArray[0];
      wm.setTranslationFromFloats(0.f, 0.f, 0.f);
      wm.invert();
      wm.transposeToRef(TmpVectors::MatrixArray[1]);

      wm = TmpVectors::MatrixArray[1];
    }

    result = Vector3::TransformNormal(result, wm);
  }

  result.normalize();

  return result;
}

std::optional<Vector2> PickingInfo::getTextureCoordinates()
{
  if (!pickedMesh || !pickedMesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
    return std::nullopt;
  }

  const auto indices = pickedMesh->getIndices();
  if (indices.empty()) {
    return std::nullopt;
  }

  const auto uvs = pickedMesh->getVerticesData(VertexBuffer::UVKind);
  if (uvs.empty()) {
    return std::nullopt;
  }

  auto uv0 = Vector2::FromArray(uvs, indices[faceId * 3] * 2);
  auto uv1 = Vector2::FromArray(uvs, indices[faceId * 3 + 1] * 2);
  auto uv2 = Vector2::FromArray(uvs, indices[faceId * 3 + 2] * 2);

  uv0 = uv0.scale(bu);
  uv1 = uv1.scale(bv);
  uv2 = uv2.scale(1.f - bu - bv);

  return Vector2(uv0.x + uv1.x + uv2.x, uv0.y + uv1.y + uv2.y);
}

} // end of namespace BABYLON
