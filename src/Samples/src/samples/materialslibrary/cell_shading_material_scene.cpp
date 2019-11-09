#include <babylon/cameras/free_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/cell/cell_material.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cell shading material scene. Scene demonstrating the use of the cell material from the
 * materials library.
 * @see https://www.babylonjs-playground.com/#0ZB1A3#1
 * @see https://doc.babylonjs.com/extensions/cell
 */
struct CellShadingMaterialScene : public IRenderableScene {

  CellShadingMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~CellShadingMaterialScene() override = default;

  const char* getName() override
  {
    return "Cell Shading Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto light  = PointLight::New("point01", Vector3(0.f, 8.f, 0.f), scene);
    auto camera = FreeCamera::New("camera", Vector3(0.f, 10.f, 7.f), scene);
    camera->setTarget(Vector3::Zero());
    camera->speed = 0.4f;
    camera->attachControl(canvas, true);

    scene->ambientColor = Color3(0.f, 0.f, 0.f);
    scene->clearColor   = Color3(0.f, 0.f, 0.f);

    // Ground
    auto groundCellMaterial              = MaterialsLibrary::CellMaterial::New("cell", scene);
    groundCellMaterial->backFaceCulling  = false;
    auto diffuseTexture1                 = Texture::New("/textures/albedo.png", scene);
    diffuseTexture1->uScale              = 16.f;
    diffuseTexture1->vScale              = 16.f;
    groundCellMaterial->computeHighLevel = true;
    groundCellMaterial->diffuseTexture   = diffuseTexture1;

    auto ground      = Mesh::CreateGround("ground", 100, 100, 100, scene);
    ground->material = groundCellMaterial;

    // Torus
    auto cellMaterial              = MaterialsLibrary::CellMaterial::New("cell", scene);
    cellMaterial->backFaceCulling  = false;
    cellMaterial->diffuseColor     = Color3(1.f, 1.f, 1.f);
    auto diffuseTexture2           = Texture::New("/textures/wood.jpg", scene);
    diffuseTexture2->uScale        = 3.f;
    diffuseTexture2->vScale        = 3.f;
    cellMaterial->computeHighLevel = true;
    cellMaterial->diffuseTexture   = diffuseTexture2;

    auto knot          = Mesh::CreateTorusKnot("knot", 1, 0.4f, 128, 64, 2, 3, scene);
    knot->position().y = 3.f;
    knot->material     = cellMaterial;

    float range  = 60.f;
    size_t count = 100;
    for (size_t index = 0; index < count; ++index) {
      auto indexStr    = std::to_string(index);
      auto newInstance = knot->createInstance("i" + indexStr);
      auto x           = range / 2.f - Math::random() * range;
      auto z           = range / 2.f - Math::random() * range;
      auto y           = 3.f;

      newInstance->position = Vector3(x, y, z);
      newInstance->rotate(Axis::Y(), Math::random() * Math::PI2, Space::WORLD);
      newInstance->scaling = Vector3(1, 1, 1);
    }
  }

}; // end of struct CellShadingMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", CellShadingMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
