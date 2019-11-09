#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Multi material scene. Example demonstrating how to apply multiple materials to a single
 * mesh using MultiMaterial class
 * @see https://www.babylonjs-playground.com/#2Q4S2S#0
 * @see https://doc.babylonjs.com/how_to/multi_materials
 */
class MultiMaterialScene : public IRenderableScene {

public:
  MultiMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _sphere{nullptr}
  {
  }

  ~MultiMaterialScene() override = default;

  const char* getName() override
  {
    return "Multi Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light       = PointLight::New("Omni", Vector3(20.f, 100.f, 2.f), scene);
    light->intensity = 0.97f;

    auto material0          = StandardMaterial::New("mat0", scene);
    material0->diffuseColor = Color3(1.f, 0.f, 0.f);
    material0->bumpTexture  = Texture::New("textures/normalMap.jpg", scene);

    auto material1          = StandardMaterial::New("mat1", scene);
    material1->diffuseColor = Color3(0.f, 0.f, 1.f);

    auto material2           = StandardMaterial::New("mat2", scene);
    material2->emissiveColor = Color3(0.4f, 0.f, 0.4f);

    auto multimat = MultiMaterial::New("multi", scene);
    multimat->subMaterials().emplace_back(material0);
    multimat->subMaterials().emplace_back(material1);
    multimat->subMaterials().emplace_back(material2);

    _sphere           = Mesh::CreateSphere("Sphere0", 16, 3, scene);
    _sphere->material = multimat;

    _sphere->subMeshes.clear();
    auto verticesCount = _sphere->getTotalVertices();

    SubMesh::New(0, 0, verticesCount, 0, 900, _sphere);
    SubMesh::New(1, 0, verticesCount, 900, 900, _sphere);
    SubMesh::New(2, 0, verticesCount, 1800, 2088, _sphere);

    camera->setPosition(Vector3(-3.f, 3.f, 0.f));

    // Animations
    scene->registerBeforeRender(
      [this](Scene* /*scene*/, EventState& /*es*/) { _sphere->rotation().y += 0.02f; });
  }

private:
  MeshPtr _sphere;

}; // end of class MultiMaterialScene

BABYLON_REGISTER_SAMPLE("Materials", MultiMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
