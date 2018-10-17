#include <babylon/samples/meshes/constructive_solid_geometries_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/csg/csg.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ConstructiveSolidGeometriesScene::ConstructiveSolidGeometriesScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ConstructiveSolidGeometriesScene::~ConstructiveSolidGeometriesScene()
{
}

const char* ConstructiveSolidGeometriesScene::getName()
{
  return "Constructive Solid Geometries Scene";
}

void ConstructiveSolidGeometriesScene::initializeScene(ICanvas* canvas,
                                                       Scene* scene)
{
  auto light = DirectionalLight::New("dir01", Vector3(0.f, -0.5f, -1.f), scene);
  auto camera
    = ArcRotateCamera::New("Camera", 0, 0, 10, Vector3::Zero(), scene);
  camera->setPosition(Vector3(10, 10, 10));
  light->position = Vector3(20, 150, 70);
  camera->minZ    = 10.f;
  camera->attachControl(canvas, true);

  scene->ambientColor = Color3(0.3f, 0.3f, 0.3f);

  auto sourceMat             = StandardMaterial::New("sourceMat", scene);
  sourceMat->wireframe       = true;
  sourceMat->backFaceCulling = false;

  auto a = Mesh::CreateSphere("sphere", 16, 4, scene);
  auto b = Mesh::CreateBox("box", 4, scene);
  // auto c = Mesh::CreateBox("box", 4, scene);

  a->material = sourceMat;
  b->material = sourceMat;
  // c->material = sourceMat;

  a->position().y += 5.f;
  b->position().y += 2.5f;
  // c->position().y += 3.5f;
  // c->rotation().y += Math::PI / 8.f;

  auto aCSG = CSG::CSG::FromMesh(a);
  auto bCSG = CSG::CSG::FromMesh(b);
  // auto cCSG = CSG::CSG::FromMesh(c);

  // Set up a MultiMaterial
  auto mat0 = StandardMaterial::New("mat0", scene);
  // auto mat1 = StandardMaterial::New("mat1", scene);

  mat0->diffuseColor.copyFromFloats(0.8f, 0.2f, 0.2f);
  mat0->backFaceCulling = false;
#if 0
  mat1->diffuseColor.copyFromFloats(0.2f, 0.8f, 0.2f);
  mat1->backFaceCulling = false;
#endif
  auto subCSG       = bCSG->subtract(aCSG);
  auto newMesh      = subCSG.toMesh("csg", mat0, scene);
  newMesh->position = Vector3(-10, 0, 0);
#if 0
  subCSG            = aCSG->subtract(bCSG);
  newMesh           = subCSG.toMesh("csg2", mat0, scene);
  newMesh->position = Vector3(10, 0, 0);

  subCSG            = aCSG->intersect(bCSG);
  newMesh           = subCSG.toMesh("csg3", mat0, scene);
  newMesh->position = Vector3(0, 0, 10);

  // Submeshes are built in order : mat0 will be for the first cube, and mat1
  // for the second
  auto multiMat = MultiMaterial::New("multiMat", scene);
  multiMat->subMaterials().emplace_back(mat0);
  multiMat->subMaterials().emplace_back(mat1);

  // Last parameter to true means you want to build 1 subMesh for each mesh
  // involved
  subCSG            = bCSG->subtract(cCSG);
  newMesh           = subCSG.toMesh("csg4", multiMat, scene, true);
  newMesh->position = Vector3(0, 0, -10);
#endif
}

} // end of namespace Samples
} // end of namespace BABYLON
