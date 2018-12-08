#include <babylon/samples/textures/mirror_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

MirrorTextureScene::MirrorTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

MirrorTextureScene::~MirrorTextureScene()
{
}

const char* MirrorTextureScene::getName()
{
  return "Mirror Texture Scene";
}

void MirrorTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New(
    "Camera", -7 * Math::PI / 16, 4 * Math::PI / 8, 20, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("hemi", Vector3(0.f, 10.f, -5.f), scene);
  light->intensity = 1.f;

  // Objects to reflect
  SphereOptions sphereOptions;
  auto sphere = MeshBuilder::CreateSphere("Sphere", sphereOptions, scene);

  auto redMaterial          = StandardMaterial::New("red", scene);
  redMaterial->diffuseColor = Color3::Red();

  sphere->material = redMaterial;

  // Creation of a glass planes
  for (unsigned int i = 0; i < 4; ++i) {
    PlaneOptions planeoptions;
    planeoptions.width  = 5.f;
    planeoptions.height = 5.f;
    auto glass      = MeshBuilder::CreatePlane("glass", planeoptions, scene);
    glass->position = Vector3(((i < 2.f) - 0.5f) * 12.f * ((i % 2) == 1), 0.f,
                              ((i < 2) - 0.5f) * 12.f * ((i % 2) == 0));
    glass->rotation = Vector3(0.f, i * Math::PI_2, 0.f);

    // Ensure working with new values for glass by computing and obtaining its
    // worldMatrix
    glass->computeWorldMatrix(true);
    auto glass_worldMatrix = glass->getWorldMatrix();

    // Obtain normals for plane and assign one of them as the normal
    auto glass_vertexData = glass->getVerticesData(VertexBuffer::NormalKind);
    auto glassNormal
      = Vector3(glass_vertexData[0], glass_vertexData[1], glass_vertexData[2]);
    // Use worldMatrix to transform normal into its current value
    glassNormal = Vector3::TransformNormal(glassNormal, glass_worldMatrix);

    // Create reflecting surface for mirror surface
    auto reflector
      = Plane::FromPositionAndNormal(glass->position, glassNormal.scale(-1.f));

    // Create the mirror material
    auto mirrorMaterial    = StandardMaterial::New("mirror", scene);
    auto reflectionTexture = MirrorTexture::New("mirror", 1024, scene, true);
    reflectionTexture->mirrorPlane    = reflector;
    reflectionTexture->renderList     = {sphere};
    reflectionTexture->level          = 1;
    mirrorMaterial->reflectionTexture = reflectionTexture;

    glass->material = mirrorMaterial;
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
