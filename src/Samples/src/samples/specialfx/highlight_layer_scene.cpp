#include <babylon/samples/specialfx/highlight_layer_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

HighlightLayerScene::HighlightLayerScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _alpha{0.01f}, _hl{nullptr}
{
}

HighlightLayerScene::~HighlightLayerScene()
{
}

const char* HighlightLayerScene::getName()
{
  return "Highlight Layer Scene";
}

void HighlightLayerScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f,
                                     200.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);
  camera->minZ = 0.1f;

  // Light
  auto light       = PointLight::New("point", Vector3(0.f, 40.f, 0.f), scene);
  light->intensity = 0.98f;

  // Environment Texture
  auto hdrTexture = HDRCubeTexture::New("/textures/room.hdr", scene, 512);

  // Skybox
  auto hdrSkybox         = Mesh::CreateBox("hdrSkyBox", 1000.f, scene);
  auto hdrSkyboxMaterial = PBRMaterial::New("skyBox", scene);
  hdrSkyboxMaterial->backFaceCulling   = false;
  hdrSkyboxMaterial->reflectionTexture = hdrTexture->clone();
  hdrSkyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  hdrSkyboxMaterial->microSurface    = 1.f;
  hdrSkyboxMaterial->cameraExposure  = 0.6f;
  hdrSkyboxMaterial->cameraContrast  = 1.6f;
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
  glass->directIntensity                = 0.f;
  glass->environmentIntensity           = 0.5f;
  glass->cameraExposure                 = 0.5f;
  glass->cameraContrast                 = 1.7f;
  glass->microSurface                   = 1.f;
  glass->reflectivityColor              = Color3(0.2f, 0.2f, 0.2f);
  glass->albedoColor                    = Color3(0.95f, 0.95f, 0.95f);
  sphereGlass->material                 = glass;

  auto metal                  = PBRMaterial::New("metal", scene);
  metal->reflectionTexture    = hdrTexture;
  metal->directIntensity      = 0.3f;
  metal->environmentIntensity = 0.7f;
  metal->cameraExposure       = 0.55f;
  metal->cameraContrast       = 1.6f;
  metal->microSurface         = 0.96f;
  metal->reflectivityColor    = Color3(0.9f, 0.9f, 0.9f);
  metal->albedoColor          = Color3(1.f, 1.f, 1.f);
  sphereMetal->material       = metal;

  auto plastic                  = PBRMaterial::New("plastic", scene);
  plastic->reflectionTexture    = hdrTexture;
  plastic->directIntensity      = 0.6f;
  plastic->environmentIntensity = 0.7f;
  plastic->cameraExposure       = 0.6f;
  plastic->cameraContrast       = 1.6f;
  plastic->microSurface         = 0.96f;
  plastic->albedoColor          = Color3(0.206f, 0.94f, 1.f);
  plastic->reflectivityColor    = Color3(0.05f, 0.05f, 0.05f);
  spherePlastic->material       = plastic;

  auto wood                  = PBRMaterial::New("wood", scene);
  wood->reflectionTexture    = hdrTexture;
  wood->directIntensity      = 1.5f;
  wood->environmentIntensity = 0.5f;
  wood->specularIntensity    = 0.3f;
  wood->cameraExposure       = 0.9f;
  wood->cameraContrast       = 1.6f;

  wood->reflectivityTexture = Texture::New("textures/reflectivity.png", scene);
  wood->useMicroSurfaceFromReflectivityMapAlpha = true;

  wood->albedoColor   = Color3::White();
  wood->albedoTexture = Texture::New("textures/albedo.png", scene);
  woodPlank->material = wood;

  _hl = HighlightLayer::New("hl", scene);
  _hl->addMesh(sphereMetal, Color3::White());
  auto hl2 = HighlightLayer::New("hl", scene);
  hl2->addMesh(spherePlastic, Color3::Green());
  auto hl3 = HighlightLayer::New("hl", scene);
  hl3->addMesh(sphereGlass, Color3::Red());

  _scene->registerBeforeRender([this](Scene*, EventState&) {
    _hl->blurHorizontalSize = 0.4f + std::cos(_alpha);
    _hl->blurVerticalSize   = 0.4f + std::cos(_alpha);

    _alpha += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
