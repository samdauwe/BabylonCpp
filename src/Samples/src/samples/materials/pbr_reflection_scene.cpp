#include <babylon/samples/materials/pbr_reflection_scene.h>

#include <babylon/animations/animation.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

PBRReflectionScene::PBRReflectionScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PBRReflectionScene::~PBRReflectionScene()
{
}

const char* PBRReflectionScene::getName()
{
  return "PBR Reflection Scene";
}

void PBRReflectionScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color4(0.f, 0.f, 0.f, 1.f);
  scene->autoClear  = true;

  //
  // gold mat
  //
  auto mat0       = PBRMetallicRoughnessMaterial::New("mat0", scene);
  mat0->metallic  = 1.f;
  mat0->roughness = 0.2f;
  mat0->baseColor = Color3(0.9f, 0.2f, 0.f);

  //
  // torusknot
  //
  TorusKnotOptions torusKnotOptions;
  torusKnotOptions.p               = 1.f;
  torusKnotOptions.q               = 2.f;
  torusKnotOptions.radius          = 2.f;
  torusKnotOptions.radialSegments  = 300;
  torusKnotOptions.tubularSegments = 100;
  auto mesh0 = MeshBuilder::CreateTorusKnot("mesh0", torusKnotOptions, scene);
  mesh0->material     = mat0;
  mesh0->position     = Vector3(0.f, 5.f, -5.f);
  mesh0->rotation().x = Math::PI_4;

  //
  // plate
  //
  CylinderOptions cylinderOptions;
  cylinderOptions.diameterTop    = 6.f;
  cylinderOptions.diameterBottom = 2.f;
  cylinderOptions.height         = 2.f;
  cylinderOptions.tessellation   = 100;
  auto mesh1 = MeshBuilder::CreateCylinder("mesh1", cylinderOptions, scene);
  mesh1->material = mat0;
  mesh1->position = Vector3(0.f, 1.f, -5.f);

  //
  // ground
  //
  GroundOptions groundOptions;
  groundOptions.width  = 1000;
  groundOptions.height = 1000;
  auto ground = MeshBuilder::CreateGround("ground", groundOptions, scene);
  auto mat1   = PBRMaterial::New("mat1", scene);
  auto mat1ReflectionTexture
    = MirrorTexture::New("tex1", static_cast<float>(std::pow(2, 10)), scene, true);

  mat1ReflectionTexture->mirrorPlane = Plane(0.f, -1.f, 0.f, 0.f);
  // mirrorplane normal is pointing towards "mirror image"
  //
  //   O       <--mirror faces to human |
  //  /|\                               |--> mirrorplane normal
  //   |\                               |
  //
  mat1ReflectionTexture->level = 0.5f;
  mat1->reflectionTexture      = mat1ReflectionTexture;
  mat1->reflectivityColor      = Color3(0.2f, 0.2f, 0.2f);
  mat1->albedoColor            = Color3(0.f, 0.2f, 0.9f);
  ground->material             = mat1;

  //
  // mirror
  //
  PlaneOptions planeOptions;
  planeOptions.width           = 20.f;
  planeOptions.height          = 20.f;
  planeOptions.sideOrientation = Mesh::DOUBLESIDE;
  auto plane0 = MeshBuilder::CreatePlane("plane0", planeOptions, scene);
  plane0->rotation().x                   = Math::PI / 180.f * -15.f;
  PBRMaterialPtr mat2                    = nullptr;
  MirrorTexturePtr mat2ReflectionTexture = nullptr;
  {
    // transform normal {
    plane0->computeWorldMatrix(true); // <------- == apply transformation NOW
    auto planeVertices = plane0->getVerticesData(
      VertexBuffer::NormalKind); // [24] 8vertices" normal data
    auto nx = planeVertices[0], ny = planeVertices[1], nz = planeVertices[2];
    auto normal = Vector3(nx, ny, nz); // (?must be)  v3(0 0 -1)
    auto mtx    = plane0->getWorldMatrix();
    auto newNormal
      = Vector3::TransformNormal(normal, mtx); // return new normal (length=1)
    // }

    mat2 = PBRMaterial::New("mat2", scene);
    mat2ReflectionTexture
      = MirrorTexture::New("tex2", static_cast<float>(std::pow(2, 10)), scene, true);
    mat2ReflectionTexture->renderList = {mesh0.get(), ground.get()};
    mat2ReflectionTexture->mirrorPlane
      = Plane::FromPositionAndNormal(plane0->position(), newNormal.negate());
    mat2ReflectionTexture->adaptiveBlurKernel = 32.f; // blur
    mat2ReflectionTexture->level              = 0.9f; // intensity of texture
    mat2->reflectionTexture                   = mat2ReflectionTexture;
    mat2->reflectivityColor                   = Color3(0.9f, 0.9f, 0.9f); // (?)
    mat2->albedoColor                         = Color3(0.f, 0.2f, 0.9f);  // (?)
    plane0->material                          = mat2;
  }

  //
  // optin meshes for reflectionTexture(s)
  //
  mat1ReflectionTexture->renderList
    = {plane0.get(), mesh0.get(), mesh1.get()}; // ground mat
  mat2ReflectionTexture->renderList
    = {mesh0.get(), mesh1.get(), ground.get()}; // mirror mat

  //
  // lights and shadow
  //
  auto light0
    = SpotLight::New("light0", Vector3(0.f, 20.f, -20.f), Vector3::Zero(),
                     Math::PI / 180.f * 130.f, 1.f, scene);
  light0->direction = Vector3::Normalize(light0->position().negate());
  light0->intensity = 40000.f;
  light0->diffuse   = Color3(0.3f, 0.3f, 0.3f);

  ground->receiveShadows           = true;
  plane0->receiveShadows           = true;
  mesh0->receiveShadows            = true;
  mesh1->receiveShadows            = true;
  auto sg                          = ShadowGenerator::New(static_cast<int>(pow(2, 11)), light0);
  sg->usePercentageCloserFiltering = true;
  sg->filteringQuality             = ShadowGenerator::QUALITY_HIGH();
  sg->addShadowCaster(plane0);
  sg->addShadowCaster(mesh0);
  sg->addShadowCaster(mesh1);

  //
  // camera
  //
  auto camera0 = ArcRotateCamera::New("camera0", Math::PI / 180.f * -135.f,
                                      Math::PI / 180.f * 60.f, 25.f,
                                      Vector3(0.f, 2.f, 0.f), scene);
  camera0->attachControl(canvas);
  camera0->lowerRadiusLimit = 20.f;
  camera0->upperRadiusLimit = 40.f;
  camera0->lowerBetaLimit   = Math::PI / 180.f * 60.f;
  camera0->upperBetaLimit   = Math::PI / 180.f * 92.f;
  camera0->lowerAlphaLimit  = Math::PI / 180.f * -165.f;
  camera0->upperAlphaLimit  = Math::PI / 180.f * -15.f;

  //
  // animations
  //
  Animation::CreateAndStartAnimation("anim0", mesh0, "rotation.y", 5, 30.f, 0.f,
                                     Math::PI2,
                                     Animation::ANIMATIONLOOPMODE_CYCLE());
}

} // end of namespace Samples
} // end of namespace BABYLON
