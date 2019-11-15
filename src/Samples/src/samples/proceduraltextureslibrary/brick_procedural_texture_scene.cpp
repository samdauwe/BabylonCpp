#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief BrickProceduralTextureScene Scene. Example demonstrating how to use brick procedural
 * texture.
 * @see https://www.babylonjs-playground.com/#1CL0BO
 * @see https://doc.babylonjs.com/extensions/BrickProceduralTexture
 */
struct BrickProceduralTextureScene : public IRenderableScene {

  BrickProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~BrickProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Brick Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 2.8f * Math::PI_2, Math::PI_2, 15.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto brickMaterial = StandardMaterial::New("BrickMaterial", scene);
    auto brickTexture
      = ProceduralTexturesLibrary::BrickProceduralTexture::New("BrickMaterialtext", 512, scene);
    brickTexture->numberOfBricksHeight = 6.f;
    brickTexture->numberOfBricksWidth  = 10.f;
    brickMaterial->diffuseTexture      = brickTexture;

    auto plane      = Mesh::CreatePlane("BrickPlane", 10, scene);
    plane->material = brickMaterial;
  }

}; // end of struct BrickProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", BrickProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
