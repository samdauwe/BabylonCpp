#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/cell/cell_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the cell material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#36VUUE#0
 */
struct CellMaterialScene : public IRenderableScene {

  CellMaterialScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~CellMaterialScene()
  {
  }

  const char* getName() override
  {
    return "Cell Material Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor = Color3(0.5f, 0.5f, 0.5f);
  
    auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI_2, 12.f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true, true);
  
    auto knot  = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);
    auto light = PointLight::New("light", Vector3(10.f, 10.f, 10.f), scene);
  
    auto cell            = MaterialsLibrary::CellMaterial::New("cell", scene);
    cell->diffuseTexture = Texture::New("textures/amiga.jpg", scene);
    cell->diffuseTexture()->uScale = cell->diffuseTexture()->vScale = 3;
    cell->computeHighLevel                                          = true;
    knot->material                                                  = cell;
  }

}; // end of struct CellMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", CellMaterialScene)
} // end of namespace Samples
} // end of namespace BABYLON