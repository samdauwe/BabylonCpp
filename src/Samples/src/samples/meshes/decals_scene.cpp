#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Decals Scene. Example demonstrating how to create decals to apply additional textures to a
 * portion of a mesh.
 * @see https://www.babylonjs-playground.com/#EEUVTY#0
 * @see https://doc.babylonjs.com/how_to/decals
 */
class DecalsScene : public IRenderableScene {

public:
  DecalsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _camera{nullptr}
      , _decalMaterial{nullptr}
      , _onPointerDown{nullptr}
  {
  }

  ~DecalsScene() override = default;

  const char* getName() override
  {
    return "Decals Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Adding a light
    auto light = HemisphericLight::New("Hemi", Vector3(0.f, 1.f, 0.f), scene);

    // Adding an Arc Rotate Camera
    _camera = ArcRotateCamera::New("Camera", -1.85f, 1.2f, 200.f, Vector3::Zero(), scene);
    _camera->attachControl(canvas, true);

    // The first parameter can be used to specify which mesh to import. Here we
    // import all meshes
    SceneLoader::ImportMesh(
      {"Shcroendiger'scat"}, "/scenes/", "SSAOcat.babylon", scene,
      [this, scene, canvas](const std::vector<AbstractMeshPtr>& newMeshes,
                            const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                            const std::vector<SkeletonPtr>& /*newSkeletons*/,
                            const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/,
                            const std::vector<TransformNodePtr>& /*transformNodes*/,
                            const std::vector<GeometryPtr>& /*geometries*/,
                            const std::vector<LightPtr>& /*lights*/) {
        // Set the target of the camera to the first imported mesh
        if (!newMeshes.empty()) {
          auto cat = newMeshes[0];

          // Set the target of the camera to the first imported mesh
          _camera->setTarget(cat);

          _decalMaterial                 = StandardMaterial::New("decalMat", scene);
          auto diffuseTexture            = Texture::New("/textures/impact.png", scene);
          diffuseTexture->hasAlpha       = true;
          _decalMaterial->diffuseTexture = diffuseTexture;
          _decalMaterial->zOffset        = -2.f;

          _onPointerDown = [this, scene, cat](PointerEvent&& evt) -> void {
            if (evt.button != MouseButtonType::LEFT) {
              return;
            }

            // Check if we are under a mesh
            auto pickInfo
              = scene->pick(scene->pointerX, scene->pointerY,
                            [cat](const AbstractMeshPtr& mesh) -> bool { return mesh == cat; });
            if (pickInfo.has_value() && pickInfo->hit) {
              Vector3 decalSize(10.f, 10.f, 10.f);

              auto newDecal      = Mesh::CreateDecal("decal", cat, *pickInfo->pickedPoint,
                                                *pickInfo->getNormal(true), decalSize);
              newDecal->material = _decalMaterial;
            }
          };
        }

        canvas->addMouseEventListener(EventType::MOUSE_BUTTON_DOWN, _onPointerDown, false);
      });
  }

private:
  ArcRotateCameraPtr _camera;
  StandardMaterialPtr _decalMaterial;
  std::function<void(PointerEvent&& evt)> _onPointerDown;

}; // end of class DecalsScene

BABYLON_REGISTER_SAMPLE("Meshes", DecalsScene)

} // end of namespace Samples
} // end of namespace BABYLON
