#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Mirror Texture Scene. Example demonstrating that MirrorTexture objects within the scene
 * can be reflected as in a mirror.
 * @see http://www.babylonjs-playground.com/#1YAIO7#5
 */
struct MirrorTextureScene : public IRenderableScene {

  MirrorTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MirrorTextureScene() override = default;

  const char* getName() override
  {
    return "Mirror Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", -7 * Math::PI / 16, 4 * Math::PI / 8, 20.f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    auto light       = HemisphericLight::New("hemi", Vector3(0.f, 10.f, -5.f), scene);
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
      auto glass          = MeshBuilder::CreatePlane("glass", planeoptions, scene);
      glass->position     = Vector3(((i < 2.f) - 0.5f) * 12.f * ((i % 2) == 1), 0.f,
                                ((i < 2) - 0.5f) * 12.f * ((i % 2) == 0));
      glass->rotation     = Vector3(0.f, i * Math::PI_2, 0.f);

      // Ensure working with new values for glass by computing and obtaining its
      // worldMatrix
      glass->computeWorldMatrix(true);
      auto glass_worldMatrix = glass->getWorldMatrix();

      // Obtain normals for plane and assign one of them as the normal
      auto glass_vertexData = glass->getVerticesData(VertexBuffer::NormalKind);
      auto glassNormal = Vector3(glass_vertexData[0], glass_vertexData[1], glass_vertexData[2]);
      // Use worldMatrix to transform normal into its current value
      glassNormal = Vector3::TransformNormal(glassNormal, glass_worldMatrix);

      // Create reflecting surface for mirror surface
      auto reflector = Plane::FromPositionAndNormal(glass->position, glassNormal.scale(-1.f));

      // Create the mirror material
      auto mirrorMaterial               = StandardMaterial::New("mirror", scene);
      auto reflectionTexture            = MirrorTexture::New("mirror", 1024.f, scene, true);
      reflectionTexture->mirrorPlane    = reflector;
      reflectionTexture->renderList     = {sphere.get()};
      reflectionTexture->level          = 1;
      mirrorMaterial->reflectionTexture = reflectionTexture;

      glass->material = mirrorMaterial;
    }
  }

}; // end of struct MirrorTextureScene

BABYLON_REGISTER_SAMPLE("Textures", MirrorTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
