#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materialslibrary/normal/normal_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the normal material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#22VQKB
 */
struct NormalMaterialScene : public IRenderableScene {

  NormalMaterialScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~NormalMaterialScene()
  {
  }

  const char* getName() override
  {
    return "Normal Material Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("camera1", 0.f, 0.f, 30.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);
  
    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 1.f;
  
    auto sphere = Mesh::CreateSphere("sphere1", 16, 20, scene);
  
    auto normalMaterial
      = MaterialsLibrary::NormalMaterial::New("normalMat", scene);
    sphere->material = normalMaterial;
  }

}; // end of struct NormalMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", NormalMaterialScene)
} // end of namespace Samples
} // end of namespace BABYLON