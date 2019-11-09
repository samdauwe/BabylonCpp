#include <babylon/samples/extensions/navigation/crowd_simulation_utils.h>

#include <babylon/materials/standard_material.h>
#include <babylon/math/color3.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

std::vector<AbstractMeshPtr> CrowdSimulationUtils::createAgentMeshes(Scene* scene,
                                                                     size_t totalNoAgents)
{
  const float totalNoAgentsf = static_cast<float>(totalNoAgents);
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

} // end of namespace Samples
} // end of namespace BABYLON
