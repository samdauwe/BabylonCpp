#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materialslibrary/fur/fur_material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the fur material from the materials library.
 * @see https://www.babylonjs-playground.com/#VABI8A#0
 * @see https://doc.babylonjs.com/extensions/fur
 */
class FurMaterialScene : public IRenderableScene {

public:
  FurMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~FurMaterialScene() override = default;

  const char* getName() override
  {
    return "Fur Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    _camera = ArcRotateCamera::New("Camera", -2.5f, 1.f, 200.f, Vector3(0.f, 5.f, 0.f), scene);
    _camera->attachControl(canvas, true);

    scene->createDefaultLight();

    SceneLoader::Append("scenes/Rabbit/", "Rabbit.babylon", scene,
                        [this, scene](Scene* /*iScene*/) {
                          scene->activeCamera = _camera;

                          _configureFur(scene->meshes[1], scene);
                        });
  }

private:
  void _configureFur(const AbstractMeshPtr& mesh, Scene* scene) const
  {
    auto fur       = MaterialsLibrary::FurMaterial::New("fur", scene);
    fur->furLength = 0.f;
    fur->furAngle  = 0.f;
    fur->furColor  = Color3(2.f, 2.f, 2.f);
    fur->diffuseTexture
      = std::static_pointer_cast<StandardMaterial>(mesh->material())->diffuseTexture;
    fur->furTexture = MaterialsLibrary::FurMaterial::GenerateTexture("furTexture", scene);
    fur->furSpacing = 6.f;
    fur->furDensity = 20.f;
    fur->furSpeed   = 300.f;
    fur->furGravity = Vector3(0.f, -1.f, 0.f);

    mesh->material = fur;

    float quality = 30.f; // It is enougth
    auto shells
      = MaterialsLibrary::FurMaterial::FurifyMesh(std::static_pointer_cast<Mesh>(mesh), quality);

    // Special for bunny (ears)
    for (const auto& shell : shells) {
      shell->material()->backFaceCulling = false;
    }
  }

private:
  ArcRotateCameraPtr _camera;

}; // end of class FurMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", FurMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
