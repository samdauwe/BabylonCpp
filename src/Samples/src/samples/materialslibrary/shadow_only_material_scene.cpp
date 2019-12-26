#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materialslibrary/shadowonly/shadow_only_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the fire material from the materials library.
 * @see https://www.babylonjs-playground.com/#1KF7V1
 * @see https://doc.babylonjs.com/extensions/shadowonly
 */
class ShadowOnlyMaterialScene : public IRenderableScene {

public:
  ShadowOnlyMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }
  ~ShadowOnlyMaterialScene() override = default;

  const char* getName() override
  {
    return "Shadow Only Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor   = Color4(0.f, 0.f, 0.f, 0.f);
    scene->ambientColor = Color3(0.4f, 0.4f, 0.4f);

    auto camera = ArcRotateCamera::New("Camera", -0.5f, 0.8f, 200.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light       = DirectionalLight::New("light", Vector3(0.f, -1.f, 1.f), scene);
    light->intensity = 0.5f;

    auto ground            = Mesh::CreatePlane("ground", 1000.f, scene);
    ground->rotation().x   = Math::PI_2;
    ground->material       = MaterialsLibrary::ShadowOnlyMaterial::New("mat", scene);
    ground->receiveShadows = true;

    ground->position().y = -50.f;

    auto shadowGenerator                         = ShadowGenerator::New(512, light);
    shadowGenerator->useBlurExponentialShadowMap = true;
    shadowGenerator->blurScale                   = 2.f;
    shadowGenerator->setDarkness(0.2f);

    SceneLoader::ImportMesh(
      {}, "scenes/", "skull.babylon", scene,
      [camera, shadowGenerator](const std::vector<AbstractMeshPtr>& newMeshes,
                                const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
        if (!newMeshes.empty()) {
          shadowGenerator->getShadowMap()->renderList().emplace_back(newMeshes[0].get());
          // Set the target of the camera to the first imported mesh
          camera->setTarget(newMeshes[0]);
        }
      });
  }

}; // end of class ShadowOnlyMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", ShadowOnlyMaterialScene)
} // end of namespace Samples
} // end of namespace BABYLON
