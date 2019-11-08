#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {
namespace Samples {

/**
 * @brief Light projected texture scene. Example demonstrating how to project a
 * texture from a spot light.
 * @see https://www.babylonjs-playground.com/#CQNGRK#0
 * @see https://doc.babylonjs.com/babylon101/lights#projection-texture
 */
class LightProjectedTextureScene : public IRenderableScene {

public:
  LightProjectedTextureScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _spotLight{nullptr}, _alpha{0.f}
  {
  }
  ~LightProjectedTextureScene()
  {
  }

  const char* getName() override
  {
    return "Light Projected Texture Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Setup environment
    auto camera
      = ArcRotateCamera::New("Camera", 0.f, 0.8f, 90.f, Vector3::Zero(), scene);
    camera->lowerBetaLimit   = 0.1f;
    camera->upperBetaLimit   = Math::PI_2 * 0.9f;
    camera->lowerRadiusLimit = 30.f;
    camera->upperRadiusLimit = 150.f;
    camera->attachControl(canvas, true);
  
    // Hemispheric light
    auto light = HemisphericLight::New("dir01", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.1f;
  
    // Spot light
    _spotLight = SpotLight::New("spot02", Vector3(30.f, 40.f, 30.f),
                                Vector3(-1.f, -2.f, -1.f), 1.1f, 16.f, scene);
    _spotLight->projectionTexture = Texture::New("textures/co.png", scene);
    _spotLight->setDirectionToTarget(Vector3::Zero());
    _spotLight->intensity = 1.5f;
  
    // Ground
    auto ground = Mesh::CreateGroundFromHeightMap(
      "ground", "textures/heightMap.png", 100, 100, 100, 0, 10, scene, false);
    auto groundMaterial            = StandardMaterial::New("ground", scene);
    groundMaterial->diffuseTexture = Texture::New("textures/ground.jpg", scene);
    auto diffuseTexture
      = std::static_pointer_cast<Texture>(groundMaterial->diffuseTexture());
    diffuseTexture->uScale        = 6.f;
    diffuseTexture->vScale        = 6.f;
    groundMaterial->specularColor = Color3(0.f, 0.f, 0.f);
    ground->position().y          = -2.05f;
    ground->material              = groundMaterial;
  
    // Animations
    scene->registerBeforeRender([&](Scene* /*scene*/, EventState& /*es*/) {
      _spotLight->position
        = Vector3(std::cos(_alpha) * 60.f, 40.f, std::sin(_alpha) * 60.f);
      _spotLight->setDirectionToTarget(Vector3::Zero());
      _alpha += 0.01f;
    });
  }

private:
  SpotLightPtr _spotLight;
  float _alpha;

}; // end of class LightProjectedTextureScene

BABYLON_REGISTER_SAMPLE("Ligths", LightProjectedTextureScene)
} // end of namespace Samples
} // end of namespace BABYLON