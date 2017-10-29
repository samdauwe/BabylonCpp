#include <babylon/samples/extensions/navigation/crowd_simulation_utils.h>

#include <babylon/materials/standard_material.h>
#include <babylon/math/color3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

vector_t<AbstractMesh*>
CrowdSimulationUtils::createAgentMeshes(Scene* scene, size_t totalNoAgents)
{
  const float totalNoAgentsf = static_cast<float>(totalNoAgents);
  vector_t<AbstractMesh*> agentsMesh(totalNoAgents);

  for (size_t i = 0; i < totalNoAgents; ++i) {
    const auto agentNof = static_cast<float>(i);

    // Create the mesh material
    auto material = StandardMaterial::New("m", scene);
    material->setEmissiveColor(
      Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f));
    material->setDiffuseColor(
      Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f));

    // Create the mesh
    auto mesh = Mesh::CreateCylinder("_", 15, 3.f, 3.f, 5, 1, scene, false);
    mesh->position().x
      = totalNoAgents * std::cos(agentNof * Math::PI2 / totalNoAgentsf);
    mesh->position().z
      = totalNoAgents * std::sin(agentNof * Math::PI2 / totalNoAgentsf);
    mesh->position().y = 0.f;
    mesh->setMaterial(material);

    agentsMesh[i] = mesh;
  }

  return agentsMesh;
}

AbstractMesh* CrowdSimulationUtils::createWayPointMesh(Scene* scene,
                                                       const Vector2& position)
{
  auto mesh = Mesh::CreateSphere("", 8, 5.f, scene);
  mesh->setPosition(Vector3(position.x, 2, position.y));

  return mesh;
}

} // end of namespace Samples
} // end of namespace BABYLON
