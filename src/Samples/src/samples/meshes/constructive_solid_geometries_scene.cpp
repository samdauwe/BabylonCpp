#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/csg/csg.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Constructive_solid_geometries_scene. Example demonstrating how to use boolean operations
 * on meshes using CSG.
 * @see https://www.babylonjs-playground.com/#T6NP3F#0
 */
struct ConstructiveSolidGeometriesScene : public IRenderableScene {

  ConstructiveSolidGeometriesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ConstructiveSolidGeometriesScene() override = default;

  const char* getName() override
  {
    return "Constructive Solid Geometries Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto light  = DirectionalLight::New("dir01", Vector3(0.f, -0.5f, -1.f), scene);
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(10.f, 10.f, 10.f));
    light->position = Vector3(20.f, 150.f, 70.f);
    camera->minZ    = 10.f;
    camera->attachControl(canvas, true);

    scene->ambientColor = Color3(0.3f, 0.3f, 0.3f);

    auto sourceMat             = StandardMaterial::New("sourceMat", scene);
    sourceMat->wireframe       = true;
    sourceMat->backFaceCulling = false;

    auto a = Mesh::CreateSphere("sphere", 16, 4, scene);
    auto b = Mesh::CreateBox("box", 4, scene);
    auto c = Mesh::CreateBox("box", 4, scene);

    a->material = sourceMat;
    b->material = sourceMat;
    c->material = sourceMat;

    a->position().y += 5.f;
    b->position().y += 2.5f;
    c->position().y += 3.5f;
    c->rotation().y += Math::PI / 8.f;

    auto aCSG = CSG::CSG::FromMesh(a);
    auto bCSG = CSG::CSG::FromMesh(b);
    auto cCSG = CSG::CSG::FromMesh(c);

    // Set up a MultiMaterial
    auto mat0 = StandardMaterial::New("mat0", scene);
    auto mat1 = StandardMaterial::New("mat1", scene);

    mat0->diffuseColor.copyFromFloats(0.8f, 0.2f, 0.2f);
    mat0->backFaceCulling = false;

    mat1->diffuseColor.copyFromFloats(0.2f, 0.8f, 0.2f);
    mat1->backFaceCulling = false;

    auto subCSG       = bCSG->subtract(aCSG);
    auto newMesh      = subCSG.toMesh("csg", mat0, scene);
    newMesh->position = Vector3(-10.f, 0.f, 0.f);

    subCSG            = aCSG->subtract(bCSG);
    newMesh           = subCSG.toMesh("csg2", mat0, scene);
    newMesh->position = Vector3(10.f, 0.f, 0.f);

    subCSG            = aCSG->intersect(bCSG);
    newMesh           = subCSG.toMesh("csg3", mat0, scene);
    newMesh->position = Vector3(0.f, 0.f, 10.f);

    // Submeshes are built in order : mat0 will be for the first cube, and mat1
    // for the second
    auto multiMat = MultiMaterial::New("multiMat", scene);
    multiMat->subMaterials().emplace_back(mat0);
    multiMat->subMaterials().emplace_back(mat1);

    // Last parameter to true means you want to build 1 subMesh for each mesh
    // involved
    subCSG            = bCSG->subtract(cCSG);
    newMesh           = subCSG.toMesh("csg4", multiMat, scene, true);
    newMesh->position = Vector3(0.f, 0.f, -10.f);
  }

}; // end of struct ConstructiveSolidGeometriesScene

BABYLON_REGISTER_SAMPLE("Meshes", ConstructiveSolidGeometriesScene)

} // end of namespace Samples
} // end of namespace BABYLON
