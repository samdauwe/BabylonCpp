#include <babylon/samples/textures/cube_texture_different_faces_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

CubeTextureDifferentFacesScene::CubeTextureDifferentFacesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CubeTextureDifferentFacesScene::~CubeTextureDifferentFacesScene()
{
}

const char* CubeTextureDifferentFacesScene::getName()
{
  return "Cube texture different faces scene";
}

void CubeTextureDifferentFacesScene::initializeScene(ICanvas* canvas,
                                                     Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light     = HemisphericLight::New("hemiLight", Vector3(-1, 1, 0), scene);
  light->diffuse = Color3(1.f, 0.f, 0.f);

  // Box with different textures on each face.
  BoxOptions boxOptions;
  auto box         = MeshBuilder::CreateBox("Box", boxOptions, scene);
  auto boxMaterial = StandardMaterial::New("mat", scene);
  boxMaterial->backFaceCulling = true;
  auto reflectionTexture = CubeTexture::New("textures/skybox/cubeSide", scene);
  reflectionTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  boxMaterial->reflectionTexture     = reflectionTexture;
  boxMaterial->diffuseColor          = Color3(0, 0, 0);
  boxMaterial->specularColor         = Color3(0, 0, 0);
  box->material                      = boxMaterial;

  box->rotation().y = Math::PI / 8.f;
  box->rotation().x = -Math::PI / 8.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
