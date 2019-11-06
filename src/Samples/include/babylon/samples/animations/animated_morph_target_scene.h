#ifndef BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/morph/morph_target_manager.h>


namespace BABYLON {

class MorphTarget;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Samples {

/**
 * @brief Animated Morph Target Scene. Example demonstrating how to morph a mesh
 * between multiple targets.
 * @see https://www.babylonjs-playground.com/#HPV2TZ#8
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class AnimatedMorphTargetScene : public IRenderableScene {

public:
  AnimatedMorphTargetScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _angle{0.f}, _target0{nullptr}
  {
  }
  
   ~AnimatedMorphTargetScene() override
  {
  }
  
  const char* getName() override
  {
    return "Animated Morph Target Scene";
  }
  
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    auto camera = ArcRotateCamera::New("camera1", 1.14f, 1.13f, 10.f, Vector3::Zero(), scene);

    // This targets the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // This attaches the camera to the canvas
    camera->attachControl(canvas, true);

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    // Our built-in 'sphere' shape. Params: name, subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene, true);

    auto materialSphere            = StandardMaterial::New("mat", scene);
    materialSphere->diffuseTexture = Texture::New("textures/misc.jpg", scene);

    sphere->material = materialSphere;

    auto sphere2 = Mesh::CreateSphere("sphere2", 16, 2, scene);
    sphere2->setEnabled(false);
    sphere2->updateMeshPositions([this](Float32Array& data) -> void { _scrambleUp(data); });

    auto manager               = MorphTargetManager::New(scene);
    sphere->morphTargetManager = manager;

    _target0 = MorphTarget::FromMesh(sphere2, "sphere2", 0.25);
    manager->addTarget(_target0);

    scene->registerBeforeRender([this](Scene*, EventState&) {
      _target0->influence = std::sin(_angle) * std::sin(_angle);
      _angle += 0.01f;
    });
  }

private:
  void _scrambleUp(Float32Array& data)
  {
    for (size_t index = 0; index < data.size(); ++index) {
      data[index] += 0.4f * Math::random();
    }
  }
  
  void _scrambleDown(Float32Array& data)
  {
    for (size_t index = 0; index < data.size(); ++index) {
      data[index] -= 0.4f * Math::random();
    }
  }

private:
  float _angle;
  MorphTargetPtr _target0;

}; // end of class AnimatedMorphTargetScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H
