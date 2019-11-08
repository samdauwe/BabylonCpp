#include <babylon/samples/samples_index.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/materialslibrary/fire/fire_material.h>
#include <babylon/math/scalar.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the fire material from the materials
 * library.
 * @see https://www.babylonjs-playground.com/#LR4YHT#0
 * @see https://doc.babylonjs.com/extensions/Fire
 */
class FireMaterialScene : public IRenderableScene {

public:
  FireMaterialScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _rootMesh{nullptr}
      , _fireMaterial{nullptr}
      , _shadowGenerator{nullptr}
      , _renderWithShadows{false}
  {
  }
  ~FireMaterialScene()
  {
  }

  const char* getName() override
  {
    return "Fire Material Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI_4,
                                       30.f, Vector3(0.f, 4.f, 0.f), scene);
    camera->attachControl(canvas, true);
  
    // Light
    auto light = SpotLight::New("light", Vector3(8.f, 16.f, 8.f),
                                Vector3(-1.f, -2.f, -1.f), 3.f, 1.f, scene);
    if (_renderWithShadows) {
      _shadowGenerator = ShadowGenerator::New(512, light);
      _shadowGenerator->useBlurExponentialShadowMap = true;
      _shadowGenerator->blurBoxOffset               = 1;
      _shadowGenerator->blurScale                   = 1.f;
      _shadowGenerator->setDarkness(0.f);
    }
  
    // Fire light, simulates the fire intensity
    auto light2     = PointLight::New("light2", Vector3(0.f, 10.f, 0.f), scene);
    light2->diffuse = Color3(1.f, 0.5f, 0.f);
  
    std::vector<IAnimationKey> keys;
    std::optional<float> previous = std::nullopt;
    for (int i = 0; i < 20; i++) {
      auto rand = Scalar::Clamp(Math::random(), 0.5f, 1.f);
  
      if (previous.has_value()) {
        if (std::abs(rand - *previous) < 0.1f) {
          continue;
        }
      }
  
      previous = rand;
  
      keys.emplace_back(IAnimationKey(static_cast<float>(i), // frame
                                      AnimationValue(rand)   // value
                                      ));
    }
  
    auto anim
      = Animation::New("anim", "intensity", 1, Animation::ANIMATIONTYPE_FLOAT(),
                       Animation::ANIMATIONLOOPMODE_CYCLE());
    anim->setKeys(keys);
  
    light2->animations.emplace_back(anim);
    scene->beginAnimation(light2, 0.f, static_cast<float>(keys.size()), true, 8);
  
    // Skybox
    auto skybox                     = Mesh::CreateBox("skyBox", 300.f, scene);
    auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling = false;
    auto reflectionTexture = CubeTexture::New("textures/skybox/santa", scene);
    reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->reflectionTexture  = reflectionTexture;
    skyboxMaterial->diffuseColor       = Color3::Black();
    skyboxMaterial->specularColor      = Color3::Black();
    skyboxMaterial->disableLighting    = true;
    skybox->material                   = skyboxMaterial;
  
    // Fire material
    _fireMaterial = MaterialsLibrary::FireMaterial::New("fire", scene);
    _fireMaterial->diffuseTexture
      = Texture::New("textures/fire/diffuse.png", scene);
    _fireMaterial->distortionTexture
      = Texture::New("textures/fire/distortion.png", scene);
    auto opacityTexture   = Texture::New("textures/fire/opacity.png", scene);
    opacityTexture->level = 0.5f;
    _fireMaterial->opacityTexture = opacityTexture;
    _fireMaterial->speed          = 5.f;
  
    // Load candle 3D Model
    _rootMesh          = Mesh::New("root", scene);
    _rootMesh->scaling = Vector3(4.f, 4.f, 4.f);
    light2->parent     = _rootMesh.get();
  
    SceneLoader::ImportMesh(
      {}, "scenes/", "candle.babylon", scene,
      [this, scene](const std::vector<AbstractMeshPtr>& meshes,
                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                    const std::vector<SkeletonPtr>& /*skeletons*/,
                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        auto plane            = scene->getMeshByName("Plane");
        plane->receiveShadows = true;
  
        for (const auto& mesh : meshes) {
          if (_renderWithShadows && mesh != plane) {
            _shadowGenerator->getShadowMap()->renderList().emplace_back(
              mesh.get());
            mesh->receiveShadows = false;
          }
  
          if (!mesh->parent()) {
            mesh->parent = _rootMesh.get();
          }
        }
  
        auto torus            = scene->getMeshByName("Torus");
        torus->receiveShadows = true;
  
        // Create the fire plane (billboarded on Y)
        plane                = Mesh::CreatePlane("firePlane", 1.5f, scene);
        plane->position      = Vector3(0.f, 8.3f, 0.f);
        plane->scaling().x   = 0.45f;
        plane->scaling().y   = 1.5f;
        plane->billboardMode = Mesh::BILLBOARDMODE_Y;
        plane->material      = _fireMaterial;
      });
  }

private:
  MeshPtr _rootMesh;
  MaterialsLibrary::FireMaterialPtr _fireMaterial;
  ShadowGeneratorPtr _shadowGenerator;
  bool _renderWithShadows;

}; // end of class FireMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", FireMaterialScene)
} // end of namespace Samples
} // end of namespace BABYLON