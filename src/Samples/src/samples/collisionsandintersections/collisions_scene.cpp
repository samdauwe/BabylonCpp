#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Collisions Scene. Scene demonstrating how to handle basic collisions to avoid a camera
 * going through a box.
 * @see https://www.babylonjs-playground.com/#U8MEB0#0
 * @see https://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
 */
struct CollisionsScene : public IRenderableScene {

  CollisionsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~CollisionsScene() override = default;

  const char* getName() override
  {
    return "Collisions Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Lights
    auto light0       = DirectionalLight::New("Omni", Vector3(-2.f, -5.f, 2.f), scene);
    light0->intensity = 0.97f;
    auto light1       = PointLight::New("Omni", Vector3(2.f, -5.f, -2.f), scene);
    light1->intensity = 0.97f;

    // Need a free camera for collisions
    auto camera = FreeCamera::New("FreeCamera", Vector3(0.f, -8.f, -20.f), scene);
    camera->attachControl(canvas, true);

    // Ground
    auto ground                     = Mesh::CreatePlane("ground", 20.f, scene);
    auto groundMaterial             = StandardMaterial::New("groundMat", scene);
    groundMaterial->diffuseColor    = Color3(1.f, 1.f, 1.f);
    groundMaterial->backFaceCulling = false;
    ground->position                = Vector3(5.f, -10.f, -15.f);
    ground->rotation                = Vector3(Math::PI_2, 0.f, 0.f);
    ground->material                = groundMaterial;

    // Simple crate
    auto box                    = Mesh::CreateBox("crate", 2.f, scene);
    auto boxMaterial            = StandardMaterial::New("Mat", scene);
    auto diffuseTexture         = Texture::New("textures/crate.png", scene);
    diffuseTexture->hasAlpha    = true;
    boxMaterial->diffuseTexture = diffuseTexture;
    box->position               = Vector3(5.f, -9.f, -10.f);
    box->material               = boxMaterial;

    // Set gravity for the scene (G force like, on Y-axis)
    scene->gravity = Vector3(0.f, -0.9f, 0.f);

    // Enable Collisions
    scene->collisionsEnabled = true;

    // Then apply collisions and gravity to the active camera
    camera->checkCollisions = true;
    camera->applyGravity    = true;

    // Set the ellipsoid around the camera (e.g. your player's size)
    camera->ellipsoid = Vector3(1.f, 1.f, 1.f);

    // Finally, say which mesh will be collisionable
    ground->checkCollisions = true;
    box->checkCollisions    = true;
  }

}; // end of struct CollisionsScene

} // end of namespace Samples
} // end of namespace BABYLON

namespace BABYLON {
namespace Samples {

BABYLON_REGISTER_SAMPLE("Collisions & intersections", CollisionsScene)

} // end of namespace Samples
} // end of namespace BABYLON
