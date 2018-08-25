#include <babylon/samples/textures/procedural_textures_scene.h>

#include <babylon/babylon_enums.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/procedurals/custom_procedural_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/cloud/cloud_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/fire/fire_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/road/road_procedural_texture.h>
#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>

namespace BABYLON {

using namespace ProceduralTexturesLibrary;

namespace Samples {

ProceduralTexturesScene::ProceduralTexturesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ProceduralTexturesScene::~ProceduralTexturesScene()
{
}

const char* ProceduralTexturesScene::getName()
{
  return "Procedural Textures Scene";
}

void ProceduralTexturesScene::initializeScene(ICanvas* /*canvas*/,
                                              Scene* /*scene*/)
{
#if 0
  // Helper functions
  const auto CreateBosquet
    = [](const std::string& name, float x, float y, float z, Scene* scene,
         ShadowGenerator* shadowGenerator, const MaterialPtr& grassMaterial) {
        auto bosquet      = Mesh::CreateBox(name, 2.f, scene);
        bosquet->position = Vector3(x, y, z);
        bosquet->material = grassMaterial;

        auto bosquetbawl      = Mesh::CreateBox(name + "bawl", 1, scene);
        bosquetbawl->position = Vector3(x, y + 1, z);
        bosquetbawl->material = grassMaterial;

        shadowGenerator->getShadowMap()->renderList().emplace_back(bosquet);
        shadowGenerator->getShadowMap()->renderList().emplace_back(bosquetbawl);
      };

  const auto CreateTree = [](const std::string& name, float x, float y, float z,
                             Scene* scene, ShadowGenerator* shadowGenerator,
                             const MaterialPtr& woodMaterial,
                             const MaterialPtr& grassMaterial) {
    auto trunk = Mesh::CreateCylinder(name + "trunk", 7, 2, 2, 12, 1, scene);
    trunk->position = Vector3(x, y, z);
    trunk->material = woodMaterial;

    auto leafs      = Mesh::CreateSphere(name + "leafs", 20, 7, scene);
    leafs->position = Vector3(x, y + 5.f, z);
    leafs->material = grassMaterial;

    shadowGenerator->getShadowMap()->renderList().emplace_back(trunk);
    shadowGenerator->getShadowMap()->renderList().emplace_back(leafs);
  };

  const auto CreateFontain
    = [](const std::string& name, float x, float y, float z, Scene* scene,
         ShadowGenerator* shadowGenerator, const MaterialPtr& marbleMaterial,
         const MaterialPtr& fireMaterial) {
        auto torus      = Mesh::CreateTorus(name + "torus", 5, 1, 20, scene);
        torus->position = Vector3(x, y, z);
        torus->material = marbleMaterial;

        auto fontainGround = Mesh::CreateBox(name + "fontainGround", 4, scene);
        fontainGround->position = Vector3(x, y - 2, z);
        fontainGround->material = marbleMaterial;

        auto fontainSculptur1 = Mesh::CreateCylinder(name + "fontainSculptur1",
                                                     2, 2, 1, 10, 1, scene);
        fontainSculptur1->position = Vector3(x, y, z);
        fontainSculptur1->material = marbleMaterial;

        Vector3 rotateAxis(1.f, 0.f, 0.f);
        auto fontainSculptur2
          = Mesh::CreateSphere(name + "fontainSculptur2", 7, 1.7f, scene);
        fontainSculptur2->position = Vector3(x, y + 0.9f, z);
        fontainSculptur2->material = fireMaterial;
        fontainSculptur2->rotate(rotateAxis, Math::PI_2, Space::LOCAL);

        shadowGenerator->getShadowMap()->renderList().emplace_back(torus);
        shadowGenerator->getShadowMap()->renderList().emplace_back(
          fontainSculptur1);
        shadowGenerator->getShadowMap()->renderList().emplace_back(
          fontainSculptur2);
      };

  const auto CreateTorch
    = [](const std::string& name, float x, float y, float z, Scene* scene,
         ShadowGenerator* shadowGenerator, const MaterialPtr& brickMaterial,
         const MaterialPtr& woodMaterial, const MaterialPtr& grassMaterial) {
        // createBrickBlock
        auto brickblock      = Mesh::CreateBox(name + "brickblock", 1, scene);
        brickblock->position = Vector3(x, y, z);
        brickblock->material = brickMaterial;

        // createWood
        auto torchwood = Mesh::CreateCylinder(name + "torchwood", 2, 0.25, 0.1f,
                                              12, 1, scene);
        torchwood->position = Vector3(x, y + 1, z);
        torchwood->material = woodMaterial;

        // leafs
        auto leafs2      = Mesh::CreateSphere(name + "leafs2", 10, 1.2f, scene);
        leafs2->position = Vector3(x, y + 2, z);
        leafs2->material = grassMaterial;

        shadowGenerator->getShadowMap()->renderList().emplace_back(torchwood);
        shadowGenerator->getShadowMap()->renderList().emplace_back(leafs2);
        shadowGenerator->getShadowMap()->renderList().emplace_back(brickblock);
      };

  // Ok, enough helpers, let the building start
  _camera = ArcRotateCamera::New("Camera", 1, 1.2f, 25, Vector3(10.f, 0.f, 0.f),
                                 scene);
  _camera->upperBetaLimit = 1.2f;
  _camera->attachControl(canvas, false);

  // Material declaration
  auto woodMaterial = StandardMaterial::New("woodMaterial", scene);
  auto woodTexture  = WoodProceduralTexture::New("woodTexture", 1024, scene);
  woodTexture->ampScale = 50.f;
  woodMaterial->setDiffuseTexture(woodTexture);

  auto grassMaterial = StandardMaterial::New("grassMaterial", scene);
  auto grassTexture  = GrassProceduralTexture::New("grassTexture", 256, scene);
  grassMaterial->setAmbientTexture(grassTexture);

  auto marbleMaterial = StandardMaterial::New("torus", scene);
  auto marbleTexture  = MarbleProceduralTexture::New("marble", 512, scene);
  marbleTexture->numberOfTilesWidth  = 5;
  marbleTexture->numberOfTilesHeight = 5;
  marbleMaterial->setAmbientTexture(marbleTexture);

  auto fireMaterial = StandardMaterial::New("fontainSculptur2", scene);
  auto fireTexture  = FireProceduralTexture::New("fire", 256, scene);
  fireMaterial->setDiffuseTexture(fireTexture);
  fireMaterial->setOpacityTexture(fireTexture);

  auto brickMaterial = StandardMaterial::New("brickMaterial", scene);
  auto brickTexture  = BrickProceduralTexture::New("brickTexture", 512, scene);
  brickTexture->numberOfBricksHeight = 2;
  brickTexture->numberOfBricksWidth  = 3;
  brickMaterial->setDiffuseTexture(brickTexture);

  // Light
  auto light = DirectionalLight::New("dir01", Vector3(-0.5, -1, -0.5), scene);
  light->diffuse  = Color3(1, 1, 1);
  light->specular = Color3(1, 1, 1);
  // light->groundColor = Color3(0, 0, 0);
  light->position = Vector3(20, 40, 20);

  // Create a square of grass using a custom procedural texture
  auto square         = Mesh::CreateGround("square", 20, 20, 2, scene);
  square->position    = Vector3(0, 0, 0);
  auto customMaterial = StandardMaterial::New("custommat", scene);
  auto customProcText = CustomProceduralTexture::New(
    "customtext", "textures/customProceduralTextures/land", 1024, scene);
  customMaterial->setAmbientTexture(customProcText);
  square->material = customMaterial;

  // Applying some shadows
  auto shadowGenerator   = new ShadowGenerator(1024, light);
  square->receiveShadows = true;

  // Creating 4 bosquets
  CreateBosquet("b1", -9.f, 1.f, 9.f, scene, shadowGenerator, grassMaterial);
  CreateBosquet("b2", -9.f, 1.f, -9.f, scene, shadowGenerator, grassMaterial);
  CreateBosquet("b3", 9.f, 1.f, 9.f, scene, shadowGenerator, grassMaterial);
  CreateBosquet("b4", 9.f, 1.f, -9.f, scene, shadowGenerator, grassMaterial);

  // Create tree
  CreateTree("a1", 0, 3.5f, 0, scene, shadowGenerator, woodMaterial,
             grassMaterial);

  // Creating macadam
  auto macadam               = Mesh::CreateGround("square", 20, 20, 2, scene);
  macadam->position          = Vector3(20, 0, 0);
  auto customMaterialmacadam = StandardMaterial::New("macadam", scene);
  auto customProcTextmacadam
    = RoadProceduralTexture::New("customtext", 512, scene);
  customMaterialmacadam->setDiffuseTexture(customProcTextmacadam);
  macadam->material       = customMaterialmacadam;
  macadam->receiveShadows = true;

  // Creating a fontain
  CreateFontain("fontain", 20, 0.25f, 0, scene, shadowGenerator, marbleMaterial,
                fireMaterial);
  CreateTorch("torch1", 15, 0.5, 5, scene, shadowGenerator, brickMaterial,
              woodMaterial, grassMaterial);
  CreateTorch("torch2", 15, 0.5f, -5, scene, shadowGenerator, brickMaterial,
              woodMaterial, grassMaterial);
  CreateTorch("torch3", 25, 0.5f, 5, scene, shadowGenerator, brickMaterial,
              woodMaterial, grassMaterial);
  CreateTorch("torch4", 25, 0.5f, -5, scene, shadowGenerator, brickMaterial,
              woodMaterial, grassMaterial);

  // Using a procedural texture to create the sky
  auto boxCloud      = Mesh::CreateSphere("boxCloud", 100, 1000, scene);
  boxCloud->position = Vector3(0.f, 0.f, 12.f);
  auto cloudMaterial = StandardMaterial::New("cloudMat", scene);
  auto cloudProcText = CloudProceduralTexture::New("cloud", 1024, scene);
  cloudMaterial->setEmissiveTexture(cloudProcText);
  cloudMaterial->backFaceCulling = false;
  cloudMaterial->emissiveTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  boxCloud->material = cloudMaterial;

  scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
    _camera->alpha += 0.001f * scene->getAnimationRatio();
  });
#endif
}

} // namespace Samples
} // end of namespace BABYLON
