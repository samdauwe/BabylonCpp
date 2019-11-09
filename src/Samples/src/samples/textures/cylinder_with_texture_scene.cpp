#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cylinder With Texture Scene. Example demonstrating how to wrap an image image around a
 * cylinder.
 * @see https://www.babylonjs-playground.com/#VA2AC#3
 * @see https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
 */
struct CylinderWithTextureScene : public IRenderableScene {

  CylinderWithTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~CylinderWithTextureScene() override = default;

  const char* getName() override
  {
    return "Cylinder With Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", Math::PI_2, 7.f * Math::PI / 16.f, 2.5f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light1       = HemisphericLight::New("hemiLight1", Vector3(-1.f, 10.f, -5.f), scene);
    light1->intensity = 0.97f;
    auto light2       = HemisphericLight::New("hemiLight2", Vector3(-10.f, -10.f, -5.f), scene);
    light2->intensity = 0.97f;

    auto canMaterial            = StandardMaterial::New("material", scene);
    canMaterial->diffuseTexture = Texture::New("textures/cannedJam.jpg", scene);

    std::vector<Vector4> faceUV{
      Vector4(0.f, 0.f, 0.f, 0.f),   //
      Vector4(1.f, 0.f, 0.32f, 1.f), //
      Vector4(0.f, 0.f, 0.25f, 1.f)  //
    };

    std::vector<Color4> faceColors{Color4(0.5, 0.5, 0.5, 1)};

    CylinderOptions options;
    options.height     = 1.16f;
    options.faceUV     = std::move(faceUV);
    options.faceColors = std::move(faceColors);

    auto can      = MeshBuilder::CreateCylinder("can", options, scene);
    can->material = canMaterial;
  }

}; // end of struct CylinderWithTextureScene

BABYLON_REGISTER_SAMPLE("Textures", CylinderWithTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
