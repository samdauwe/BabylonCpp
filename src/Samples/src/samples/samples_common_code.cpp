#include <babylon/samples/samples_common_code.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/color3.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {
namespace Samples {

std::vector<AbstractMeshPtr> CrowdSimulationUtils::createAgentMeshes(Scene* scene,
                                                                     size_t totalNoAgents)
{
  const auto totalNoAgentsf = static_cast<float>(totalNoAgents);
  std::vector<AbstractMeshPtr> agentsMesh(totalNoAgents);

  for (size_t i = 0; i < totalNoAgents; ++i) {
    const auto agentNof = static_cast<float>(i);

    // Create the mesh material
    auto material           = StandardMaterial::New("m", scene);
    material->emissiveColor = Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f);
    material->diffuseColor  = Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f);

    // Create the mesh
    auto mesh          = Mesh::CreateCylinder("_", 15, 3.f, 3.f, 5, 1, scene, false);
    mesh->position().x = totalNoAgents * std::cos(agentNof * Math::PI2 / totalNoAgentsf);
    mesh->position().z = totalNoAgents * std::sin(agentNof * Math::PI2 / totalNoAgentsf);
    mesh->position().y = 0.f;
    mesh->material     = material;

    agentsMesh[i] = mesh;
  }

  return agentsMesh;
}

AbstractMeshPtr CrowdSimulationUtils::createWayPointMesh(Scene* scene, const Vector2& position)
{
  auto mesh      = Mesh::CreateSphere("", 8, 5.f, scene);
  mesh->position = Vector3(position.x, 2, position.y);

  return mesh;
}

MeshPtr MeshUtils::CreateTwelvePointedStar(size_t len, Scene* scene)
{
  static const Polyhedron dodecahedron{// name
                                       "Dodecahedron",
                                       // category
                                       "",
                                       // vertices
                                       {{0.f, 0.f, 1.070466f},
                                        {0.7136442f, 0.f, 0.7978784f},
                                        {-0.3568221f, 0.618034f, 0.7978784f},
                                        {-0.3568221f, -0.618034f, 0.7978784f},
                                        {0.7978784f, 0.618034f, 0.3568221f},
                                        {0.7978784f, -0.618034f, 0.3568221f},
                                        {-0.9341724f, 0.381966f, 0.3568221f},
                                        {0.1362939f, 1.f, 0.3568221f},
                                        {0.1362939f, -1.f, 0.3568221f},
                                        {-0.9341724f, -0.381966f, 0.3568221f},
                                        {0.9341724f, 0.381966f, -0.3568221f},
                                        {0.9341724f, -0.381966f, -0.3568221f},
                                        {-0.7978784f, 0.618034f, -0.3568221f},
                                        {-0.1362939f, 1.f, -0.3568221f},
                                        {-0.1362939f, -1, -0.3568221f},
                                        {-0.7978784f, -0.618034f, -0.3568221f},
                                        {0.3568221f, 0.618034f, -0.7978784f},
                                        {0.3568221f, -0.618034f, -0.7978784f},
                                        {-0.7136442f, 0.f, -0.7978784f},
                                        {0.f, 0.f, -1.070466f}},
                                       // faces
                                       {{0, 1, 4, 7, 2},
                                        {0, 2, 6, 9, 3},
                                        {0, 3, 8, 5, 1},
                                        {1, 5, 11, 10, 4},
                                        {2, 7, 13, 12, 6},
                                        {3, 9, 15, 14, 8},
                                        {4, 10, 16, 13, 7},
                                        {5, 8, 14, 17, 11},
                                        {6, 12, 18, 15, 9},
                                        {10, 11, 17, 19, 16},
                                        {12, 13, 16, 19, 18},
                                        {14, 15, 18, 19, 17}}};

  Uint32Array face;
  auto pointIndex = 0ull;
  auto centerX    = 0.f;
  auto centerY    = 0.f;
  auto centerZ    = 0.f;
  Float32Array positions;
  IndicesArray indices;
  auto vec1       = Vector3::Zero();
  auto vec2       = Vector3::Zero();
  auto nbVertices = dodecahedron.vertices.size();
  for (size_t v = 0; v < nbVertices; ++v) {
    stl_util::concat(positions, dodecahedron.vertices[v]);
  }
  for (size_t f = 0; f < 12; ++f) {
    face    = dodecahedron.faces[f];
    centerX = 0.f;
    centerY = 0.f;
    centerZ = 0.f;
    vec1.set(dodecahedron.vertices[face[1]][0] - dodecahedron.vertices[face[0]][0],
             dodecahedron.vertices[face[1]][1] - dodecahedron.vertices[face[0]][1],
             dodecahedron.vertices[face[1]][2] - dodecahedron.vertices[face[0]][2]);
    vec2.set(dodecahedron.vertices[face[2]][0] - dodecahedron.vertices[face[1]][0],
             dodecahedron.vertices[face[2]][1] - dodecahedron.vertices[face[1]][1],
             dodecahedron.vertices[face[2]][2] - dodecahedron.vertices[face[1]][2]);
    auto norm = Vector3::Cross(vec1, vec2).normalize();
    for (size_t v = 0; v < 5; ++v) {
      centerX += dodecahedron.vertices[face[v]][0];
      centerY += dodecahedron.vertices[face[v]][1];
      centerZ += dodecahedron.vertices[face[v]][2];
    }
    pointIndex = 1 * (nbVertices + f);
    stl_util::concat(positions, {centerX / 6.f + len * norm.x, centerY / 6.f + len * norm.y,
                                 centerZ / 6.f + len * norm.z});
    for (size_t v = 0; v < 5; ++v) {
      stl_util::concat(indices, {face[v], static_cast<unsigned>(pointIndex), face[(v + 1) % 5]});
    }
  }

  auto vertexData       = std::make_unique<VertexData>();
  vertexData->positions = std::move(positions);
  vertexData->indices   = std::move(indices);

  Float32Array normals;
  VertexData::ComputeNormals(positions, indices, normals);

  vertexData->normals = std::move(normals);

  auto mesh = Mesh::New("star", scene);
  vertexData->applyToMesh(*mesh);

  return mesh;
}

} // end of namespace Samples
} // end of namespace BABYLON
