#include <babylon/samples/meshes/rotation_and_scaling_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

RotationAndScalingScene::RotationAndScalingScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

RotationAndScalingScene::~RotationAndScalingScene()
{
}

const char* RotationAndScalingScene::getName()
{
  return "Rotation and Scaling Scene";
}

void RotationAndScalingScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("Camera", Math::PI, Math::PI / 8.f, 150.f,
                                     Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light      = HemisphericLight::New("Hemi", Vector3(0, 1, 0), scene);
  light->diffuse  = Color3::FromInt(0xf68712);
  light->specular = Color3::FromInt(0xf1471d);

  // Creation of boxes
  auto box1 = Mesh::CreateBox("Box1", 6.f, scene);
  auto box2 = Mesh::CreateBox("Box2", 6.f, scene);
  auto box3 = Mesh::CreateBox("Box3", 6.f, scene);
  auto box4 = Mesh::CreateBox("Box4", 6.f, scene);
  auto box5 = Mesh::CreateBox("Box5", 6.f, scene);
  auto box6 = Mesh::CreateBox("Box6", 6.f, scene);
  auto box7 = Mesh::CreateBox("Box7", 6.f, scene);

  // Moving boxes on the x axis
  box1->position().x = -20.f;
  box2->position().x = -10.f;
  box3->position().x = 0.f;
  box4->position().x = 15.f;
  box5->position().x = 30.f;
  box6->position().x = 45.f;

  // Rotate box around the x axis
  box1->rotation().x = Math::PI / 6.f;

  // Rotate box around the y axis
  box2->rotation().y = Math::PI / 3.f;

  // Scaling on the x axis
  box4->scaling().x = 2.f;

  // Scaling on the y axis
  box5->scaling().y = 2.f;

  // Scaling on the z axis
  box6->scaling().z = 2.f;

  // Moving box7 relatively to box1
  box7->setParent(box1.get());
  box7->position().z = -10.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
