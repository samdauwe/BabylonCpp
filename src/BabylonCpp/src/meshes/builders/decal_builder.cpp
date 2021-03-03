#include <babylon/meshes/builders/decal_builder.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/position_normal_texture_vertex.h>
#include <babylon/maths/position_normal_vertex.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr DecalBuilder::CreateDecal(const std::string& name, const AbstractMeshPtr& sourceMesh,
                                  DecalOptions& options)
{
  const auto indices   = sourceMesh->getIndices();
  const auto positions = sourceMesh->getVerticesData(VertexBuffer::PositionKind);
  const auto normals   = sourceMesh->getVerticesData(VertexBuffer::NormalKind);
  const auto uvs       = sourceMesh->getVerticesData(VertexBuffer::UVKind);
  const auto position  = options.position.value_or(Vector3::Zero());
  const auto normal    = options.normal.value_or(Vector3::Up());
  const auto size      = options.size.value_or(Vector3::One());
  const auto angle     = options.angle.value_or(0.f);

  auto yaw   = -std::atan2(normal.z, normal.x) - Math::PI_2;
  auto len   = std::sqrt(normal.x * normal.x + normal.z * normal.z);
  auto pitch = std::atan2(normal.y, len);

  // Matrix
  auto decalWorldMatrix = Matrix::RotationYawPitchRoll(yaw, pitch, angle)
                            .multiply(Matrix::Translation(position.x, position.y, position.z));
  auto inverseDecalWorldMatrix = Matrix::Invert(decalWorldMatrix);
  auto meshWorldMatrix         = sourceMesh->getWorldMatrix();
  auto transformMatrix         = meshWorldMatrix.multiply(inverseDecalWorldMatrix);

  auto vertexData       = std::make_unique<VertexData>();
  vertexData->indices   = {};
  vertexData->positions = {};
  vertexData->normals   = {};
  vertexData->uvs       = {};

  auto currentVertexDataIndex = 0u;

  const auto extractDecalVector3 = [&](std::size_t indexId) -> PositionNormalTextureVertex {
    PositionNormalTextureVertex result;
    if (indices.empty() || positions.empty() || normals.empty()) {
      return result;
    }

    const auto& vertexId = indices[indexId];
    result.position
      = Vector3(positions[vertexId * 3], positions[vertexId * 3 + 1], positions[vertexId * 3 + 2]);

    // Send vector to decal local world
    result.position = Vector3::TransformCoordinates(result.position, transformMatrix);

    // Get normal
    result.normal
      = Vector3(normals[vertexId * 3 + 0], normals[vertexId * 3 + 1], normals[vertexId * 3 + 2]);
    result.normal = Vector3::TransformNormal(result.normal, transformMatrix);

    if (options.captureUVS.value_or(false) && uvs.size() > (vertexId * 2 + 1)) {
      result.uv = Vector2(uvs[vertexId * 2], uvs[vertexId * 2 + 1]);
    }

    return result;
  };

  // Inspired by
  // https://github.com/mrdoob/three.js/blob/eee231960882f6f3b6113405f524956145148146/examples/js/geometries/DecalGeometry.js
  const auto clip
    = [&size](const std::vector<PositionNormalTextureVertex>& vertices, const Vector3& axis) {
        if (vertices.empty()) {
          return vertices;
        }

        auto clipSize = 0.5f * std::abs(Vector3::Dot(size, axis));

        const auto clipVertices
          = [&](const PositionNormalTextureVertex& v0,
                const PositionNormalTextureVertex& v1) -> PositionNormalTextureVertex {
          auto clipFactor = Vector3::GetClipFactor(v0.position, v1.position, axis, clipSize);

          return PositionNormalTextureVertex(Vector3::Lerp(v0.position, v1.position, clipFactor),
                                             Vector3::Lerp(v0.normal, v1.normal, clipFactor));
        };

        std::vector<PositionNormalTextureVertex> result;

        bool v1Out, v2Out, v3Out;
        unsigned int total;
        std::optional<PositionNormalTextureVertex> nV1, nV2, nV3, nV4;
        float d1, d2, d3;
        for (std::size_t index = 0; index < vertices.size(); index += 3) {
          total = 0;

          nV1 = std::nullopt;
          nV2 = std::nullopt;
          nV3 = std::nullopt;
          nV4 = std::nullopt;

          d1 = Vector3::Dot(vertices[index].position, axis) - clipSize;
          d2 = Vector3::Dot(vertices[index + 1].position, axis) - clipSize;
          d3 = Vector3::Dot(vertices[index + 2].position, axis) - clipSize;

          v1Out = d1 > 0;
          v2Out = d2 > 0;
          v3Out = d3 > 0;

          total = (v1Out ? 1 : 0) + (v2Out ? 1 : 0) + (v3Out ? 1 : 0);

          switch (total) {
            case 0:
              result.emplace_back(vertices[index + 0]);
              result.emplace_back(vertices[index + 1]);
              result.emplace_back(vertices[index + 2]);
              break;
            case 1:

              if (v1Out) {
                nV1 = vertices[index + 1];
                nV2 = vertices[index + 2];
                nV3 = clipVertices(vertices[index], *nV1);
                nV4 = clipVertices(vertices[index], *nV2);
              }

              if (v2Out) {
                nV1 = vertices[index + 0];
                nV2 = vertices[index + 2];
                nV3 = clipVertices(vertices[index + 1], *nV1);
                nV4 = clipVertices(vertices[index + 1], *nV2);

                result.emplace_back(*nV3);
                result.emplace_back(*nV2);
                result.emplace_back(*nV1);

                result.emplace_back(*nV2);
                result.emplace_back(*nV3);
                result.emplace_back(*nV4);
                break;
              }
              if (v3Out) {
                nV1 = vertices[index + 0];
                nV2 = vertices[index + 1];
                nV3 = clipVertices(vertices[index + 2], *nV1);
                nV4 = clipVertices(vertices[index + 2], *nV2);
              }

              if (nV1 && nV2 && nV3 && nV4) {
                result.emplace_back(*nV1);
                result.emplace_back(*nV2);
                result.emplace_back(*nV3);

                result.emplace_back(*nV4);
                result.emplace_back(*nV3);
                result.emplace_back(*nV2);
              }
              break;
            case 2:
              if (!v1Out) {
                nV1 = vertices[index + 0];
                nV2 = clipVertices(*nV1, vertices[index + 1]);
                nV3 = clipVertices(*nV1, vertices[index + 2]);
                result.emplace_back(*nV1);
                result.emplace_back(*nV2);
                result.emplace_back(*nV3);
              }
              if (!v2Out) {
                nV1 = vertices[index + 1];
                nV2 = clipVertices(*nV1, vertices[index + 2]);
                nV3 = clipVertices(*nV1, vertices[index + 0]);
                result.emplace_back(*nV1);
                result.emplace_back(*nV2);
                result.emplace_back(*nV3);
              }
              if (!v3Out) {
                nV1 = vertices[index + 2];
                nV2 = clipVertices(*nV1, vertices[index + 0]);
                nV3 = clipVertices(*nV1, vertices[index + 1]);
                result.emplace_back(*nV1);
                result.emplace_back(*nV2);
                result.emplace_back(*nV3);
              }
              break;
            case 3:
              break;
            default:
              break;
          }
        }

        return result;
      };

  for (std::size_t index = 0; index < indices.size(); index += 3) {
    std::vector<PositionNormalTextureVertex> faceVertices;

    faceVertices.emplace_back(extractDecalVector3(index));
    faceVertices.emplace_back(extractDecalVector3(index + 1));
    faceVertices.emplace_back(extractDecalVector3(index + 2));

    // Clip
    faceVertices = clip(faceVertices, Vector3(1.f, 0.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(-1.f, 0.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 1.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, -1.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 0.f, 1.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 0.f, -1.f));

    if (faceVertices.empty()) {
      continue;
    }

    // Add UVs and get back to world
    for (auto& vertex : faceVertices) {
      vertexData->indices.emplace_back(currentVertexDataIndex);
      vertex.position.toArray(vertexData->positions, currentVertexDataIndex * 3);
      vertex.normal.toArray(vertexData->normals, currentVertexDataIndex * 3);

      if (!options.captureUVS.value_or(false)) {
        vertexData->uvs.emplace_back(0.5f + vertex.position.x / size.x);
        vertexData->uvs.emplace_back(0.5f + vertex.position.y / size.y);
      }
      else {
        vertex.uv.toArray(vertexData->uvs, currentVertexDataIndex * 2);
      }

      ++currentVertexDataIndex;
    }
  }

  // Return mesh
  auto decal = Mesh::New(name, sourceMesh->getScene());
  vertexData->applyToMesh(*decal);

  decal->position = position;
  decal->rotation = Vector3(pitch, yaw, angle);

  return decal;
}

} // end of namespace BABYLON
