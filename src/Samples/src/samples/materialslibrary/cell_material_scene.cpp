#include <babylon/samples/materialslibrary/cell_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/cell/cell_material.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

CellMaterialScene::CellMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CellMaterialScene::~CellMaterialScene()
{
}

const char* CellMaterialScene::getName()
{
  return "Cell Material Scene";
}

void CellMaterialScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color3(0.5f, 0.5f, 0.5f);

  auto camera
    = ArcRotateCamera::New("Camera", 0, Math::PI_2, 12, Vector3::Zero(), scene);
  camera->attachControl(canvas, true, true);

  auto knot  = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);
  auto light = PointLight::New("light", Vector3(10.f, 10.f, 10.f), scene);

  auto cell            = MaterialsLibrary::CellMaterial::New("cell", scene);
  cell->diffuseTexture = Texture::New("textures/amiga.jpg", scene);
  cell->diffuseTexture()->uScale = cell->diffuseTexture()->vScale = 3;
  cell->computeHighLevel                                          = true;
  knot->material                                                  = cell;
}

} // end of namespace Samples
} // end of namespace BABYLON
