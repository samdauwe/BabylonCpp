#include <babylon/samples/meshes/colored_ribbon_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/extensions/noisegeneration/perlin_noise.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {
namespace Samples {

ColoredRibbonScene::ColoredRibbonScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ColoredRibbonScene::~ColoredRibbonScene()
{
}

const char* ColoredRibbonScene::getName()
{
  return "Colored Ribbon Scene";
}

void ColoredRibbonScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color3::Black();

  // Create camera and lights
  auto camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
  camera->wheelPrecision = 10.0;
  camera->setPosition(Vector3(0.f, 50.f, -50.f));
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity   = 0.75f;
  light->groundColor = Color3::Gray();
  light->specular    = Color3::Black();

  auto pl = PointLight::New("pl", camera->position, scene);

  // Add a skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 1000.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  const std::vector<std::string> files{
    "textures/skybox/space_left.jpg",  //
    "textures/skybox/space_up.jpg",    //
    "textures/skybox/space_front.jpg", //
    "textures/skybox/space_right.jpg", //
    "textures/skybox/space_down.jpg",  //
    "textures/skybox/space_back.jpg",  //
  };
  auto reflectionTexture = CubeTexture::CreateFromImages(files, scene);
  reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->reflectionTexture  = reflectionTexture;
  skyboxMaterial->diffuseColor       = Color3::Black();
  skyboxMaterial->specularColor      = Color3::Black();
  skyboxMaterial->disableLighting    = true;
  skybox->infiniteDistance           = true;
  skybox->material                   = skyboxMaterial;

  // Map data creation
  // The map is a flat array of successive 3D coordinates (x, y, z).
  // It's defined by a number of points on its width : mapSubX
  // and a number of points on its height : mapSubZ

  auto mapSubX        = 100;  // point number on X axis
  auto mapSubZ        = 100;  // point number on Z axis
  uint32_t seed       = 10;   // seed
  auto noiseScale     = 0.3f; // noise frequency
  auto elevationScale = 12.f;
  Extensions::PerlinNoise noise(seed);
  std::vector<std::vector<Vector3>> paths; // array for the ribbon model
  std::vector<Color4> colors;              // array for the ribbon model
  for (auto l = 0; l < mapSubZ; l++) {
    std::vector<Vector3> path; // only for the ribbon
    for (auto w = 0; w < mapSubX; w++) {
      auto x = (w - mapSubX * 0.5f) * 2.f;
      auto z = (l - mapSubZ * 0.5f) * 2.f;
      auto y
        = static_cast<float>(noise.noise(static_cast<double>(x * noiseScale),
                                         static_cast<double>(z * noiseScale)));
      // let's increase a bit the noise computed altitude
      y *= (0.5f + y) * y * elevationScale;
      // Colors of the map
      auto r = y / 4.f;
      auto g = 0.5f;
      auto b = 0.5f;
      colors.emplace_back(Color4(r, g, b, 1.f));
      path.emplace_back(Vector3(x, y, z));
    }
    paths.emplace_back(path);
  }

  RibbonOptions options(paths);
  options.colors          = std::move(colors);
  options.sideOrientation = 1;
  auto map                = MeshBuilder::CreateRibbon("m", options, scene);
  map->position().y       = -1.f;
  map->convertToFlatShadedMesh();
}

} // end of namespace Samples
} // end of namespace BABYLON
