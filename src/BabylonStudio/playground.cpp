#ifdef BABYLON_BUILD_PLAYGROUND

// This is the playground : you can here enter / modify code during the execution, 
// and the scene will be updated automatically!

// Do not remove the includes below, they are needed to enable Runtime Compilation.
#include "BabylonStudio/rtc/playground_autocompile.h"
#include <babylon/interfaces/irenderable_scene_with_hud.h>
#include "imgui.h"

// You can edit the code below freely (add includes, modify the scene, etc)
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

using namespace BABYLON;

struct PlaygroundScene : public IRenderableSceneWithHud {

  PlaygroundScene(ICanvas* iCanvas = nullptr) : IRenderableSceneWithHud(iCanvas) {}
  ~PlaygroundScene() override = default;
  const char* getName() override { return "Materials Scene"; }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {

  auto camera = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f,
                                     200.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);
  camera->minZ = 0.1f;

  // Environment Texture
  auto hdrTexture
    = CubeTexture::CreateFromPrefilteredData("textures/environment.dds", scene);

  scene->imageProcessingConfiguration()->exposure = 0.6f;
  scene->imageProcessingConfiguration()->contrast = 1.6f;

  // Skybox
  auto hdrSkybox         = Mesh::CreateBox("hdrSkyBox", 1000.f, scene);
  auto hdrSkyboxMaterial = PBRMaterial::New("skyBox", scene);
  hdrSkyboxMaterial->backFaceCulling   = false;
  hdrSkyboxMaterial->reflectionTexture = hdrTexture->clone();
  hdrSkyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  hdrSkyboxMaterial->microSurface    = 1.f;
  hdrSkyboxMaterial->disableLighting = true;
  hdrSkybox->material                = hdrSkyboxMaterial;
  hdrSkybox->infiniteDistance        = true;

  // Create meshes
  auto sphereGlass = Mesh::CreateSphere("sphereGlass", 48, 30.f, scene);
  sphereGlass->translate(Vector3(1.f, 0.f, 0.f), -60.f);

  auto sphereMetal = Mesh::CreateSphere("sphereMetal", 48, 30.f, scene);
  sphereMetal->translate(Vector3(1.f, 0.f, 0.f), 60.f);

  auto spherePlastic = Mesh::CreateSphere("spherePlastic", 48, 30.f, scene);
  spherePlastic->translate(Vector3(0.f, 0.f, 1.f), -60.f);

  BoxOptions woodPlankOptions;
  woodPlankOptions.width  = 65.f;
  woodPlankOptions.height = 1.f;
  woodPlankOptions.depth  = 65.f;
  auto woodPlank = MeshBuilder::CreateBox("plane", woodPlankOptions, scene);

  // Create materials
  auto glass                            = PBRMaterial::New("glass", scene);
  glass->reflectionTexture              = hdrTexture;
  glass->refractionTexture              = hdrTexture;
  glass->linkRefractionWithTransparency = true;
  glass->indexOfRefraction              = 0.52f;
  glass->alpha                          = 0.f;
  glass->microSurface                   = 1;
  glass->reflectivityColor              = Color3(0.2f, 0.2f, 0.2f);
  glass->albedoColor                    = Color3(0.85f, 0.85f, 0.85f);
  sphereGlass->material                 = glass;

  auto metal               = PBRMaterial::New("metal", scene);
  metal->reflectionTexture = hdrTexture;
  metal->microSurface      = 0.96f;
  metal->reflectivityColor = Color3(0.85f, 0.85f, 0.85f);
  metal->albedoColor       = Color3(0.01f, 0.01f, 0.01f);
  sphereMetal->material    = metal;

  auto plastic               = PBRMaterial::New("plastic", scene);
  plastic->reflectionTexture = hdrTexture;
  plastic->microSurface      = 0.96f;
  plastic->albedoColor       = Color3(0.206f, 0.94f, 1.f);
  plastic->reflectivityColor = Color3(0.003f, 0.003f, 0.003f);
  spherePlastic->material    = plastic;

  auto wood                  = PBRMaterial::New("wood", scene);
  wood->reflectionTexture    = hdrTexture;
  wood->environmentIntensity = 1.f;
  wood->specularIntensity    = 0.3f;

  wood->reflectivityTexture = Texture::New("textures/reflectivity.png", scene);
  wood->useMicroSurfaceFromReflectivityMapAlpha = true;

  wood->albedoColor   = Color3::White();
  wood->albedoTexture = Texture::New("textures/albedo.png", scene);
  woodPlank->material = wood;
  
  auto sliderFloatProperty = [](const char * label, auto & v, float min, float max) {
  	float f = v;
  	ImGui::SliderFloat(label, &f, min, max);
  	v = f;
  };
  
    // Here you can create a HUD (Head Up Display) GUI
    // that enables you to tweak the 3D params
    hudGui = [=]() { // capture Ptrs by value here, not by reference
    	sliderFloatProperty("Refraction", glass->indexOfRefraction, 0., 3.);
    	sliderFloatProperty("MicroSurface", glass->microSurface, 0.f, 1.f);
    };

  }
};



////////////////////////////////////////////
// Do not edit below, these lines enable the Runtime Compilation
REGISTERCLASS(Playground)
std::shared_ptr<BABYLON::IRenderableSceneWithHud> Playground::MakeScene() {
  return std::make_shared<PlaygroundScene>();
}

#endif // #ifdef BABYLON_BUILD_PLAYGROUND


