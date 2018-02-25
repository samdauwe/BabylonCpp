#include <babylon/samples/materials/fresnel_parameters_torus_knot_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

FresnelParametersTorusKnotScene::FresnelParametersTorusKnotScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

FresnelParametersTorusKnotScene::~FresnelParametersTorusKnotScene()
{
}

const char* FresnelParametersTorusKnotScene::getName()
{
  return "Fresnel Parameters TorusKnot Scene";
}

void FresnelParametersTorusKnotScene::initializeScene(ICanvas* canvas,
                                                      Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  auto material = StandardMaterial::New("kosh", scene);
  auto torusKnot
    = Mesh::CreateTorusKnot("knot", 2.f, 0.5f, 128, 64, 2.f, 3.f, scene);
  auto light = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);
  light->intensity = 0.9f;

  camera->setPosition(Vector3(-10, 3, 0));
  camera->attachControl(canvas, true);

  // Set the TorusKnot material
  material->diffuseColor  = Color3(0, 0, 0);
  material->emissiveColor = Color3(1, 1, 1);
  material->specularPower = 128;
  material->setAlpha(0.2f);
  material->setAlphaMode(EngineConstants::ALPHA_PREMULTIPLIED);
  torusKnot->setMaterial(material);

  // Set opacity fresnel parameters
  auto& opacityFresnelParameters      = *material->opacityFresnelParameters();
  opacityFresnelParameters.power      = 0.8f;
  opacityFresnelParameters.leftColor  = Color3::White();
  opacityFresnelParameters.rightColor = Color3::Black();

  // Set emissive fresnel parameters
  auto& emissiveFresnelParameters      = *material->emissiveFresnelParameters();
  emissiveFresnelParameters.power      = 0.5f;
  emissiveFresnelParameters.leftColor  = Color3(0.6f, 0.8f, 0.9f);
  emissiveFresnelParameters.rightColor = Color3::Teal().scale(0.5f);

  // Mark fresnel parameters a dirty
  material->markAsDirty(Material::FresnelDirtyFlag());
}

} // end of namespace Samples
} // end of namespace BABYLON
