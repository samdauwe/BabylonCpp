#include <babylon/samples/animations/bone_rotatation_in_world_space_scene.h>

#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

BoneRotationInWorldSpaceScene::BoneRotationInWorldSpaceScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _rotationAxis{Axis::Z()}
    , _mesh{nullptr}
    , _skeleton{nullptr}
{
}

BoneRotationInWorldSpaceScene::~BoneRotationInWorldSpaceScene()
{
}

const char* BoneRotationInWorldSpaceScene::getName()
{
  return "Bone Rotation In World Space Scene";
}

void BoneRotationInWorldSpaceScene::initializeScene(ICanvas* canvas,
                                                    Scene* scene)
{
  // Adding a light
  auto light = PointLight::New("Omni", Vector3(10.f, 0.f, 10.f), scene);

  // Adding an Arc Rotate Camera
  auto camera = ArcRotateCamera::New("Camera", Math::PI_2, Math::PI_2, 10.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  // The first parameter can be used to specify which mesh to import. Here we
  // import all meshes
  SceneLoader::ImportMesh(
    {}, "scenes/", "boneRotation.babylon", scene,
    [this](const std::vector<AbstractMeshPtr>& meshes,
           const std::vector<IParticleSystemPtr>& /*particleSystems*/,
           const std::vector<SkeletonPtr>& skeletons,
           const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      if (!meshes.empty() && !skeletons.empty()) {
        // Update mesh rotation
        _mesh               = meshes[0];
        _mesh->rotation().x = Math::PI * 0.25f;
        // Bone rotate animations
        _skeleton = skeletons[0];
        _scene->registerBeforeRender(
          [this](Scene* /*scene*/, EventState& /*es*/) {
            _skeleton->bones[0]->rotate(_rotationAxis, 0.01f, Space::WORLD,
                                        _mesh.get());
            _skeleton->bones[1]->rotate(_rotationAxis, 0.01f, Space::WORLD,
                                        _mesh.get());
            _skeleton->bones[2]->rotate(_rotationAxis, 0.01f, Space::WORLD,
                                        _mesh.get());
          });
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
