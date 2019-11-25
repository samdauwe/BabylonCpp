#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Picking Scene. Example demonstrating how to use mouse or touch to pick meshes on the
 * screen.
 * @see https://www.babylonjs-playground.com/#NU4F6Y#0
 * @see https://doc.babylonjs.com/babylon101/picking_collisions
 */
class PickingScene : public IRenderableScene {

public:
  PickingScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _impact{nullptr}
  {
  }

  ~PickingScene() override = default;

  const char* getName() override
  {
    return "Picking Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    // Setup environment
    auto light0     = PointLight::New("Omni", Vector3(0.f, 10.f, 20.f), scene);
    auto freeCamera = FreeCamera::New("FreeCamera", Vector3(0.f, 0.f, -30.f), scene);

    // Impact impostor
    _impact                                    = Mesh::CreatePlane("impact", 1.f, scene);
    auto impactMaterial                        = StandardMaterial::New("impactMat", scene);
    impactMaterial->diffuseTexture             = Texture::New("textures/impact.png", scene);
    impactMaterial->diffuseTexture()->hasAlpha = true;
    _impact->material                          = impactMaterial;
    _impact->position                          = Vector3(0.f, 0.f, -0.1f);

    // Wall
    auto wall                   = Mesh::CreatePlane("wall", 20.0, scene);
    auto wallMaterial           = StandardMaterial::New("wallMat", scene);
    wallMaterial->emissiveColor = Color3(0.5f, 1.f, 0.5f);
    wall->material              = wallMaterial;

    // When pointer down event is raised
    scene->onPointerDown
      = [this](const PointerEvent& /*evt*/, const std::optional<PickingInfo>& pickResult,
               PointerEventTypes /*type*/) {
          // if the click hits the ground object, we change the impact position
          if (pickResult.has_value() && pickResult->hit) {
            _impact->position().x = pickResult->pickedPoint->x;
            _impact->position().y = pickResult->pickedPoint->y;
          }
        };
  }

private:
  MeshPtr _impact;

}; // end of class PickingScene

BABYLON_REGISTER_SAMPLE("Interactions & events", PickingScene)

} // end of namespace Samples
} // end of namespace BABYLON
