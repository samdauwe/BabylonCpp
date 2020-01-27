#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>
#include <babylon/samples/samples_common_code.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Inner Mesh Points Scene. Example demonstrating how to create points inside a mesh.
 * @see https://www.babylonjs-playground.com/#2K3T61#8
 * @see https://doc.babylonjs.com/snippets/innermeshpoints
 */
struct InnerMeshPointsScene : public IRenderableScene {

  InnerMeshPointsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~InnerMeshPointsScene() override = default;

  const char* getName() override
  {
    return "Inner Mesh Points Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 8.f, 15.f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light    = HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);
    auto pl       = PointLight::New("pl", Vector3::Zero(), scene);
    pl->diffuse   = Color3(1.f, 1.f, 1.f);
    pl->specular  = Color3(0.5f, 0.2f, 0.2f);
    pl->intensity = 0.75f;

    // Create 12 Pointed Star Mesh
    auto tps                   = MeshUtils::CreateTwelvePointedStar(3ull, scene);
    tps->material              = StandardMaterial::New("", scene);
    tps->material()->wireframe = true;

    auto nbParticles = 5000ull;
    auto points      = tps->createInnerPoints(nbParticles);
    SphereOptions options;
    options.diameter = 0.05f;
    options.segments = 6u;
    auto sphere      = MeshBuilder::CreateSphere("point", options, scene);
    for (size_t p = 0; p < nbParticles; ++p) {
      sphere->createInstance("");
      sphere->position = points[p].copy();
    }
  }

}; // end of struct InnerMeshPointsScene

BABYLON_REGISTER_SAMPLE("Meshes", InnerMeshPointsScene)

} // end of namespace Samples
} // end of namespace BABYLON
