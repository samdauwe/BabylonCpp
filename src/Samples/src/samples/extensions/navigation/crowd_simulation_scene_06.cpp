#include <vector>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/navigation/crowd_simulation.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_utils.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief The scene class simulates 196 agents trying to reach there opposite position, 4 obstacles.
 * This simulation uses Dijkstra algorithm (user define waypoints, here gray spheres) to help agents
 * to find their way. It's more fluid than the last demo, but it costs more computation time.
 */
class CrowdSimulationScene06 : public IRenderableScene {

public:
  CrowdSimulationScene06(ICanvas* iCanvas) : IRenderableScene(iCanvas), _crowdSimulation{nullptr}
  {
  }

  ~CrowdSimulationScene06() override = default;

  const char* getName() override
  {
    return "Crowd Simulation Scene 06";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Change the clear color
    scene->clearColor = Color4(0.36f, 0.36f, 0.36f);

    // Create a camera
    auto camera = ArcRotateCamera::New("camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);

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
    auto groundMaterial           = StandardMaterial::New("groundMaterial", scene);
    groundMaterial->emissiveColor = Color3::Black();
    groundMaterial->specularColor = Color3::Black();
    groundMaterial->diffuseColor  = Color3(0.36f, 0.36f, 0.36f);

    // Create the ground
    auto ground            = Mesh::CreateBox("ground", 600.f, scene);
    ground->position().y   = -6.f;
    ground->scaling().y    = 0.001f;
    ground->material       = groundMaterial;
    ground->receiveShadows = true;

    // Setup Scenario
    setupScenario(scene);

    // Update scene model
    scene->onBeforeCameraRenderObservable.add(
      [this](Camera*, EventState&) { _crowdSimulation->update(); });
  }

private:
  /**
   * @brief 196 agents try to reach there opposite position, 4 obstacles. This
   * simulation uses Dijkstra algorithm (user define waypoints, here gray
   * spheres) to help agents to find their way. It's more fluid than the last
   * demo, but it costs more computation time.
   */
  void setupScenario(Scene* scene)
  {
    // Create agent meshes
    _agentMeshes = CrowdSimulationUtils::createAgentMeshes(scene, 196);

    // Create new crowd simulation
    _crowdSimulation.reset(new Extensions::CrowdSimulation());
    _crowdSimulation->setTimeStep(0.3f);

    // Create agents
    unsigned int index = 0;
    size_t agentId     = 0;
    for (unsigned int i = 0; i < 7; ++i) {
      for (unsigned int j = 0; j < 7; ++j) {
        agentId = _crowdSimulation->addAgent(_agentMeshes[index++],
                                             Vector3(55.f + i * 10.f, 0, 55.f + j * 10.f));
        _crowdSimulation->setAgentGoal(agentId, Vector2(-55.f + i * -10.f, -55.f + j * -10.f));

        agentId = _crowdSimulation->addAgent(_agentMeshes[index++],
                                             Vector3(-55.f - i * 10.f, 0, 55.f + j * 10.f));
        _crowdSimulation->setAgentGoal(agentId, Vector2(55.f + i * 10.f, -55.f - j * 10.f));

        agentId = _crowdSimulation->addAgent(_agentMeshes[index++],
                                             Vector3(55.f + i * 10.f, 0, -55.f - j * 10.f));
        _crowdSimulation->setAgentGoal(agentId, Vector2(-55.f + i * -10.f, 55.f - j * -10.f));

        agentId = _crowdSimulation->addAgent(_agentMeshes[index++],
                                             Vector3(-55.f - i * 10.f, 0, -55.f - j * 10.f));
        _crowdSimulation->setAgentGoal(agentId, Vector2(55.f + i * 10.f, 55.f + j * 10.f));
      }
    }

    // Create obstacles
    const std::vector<Vector3> boxPositions{
      Vector3(25.f, -3.f, 25.f),   //
      Vector3(-25.f, -3.f, 25.f),  //
      Vector3(-25.f, -3.f, -25.f), //
      Vector3(25.f, -3.f, -25.f),  //
    };

    for (const auto& boxPosition : boxPositions) {
      auto box = Mesh::CreateBox("", 30.f, scene);
      _crowdSimulation->addObstacleByBoundingBox(box, boxPosition);
    }

    // Add waypoints
    const std::vector<Vector2> wayPoints{
      Vector2(-42.1f, -42.2f),  //
      Vector2(-42.3f, -8.4f),   //
      Vector2(-42.1f, 8.21f),   //
      Vector2(-42.21f, 42.f),   //
      Vector2(-8.1f, -42.f),    //
      Vector2(-8.12f, -8.f),    //
      Vector2(-8.05f, 8.f),     //
      Vector2(-8.05f, 42.f),    //
      Vector2(8.08f, -42.f),    //
      Vector2(8.07f, -8.09f),   //
      Vector2(8.09f, 8.f),      //
      Vector2(8.08f, 42.f),     //
      Vector2(42.02f, -42.09f), //
      Vector2(42.01f, -8.09f),  //
      Vector2(42.04f, 8.09f),   //
      Vector2(42.08f, 42.09f)   //
    };

    for (const auto& wayPoint : wayPoints) {
      CrowdSimulationUtils::createWayPointMesh(scene, wayPoint);
      _crowdSimulation->addWayPoint(wayPoint);
    }

    // Intialize the simulation
    _crowdSimulation->processObstacles();
    _crowdSimulation->computeRoadMap();
  }

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene06

BABYLON_REGISTER_SAMPLE("Extensions", CrowdSimulationScene06)

} // end of namespace Samples
} // end of namespace BABYLON
