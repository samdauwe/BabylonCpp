#include "HelloScene.h"

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>

#include <babylon/engines/scene.h>
#include <babylon/maths/color4.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>

#include <iostream>
#include <stdexcept>

namespace Samples {
using namespace BABYLON;

// This files demonstrates how to create a very simple renderable scene
struct HelloScene : public IRenderableScene
{
public:
  HelloScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }
  
  const char* getName() override { return "Hello Scene"; }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor = Color4(0.5f, 0.f, 0.f, 1.f);
    
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(0.f, 5.f, -10.f));
    camera->attachControl(canvas, true);

    camera->upperBetaLimit = Math::PI_2;
    camera->lowerRadiusLimit = 4.f;

    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.7f;

    auto knot = Mesh::CreateTorusKnot("knot", 1, 0.4f, 128, 64, 2, 3, scene);

    // Mirror
    auto mirror = Mesh::CreateBox("Mirror", 1.0, scene);
    mirror->scaling = Vector3(100.f, 0.01f, 100.f);
    auto mirrorMaterial = StandardMaterial::New("mirror", scene);
    auto reflectionTexture = MirrorTexture::New("mirror", 1024.f, scene, true);
    reflectionTexture->mirrorPlane = Plane(0.f, -1.f, 0.f, -2.f);
    reflectionTexture->renderList = {knot.get()};
    reflectionTexture->level = 1.f;
    reflectionTexture->samples = 16;
    mirrorMaterial->reflectionTexture = reflectionTexture;
    mirror->material = mirrorMaterial;
    mirror->position = Vector3(0.f, -2.f, 0.f);

    // Main material
    auto mainMaterial = StandardMaterial::New("main", scene);
    knot->material = mainMaterial;
    mainMaterial->diffuseColor = Color4(0.2f, 0.4f, 0.8f, 1.f);

    // Fog
    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogColor = scene->clearColor;
    scene->fogStart = 20.f;
    scene->fogEnd   = 50.f;
  }
};

std::shared_ptr<IRenderableScene> MakeHelloScene(ICanvas* canvas)
{
  return std::make_shared<HelloScene>(canvas);
}

} // end of namespace Samples
