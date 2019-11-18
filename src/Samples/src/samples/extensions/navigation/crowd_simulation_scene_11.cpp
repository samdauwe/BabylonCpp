#include <memory>

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
#include <babylon/samples/samples_index.h>
#include <babylon/samples/samples_utils.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief The scene class simulates 256 agents. More complex scene with Dijkstra activated and
 * containing obstacles.
 */
class CrowdSimulationScene11 : public IRenderableScene {

public:
  CrowdSimulationScene11(ICanvas* iCanvas) : IRenderableScene(iCanvas), _crowdSimulation{nullptr}
  {
  }

  ~CrowdSimulationScene11() override = default;

  const char* getName() override
  {
    return "Crowd Simulation Scene 11";
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
   * @brief 256 agents. More complex scene with Dijkstra activated and
   * containing obstacles.
   */
  void setupScenario(Scene* scene)
  {
    // Create agent meshes
    _agentMeshes = CrowdSimulationUtils::createAgentMeshes(scene, 256);

    // Create new crowd simulation
    _crowdSimulation = std::make_unique<Extensions::CrowdSimulation>();
    _crowdSimulation->setTimeStep(0.3f);

    // Create agents
    unsigned int index = 0;
    size_t agentId     = 0;
    for (unsigned int i = 0; i < 16; ++i) {
      for (unsigned int j = 0; j < 16; ++j) {
        agentId = _crowdSimulation->addAgent(_agentMeshes[index++],
                                             Vector3(205.f + i * 1.f, 0.f, 0.f + j * 1.f));
        _crowdSimulation->setAgentGoal(agentId, Vector2(-150.f, 0.f));
      }
    }

    // Create obstacles
    const std::vector<Vector3> boxPositions{
      // Left boxes
      Vector3(150.f, 0.f, 29.f),   //
      Vector3(150.f, 0.f, -29.f),  //
      Vector3(260.f, 0.f, 0.f),    //
      Vector3(173.5f, 0.f, 55.f),  //
      Vector3(236.5f, 0.f, 55.f),  //
      Vector3(173.5f, 0.f, -55.f), //
      Vector3(236.5f, 0.f, -55.f), //

      // Right boxes
      Vector3(-300.f + 50.f, 0.f, 29.f),   //
      Vector3(-300.f + 50.f, 0.f, -29.f),  //
      Vector3(-190.f + 50.f, 0.f, 0.f),    //
      Vector3(-276.5f + 50.f, 0.f, 55.f),  //
      Vector3(-213.5f + 50.f, 0.f, 55.f),  //
      Vector3(-276.5f + 50.f, 0.f, -55.f), //
      Vector3(-213.5f + 50.f, 0.f, -55.f)  //
    };

    const std::vector<Vector3> scalings{
      // Left boxes
      Vector3(5.f, 10.f, 52.f),  //
      Vector3(5.f, 10.f, 52.f),  //
      Vector3(5.f, 10.f, 110.f), //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f),  //

      // Right boxes
      Vector3(5.f, 10.f, 52.f),  //
      Vector3(5.f, 10.f, 52.f),  //
      Vector3(5.f, 10.f, 110.f), //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f),  //
      Vector3(52.f, 10.f, 5.f)   //
    };

    for (unsigned int i = 0; i < boxPositions.size(); ++i) {
      auto box     = Mesh::CreateBox("", 1.f, scene);
      box->scaling = scalings[i];
      _crowdSimulation->addObstacleByBoundingBox(box, boxPositions[i]);
    }

    // Add waypoints
    const std::vector<Vector2> wayPoints{
      Vector2(140.f, 0.f),           //
      Vector2(160.f, 0.f),           //
      Vector2(205.f, -65.f),         //
      Vector2(205.f, -45.f),         //
      Vector2(205.f, 65.f),          //
      Vector2(205.f, 45.f),          //
      Vector2(145.f, 65.f),          //
      Vector2(145.f, -65.f),         //
      Vector2(265.f, 65.f),          //
      Vector2(265.f, -65.f),         //
      Vector2(140.f - 400.f, 0.f),   //
      Vector2(160.f - 400.f, 0.f),   //
      Vector2(205.f - 400.f, -65.f), //
      Vector2(205.f - 400.f, -45.f), //
      Vector2(205.f - 400.f, 65.f),  //
      Vector2(205.f - 400.f, 45.f),  //
      Vector2(145.f - 400.f, 65.f),  //
      Vector2(145.f - 400.f, -65.f), //
      Vector2(265.f - 400.f, 65.f),  //
      Vector2(265.f - 400.f, -65.f), //
    };

    for (const auto& wayPoint : wayPoints) {
      CrowdSimulationUtils::createWayPointMesh(scene, wayPoint);
      _crowdSimulation->addWayPoint(wayPoint);
    }

    // Set low precision
    _crowdSimulation->setPrecision(1);

    // Intialize the simulation
    _crowdSimulation->processObstacles();
    _crowdSimulation->computeRoadMap();
  }

private:
  std::unique_ptr<Extensions::CrowdSimulation> _crowdSimulation;
  std::vector<AbstractMeshPtr> _agentMeshes;

}; // end of class CrowdSimulationScene11

BABYLON_REGISTER_SAMPLE("Extensions", CrowdSimulationScene11)

} // end of namespace Samples
} // end of namespace BABYLON
