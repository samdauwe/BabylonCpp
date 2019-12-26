#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/sky/sky_material.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Scene demonstrating the use of the sky material from the materials library.
 * Press the following keys:
 * - 1: Set Day
 * - 2: Set Evening
 * - 3: Increase Luminance
 * - 4: Decrease Luminance
 * - 5: Increase Turbidity
 * - 6: Decrease Turbidity
 * @see https://www.babylonjs-playground.com/#E6OZX#122
 * @see https://doc.babylonjs.com/extensions/Sky
 */
class SkyMaterialScene : public IRenderableScene {

public:
  SkyMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas), _skyboxMaterial{nullptr}
  {
  }

  ~SkyMaterialScene() override = default;

  const char* getName() override
  {
    return "Sky Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = FreeCamera::New("camera1", Vector3(5.f, 4.f, -47.f), scene);
    camera->setTarget(Vector3::Zero());
    camera->attachControl(canvas, true);

    // Light
    auto light       = HemisphericLight::New("light", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 1.f;

    // Ground
    auto ground = Mesh::CreateGroundFromHeightMap("ground", "textures/heightMap.png", 100, 100, 100,
                                                  0, 10, scene, false);
    auto groundMaterial            = StandardMaterial::New("ground", scene);
    auto groundtexture             = Texture::New("textures/ground.jpg", scene);
    groundtexture->uScale          = 6.f;
    groundtexture->vScale          = 6.f;
    groundMaterial->specularColor  = Color3(0.f, 0.f, 0.f);
    groundMaterial->diffuseTexture = groundtexture;
    ground->position().y           = -2.05f;
    ground->material               = groundMaterial;

    // Sky material
    _skyboxMaterial                  = MaterialsLibrary::SkyMaterial::New("skyMaterial", scene);
    _skyboxMaterial->backFaceCulling = false;

    // Sky mesh (box)
    auto skybox      = Mesh::CreateBox("skyBox", 1000.f, scene);
    skybox->material = _skyboxMaterial;

    // Events
    canvas->addKeyEventListener(EventType::KEY_DOWN, [this](KeyboardEvent&& evt) {
      switch (evt.keyCode) {
        case 49: // 1
          setSkyConfig("material.inclination", _skyboxMaterial->inclination, 0.f);
          break;
        case 50: // 2
          setSkyConfig("material.inclination", _skyboxMaterial->inclination, -0.5f);
          break;
        case 51: // 3
          setSkyConfig("material.luminance", _skyboxMaterial->luminance, 0.1f);
          break;
        case 52: // 4
          setSkyConfig("material.luminance", _skyboxMaterial->luminance, 1.f);
          break;
        case 53: // 5
          setSkyConfig("material.turbidity", _skyboxMaterial->turbidity, 40.f);
          break;
        case 54: // 6
          setSkyConfig("material.turbidity", _skyboxMaterial->turbidity, 5.f);
          break;
        default:
          break;
      }
    });

    // Set to Day by default
    setSkyConfig("material.inclination", _skyboxMaterial->inclination, 0.f);
  }

private:
  void setSkyConfig(const std::string& property, float /*from*/, float to)
  {
    if (property == "material.inclination") {
      _skyboxMaterial->inclination = to;
    }
    else if (property == "material.luminance") {
      _skyboxMaterial->luminance = to;
    }
    else if (property == "material.turbidity") {
      _skyboxMaterial->turbidity = to;
    }
  }

private:
  MaterialsLibrary::SkyMaterialPtr _skyboxMaterial;

}; // end of class SkyMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", SkyMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
