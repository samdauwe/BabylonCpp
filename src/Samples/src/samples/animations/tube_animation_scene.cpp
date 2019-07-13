#include <babylon/samples/animations/tube_animation_scene.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/quaternion.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

TubeAnimationScene::TubeAnimationScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

TubeAnimationScene::~TubeAnimationScene()
{
}

const char* TubeAnimationScene::getName()
{
  return "Tube Animation Scene";
}

void TubeAnimationScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change clear color of the scene
  scene->clearColor = Color4(0.5f, 0.5f, 0.5f);

  // Create the camera
  auto camera = FreeCamera::New("FreeCam", Vector3(0.f, 5.f, -60.f), scene);
  camera->position = Vector3(0.f, 0.f, -70.f);
  camera->attachControl(canvas, true);

  // Create a light
  auto light       = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  light->intensity = 0.7f;

  // Create a tube
  auto mat             = StandardMaterial::New("mat1", scene);
  mat->alpha           = 1.f;
  mat->diffuseColor    = Color3(0.5f, 0.5f, 1.0f);
  mat->backFaceCulling = false;
  mat->wireframe       = true;

  const auto curvePoints = [](float l, float t) {
    std::vector<Vector3> path;
    float step = l / t;
    for (float i = -l / 2.f; i < l / 2.f; i += step) {
      path.emplace_back(Vector3(5.f * std::sin(i * t / 400.f), i,
                                5.f * std::cos(i * t / 400.f)));
    }
    return path;
  };

  auto curve = curvePoints(40.f, 200.f);

  // Create tube
  auto tube = Mesh::CreateTube("tube", curve, 5, 60, nullptr, 0, scene, false,
                               Mesh::FRONTSIDE);
  tube->material = mat;

  auto mat1          = StandardMaterial::New("mat1", scene);
  mat1->alpha        = 1.f;
  mat1->diffuseColor = Color3(0.5f, 0.5f, 1.f);

  BoxOptions options;
  options.size             = 2.f;
  options.faceColors[3]    = Color4(1.f, 0.f, 0.f, 1.f); // Red
  auto cube                = MeshBuilder::CreateBox("box", options, scene);
  cube->rotationQuaternion = Quaternion::Identity();

  // Animation on position
  auto cubeAnimation = Animation::New("myAnimation", "position", 30,
                                      Animation::ANIMATIONTYPE_VECTOR3(),
                                      Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  std::vector<IAnimationKey> keys;
  float frame = 0.f;
  for (const auto& point : curve) {
    keys.emplace_back(IAnimationKey(frame, AnimationValue(point)));
    ++frame;
  }

  cubeAnimation->setKeys(keys);
  cube->animations.emplace_back(cubeAnimation);

  // Animation on rotation
  auto cubeRotationAnimation
    = Animation::New("rotateAnimation", "rotationQuaternion", 30,
                     Animation::ANIMATIONTYPE_QUATERNION(),
                     Animation::ANIMATIONLOOPMODE_CYCLE());

  // Animation keys
  std::vector<IAnimationKey> rotationKeys;
  frame = 0;
  for (unsigned int index = 0; index < curve.size() - 1; ++index) {
    auto pointToRotateTo = curve[index + 1];
    auto axis1           = curve[index].subtract(pointToRotateTo).normalize();
    auto axis2 = Vector3::Cross(axis1, Vector3(0.f, 1.f, 0.f)).normalize();
    auto axis3 = Vector3::Cross(axis1, axis2).normalize();

    auto matrix = Matrix::Identity();

    matrix.setAtIndex(0, axis1.x);
    matrix.setAtIndex(1, axis1.y);
    matrix.setAtIndex(2, axis1.z);

    matrix.setAtIndex(4, axis2.x);
    matrix.setAtIndex(5, axis2.y);
    matrix.setAtIndex(6, axis2.z);

    matrix.setAtIndex(8, axis3.x);
    matrix.setAtIndex(9, axis3.y);
    matrix.setAtIndex(10, axis3.z);

    auto rotation = Quaternion::FromRotationMatrix(matrix);
    rotationKeys.emplace_back(IAnimationKey(frame, AnimationValue(rotation)));
    ++frame;
  }

  cubeRotationAnimation->setKeys(rotationKeys);
  cube->animations.emplace_back(cubeRotationAnimation);

  scene->beginAnimation(cube, 0.f, frame - 2.f, true);
}

} // end of namespace Samples
} // end of namespace BABYLON
