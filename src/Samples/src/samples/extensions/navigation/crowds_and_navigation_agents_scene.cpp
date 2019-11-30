#include <babylon/cameras/free_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/recastjs/recastjs_plugin.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/navigation/iagent_parameters.h>
#include <babylon/navigation/icrowd.h>
#include <babylon/navigation/inav_mesh_parameters.h>
#include <babylon/samples/samples_index.h>
#include <babylon/samples/samples_utils.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief The scene class simulates autonomous agents, making them navigate within the navmesh
 * constraint. The agents will find the best path to that destination while avoinding other crowd
 * agents. An agent is attach to a Transform. That means that you have to attach a mesh to see them
 * but also that you can attach pretty much anything.
 * @see https://www.babylonjs-playground.com/#X5XCVT
 * @see https://doc.babylonjs.com/extensions/crowd
 */
class CrowdsAndNavigationAgents : public IRenderableScene {

public:
  CrowdsAndNavigationAgents(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~CrowdsAndNavigationAgents() override = default;

  const char* getName() override
  {
    return "Crowds and navigation agents scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    camera = FreeCamera::New("camera1", Vector3(-6.f, 4.f, -8.f), scene);
    // This targets the camera to scene origin
    camera->setTarget(Vector3::Zero());
    // This attaches the camera to the canvas
    camera->attachControl(canvas, true);

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    auto staticMesh = createStaticMesh(scene);
    INavMeshParameters navmeshParameters;
    navmeshParameters.cs                     = 0.2f;
    navmeshParameters.ch                     = 0.2f;
    navmeshParameters.walkableSlopeAngle     = 90.f;
    navmeshParameters.walkableHeight         = 1;
    navmeshParameters.walkableClimb          = 1;
    navmeshParameters.walkableRadius         = 1;
    navmeshParameters.maxEdgeLen             = 12;
    navmeshParameters.maxSimplificationError = 1.3f;
    navmeshParameters.minRegionArea          = 8;
    navmeshParameters.mergeRegionArea        = 20;
    navmeshParameters.maxVertsPerPoly        = 6;
    navmeshParameters.detailSampleDist       = 6.f;
    navmeshParameters.detailSampleMaxError   = 1.f;

    navigationPlugin.createMavMesh({staticMesh}, navmeshParameters);
    auto navmeshdebug      = navigationPlugin.createDebugNavMesh(scene);
    navmeshdebug->position = Vector3(0.f, 0.01f, 0.f);

    auto matdebug          = StandardMaterial::New("matdebug", scene);
    matdebug->diffuseColor = Color3(0.1f, 0.2f, 1.f);
    matdebug->alpha        = 0.2f;
    navmeshdebug->material = matdebug;

    // crowd
    crowd = navigationPlugin.createCrowd(10, 0.1f, scene);
    IAgentParameters agentParams;
    agentParams.radius                = 0.1f;
    agentParams.height                = 0.2f;
    agentParams.maxAcceleration       = 4.f;
    agentParams.maxSpeed              = 1.f;
    agentParams.collisionQueryRange   = 0.5f;
    agentParams.pathOptimizationRange = 0.f;
    agentParams.separationWeight      = 1.f;

    for (unsigned int i = 0; i < 4; ++i) {
      auto width = 0.20f;
      BoxOptions boxOptions;
      boxOptions.size        = width;
      boxOptions.height      = width;
      auto agentCube         = MeshBuilder::CreateBox("cube", boxOptions, scene);
      auto matAgent          = StandardMaterial::New("mat2", scene);
      auto variation         = Math::random();
      matAgent->diffuseColor = Color3(0.4f + variation * 0.6f, 0.3f, 1.f - variation * 0.3f);
      agentCube->material    = matAgent;
      auto randomPos    = navigationPlugin.getRandomPointAround(Vector3(-2.f, 0.1f, -1.8f), 0.5f);
      auto transform    = TransformNode::New("transform");
      agentCube->parent = transform.get();
      crowd->addAgent(randomPos, agentParams, transform);
    }

    // When pointer down event is raised
    scene->onPointerDown
      = [this](const PointerEvent& /*evt*/, const std::optional<PickingInfo>& pickResult,
               PointerEventTypes /*type*/) {
          if (pickResult.has_value() && pickResult->hit) {
            pointerDown(pickResult->pickedMesh);
          }
        };
  }

  std::optional<Vector3> getGroundPosition(Scene* scene)
  {
    auto pickinfo = scene->pick(scene->pointerX, scene->pointerY);
    if (pickinfo->hit) {
      return pickinfo->pickedPoint;
    }

    return std::nullopt;
  }

  void pointerDown(const AbstractMeshPtr& mesh)
  {
    auto scene    = _scene.get();
    currentMesh   = mesh;
    startingPoint = getGroundPosition(scene);
    if (startingPoint) { // we need to disconnect camera from canvas
      camera->detachControl(_canvas);

      auto agents = crowd->getAgents();
      for (const auto& agent : agents) {
        const auto randomPos = navigationPlugin.getRandomPointAround(*startingPoint, 1.f);
        crowd->agentGoto(agent, navigationPlugin.getClosestPoint(*startingPoint));
      }
      const auto pathPoints = navigationPlugin.computePath(
        crowd->getAgentPosition(agents[0]), navigationPlugin.getClosestPoint(*startingPoint));
      DashedLinesOptions dashedLinesOptions;
      dashedLinesOptions.points    = pathPoints;
      dashedLinesOptions.updatable = true;
      dashedLinesOptions.instance  = pathLine;
      pathLine = MeshBuilder::CreateDashedLines("ribbon", dashedLinesOptions, scene);
    }
  }

  MeshPtr createStaticMesh(Scene* scene)
  {
    auto ground = Mesh::CreateGround("ground1", 6, 6, 2, scene);

    // Materials
    auto mat1          = StandardMaterial::New("mat1", scene);
    mat1->diffuseColor = Color3(1.f, 1.f, 1.f);

    SphereOptions sphereOptions;
    sphereOptions.diameter = 2.f;
    sphereOptions.segments = 16;
    auto sphere            = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
    sphere->material       = mat1;
    sphere->position().y   = 1.f;

    BoxOptions boxOptions;
    boxOptions.size   = 1.f;
    boxOptions.height = 3.f;
    auto cube         = MeshBuilder::CreateBox("cube", boxOptions, scene);
    cube->position    = Vector3(1.f, 1.5f, 0.f);
    // cube.material = mat2;

    auto mesh = Mesh::MergeMeshes({sphere, cube, ground});
    return mesh;
  }

private:
  Extensions::RecastJSPlugin navigationPlugin;
  ICrowdPtr crowd;
  FreeCameraPtr camera;
  AbstractMeshPtr currentMesh;
  LinesMeshPtr pathLine;
  std::optional<Vector3> startingPoint;

}; // end of class CrowdsAndNavigationAgents

BABYLON_REGISTER_SAMPLE("Extensions", CrowdsAndNavigationAgents)

} // end of namespace Samples
} // end of namespace BABYLON
