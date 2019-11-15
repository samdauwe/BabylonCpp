#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materialslibrary/gradient/gradient_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the gradient material from the materials library.
 * @see https://doc.babylonjs.com/extensions/Gradient
 */
class GradientMaterialSphereScene : public IRenderableScene {

public:
  GradientMaterialSphereScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _sphere{nullptr}, _alpha{Math::PI}
  {
  }

  ~GradientMaterialSphereScene() override = default;

  const char* getName() override
  {
    return "Gradient Material Sphere Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -10.f), scene);
    camera->setTarget(Vector3::Zero());
    camera->attachControl(canvas, true);

    auto light         = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->groundColor = Color3(1.f, 1.f, 1.f);
    light->intensity   = 1.f;

    _sphere               = Mesh::CreateSphere("sphere1", 16, 5, scene);
    _sphere->position().y = 0;

    auto gradientMaterial         = MaterialsLibrary::GradientMaterial::New("grad", scene);
    gradientMaterial->topColor    = Color3::Green();
    gradientMaterial->bottomColor = Color3::Blue();
    gradientMaterial->offset      = 0.f;
    gradientMaterial->smoothness  = 1.3f;
    _sphere->material             = gradientMaterial;

    scene->registerBeforeRender([this](Scene*, EventState&) {
      _sphere->position().y += std::cos(_alpha) / 100.f;
      _alpha += 0.01f;
    });
  }

private:
  MeshPtr _sphere;
  float _alpha;

}; // end of struct GradientMaterialSphereScene

BABYLON_REGISTER_SAMPLE("Materials Library", GradientMaterialSphereScene)

} // end of namespace Samples
} // end of namespace BABYLON
