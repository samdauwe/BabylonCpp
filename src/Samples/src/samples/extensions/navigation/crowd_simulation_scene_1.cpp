#include <babylon/samples/extensions/navigation/crowd_simulation_scene_1.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/extensions/navigation/crowd_simulation.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

#include <unistd.h>

namespace BABYLON {
namespace Samples {

CrowdSimulationScene1::CrowdSimulationScene1(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _crowdSimulation{nullptr}
{
}

CrowdSimulationScene1::~CrowdSimulationScene1()
{
}

const char* CrowdSimulationScene1::getName()
{
  return "Crowd Simulation Scene 1";
}

void CrowdSimulationScene1::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change the clear color
  scene->clearColor = Color4(0.36f, 0.36f, 0.36f);

  // Create a camera
  auto camera
    = ArcRotateCamera::New("camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);

  // Set the camera position and limits
  camera->setPosition(Vector3(0.f, 500.f, 0.1f));
  camera->lowerBetaLimit = 0.1f;
  camera->upperBetaLimit = Math::PI_2 * 0.95f;

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light       = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  light->intensity = 1.f;

  // Create the ground material
  auto groundMaterial = StandardMaterial::New("groundMaterial", scene);
  groundMaterial->setEmissiveColor(Color3::Black());
  groundMaterial->setSpecularColor(Color3::Black());
  groundMaterial->setDiffuseColor(Color3(0.36f, 0.36f, 0.36f));

  // Create the ground
  auto ground          = Mesh::CreateBox("ground", 600.f, scene);
  ground->position().y = -6.f;
  ground->scaling().y  = 0.001f;
  ground->setMaterial(groundMaterial);
  ground->setReceiveShadows(true);

  // Setup Scenario
  setupScenario();

  // Update scene model
  scene->onBeforeCameraRenderObservable.add(
    [this](Camera*, EventState&) { _crowdSimulation->update(); });
}

Mesh* CrowdSimulationScene1::createAgentsMesh(size_t agentNo,
                                              size_t totalNoAgents)
{
  const float agentNof       = static_cast<float>(agentNo);
  const float totalNoAgentsf = static_cast<float>(totalNoAgents);

  // Create the mesh material
  auto material = StandardMaterial::New("m", _scene.get());
  material->setEmissiveColor(
    Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f));
  material->setDiffuseColor(
    Color3(1.f, 0.7f - (agentNof / totalNoAgentsf), 0.f));

  // Create the mesh
  auto mesh
    = Mesh::CreateCylinder("_", 15, 3.f, 3.f, 5, 1, _scene.get(), false);
  mesh->position().x
    = totalNoAgents * std::cos(agentNof * Math::PI2 / totalNoAgentsf);
  mesh->position().y = 0.f;
  mesh->position().z
    = totalNoAgents * std::sin(agentNof * Math::PI2 / totalNoAgentsf);
  mesh->setMaterial(material);

  return mesh;
}

void CrowdSimulationScene1::setupScenario()
{
  // Create agent meshes
  _agentMeshes.resize(128);
  for (size_t agentNo = 0; agentNo < _agentMeshes.size(); ++agentNo) {
    _agentMeshes[agentNo] = createAgentsMesh(agentNo, _agentMeshes.size());
  }
  // Create new crowd simulation
  _crowdSimulation.reset(new Extensions::CrowdSimulation());
  _crowdSimulation->setTimeStep(0.3f);

  for (size_t agentNo = 0; agentNo < _agentMeshes.size(); ++agentNo) {
    auto agentMesh = _agentMeshes[agentNo];
    auto agentId   = _crowdSimulation->addAgent(agentMesh);
    _crowdSimulation->setAgentGoal(
      agentId, Vector2(-agentMesh->position().x, -agentMesh->position().z));
    _crowdSimulation->setAgentMaxSpeed(agentId, 1.f);
  }

  _crowdSimulation->processObstacles();
  _crowdSimulation->computeRoadMap();
}

} // end of namespace Samples
} // end of namespace BABYLON
