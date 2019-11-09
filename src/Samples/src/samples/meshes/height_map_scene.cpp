#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Height map scene. Example demonstrating how to use a height map to
 * extrude a plane to create mountains.
 * @see https://www.babylonjs-playground.com/#95PXRY#0
 */
class HeightMapScene : public IRenderableScene {

public:
  HeightMapScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _positionStepX{0.5f}, reverseSun{true}
  {
  }

  ~HeightMapScene() override = default;

  const char* getName() override
  {
    return "Height Map Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Light
    _spot           = PointLight::New("spot", Vector3(0.f, 30.f, 10.f), scene);
    _spot->diffuse  = Color3(1.f, 1.f, 1.f);
    _spot->specular = Color3(0.f, 0.f, 0.f);

    // Camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 80.f, Vector3::Zero(), scene);
    camera->lowerBetaLimit   = 0.1f;
    camera->upperBetaLimit   = Math::PI_2 * 0.9f;
    camera->lowerRadiusLimit = 30.f;
    camera->upperRadiusLimit = 150.f;
    camera->attachControl(canvas, true);

    // Ground
    auto groundMaterial            = StandardMaterial::New("ground", scene);
    groundMaterial->diffuseTexture = Texture::New("textures/earth.jpg", scene);

    auto ground = Mesh::CreateGroundFromHeightMap("ground", "textures/worldHeightMap.jpg", 200, 100,
                                                  250, 0, 10, scene, false);
    ground->material = groundMaterial;

    // Sphere to see the light's position
    _sun                        = Mesh::CreateSphere("sun", 10, 4, scene);
    auto _sunMaterial           = StandardMaterial::New("sun", scene);
    _sunMaterial->emissiveColor = Color3(1.f, 1.f, 0.f);
    _sun->material              = _sunMaterial;

    // Skybox
    auto skybox                       = Mesh::CreateBox("skyBox", 800.f, scene);
    auto skyboxMaterial               = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = CubeTexture::New("textures/skybox/skybox", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->disableLighting                      = true;
    skybox->material                                     = skyboxMaterial;

    // Sun animation
    scene->registerBeforeRender([&](Scene* /*scene*/, EventState& /*es*/) {
      _sun->position = _spot->position();
      _spot->position().x -= _positionStepX;
      if (_spot->position().x < -90.f) {
        if (reverseSun) {
          _positionStepX = -_positionStepX;
        }
        else {
          _spot->position().x = 100.f;
        }
      }
      if (_spot->position().x > 90.f && reverseSun) {
        _positionStepX = -_positionStepX;
      }
    });
  }

private:
  float _positionStepX;
  bool reverseSun;
  MeshPtr _sun;
  PointLightPtr _spot;

}; // end of class HeightMapScene

BABYLON_REGISTER_SAMPLE("Meshes", HeightMapScene)

} // end of namespace Samples
} // end of namespace BABYLON
