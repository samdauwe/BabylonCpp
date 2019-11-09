#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class LorenzAttractorScene : public IRenderableScene {

public:
  LorenzAttractorScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~LorenzAttractorScene()
  {
  }

  const char* getName() override
  {
    return "Lorenz Attractor Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera
      = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(20.f, 200.f, 400.f));
    camera->maxZ             = 20000.f;
    camera->lowerRadiusLimit = 150.f;
  
    // Attach the camera to the canvas
    camera->attachControl(canvas, true);
  
    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light       = HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);
    light->intensity = 1.f;
  
    // Basic Lorenz Attractor generator code
    auto generateLorenzAttractor = [](unsigned int lorenzIterationCount) {
      std::vector<Vector3> points(lorenzIterationCount);
      float o    = 15.f;
      float x    = 0.1f;
      float y    = 0.f;
      float z    = 0.f;
      float a    = 10.f;
      float b    = 28.f;
      float c    = 8.f / 3.f;
      float t    = 0.01f;
      float zoom = 4.f;
      // Iterate and update x,y and z locations
      // based upon the Lorenz equations
      for (unsigned int i = 0; i < lorenzIterationCount; ++i) {
        float xt  = x + t * a * (y - x);
        float yt  = y + t * (x * (b - z) - y);
        float zt  = z + t * (x * y - c * z);
        points[i] = Vector3(xt * zoom, yt * zoom, (zt - o) * zoom);
        x         = xt;
        y         = yt;
        z         = zt;
      }
      return points;
    };
  
    // Create a Lorenz Attractor
    auto points = generateLorenzAttractor(20000);
  
    // Creation of a lines mesh
    lorenz        = Mesh::CreateLines("LorenzAttractor", points, scene);
    lorenz->color = Color3::Red();
  
    scene->registerBeforeRender([this](Scene*, EventState&) {
      lorenz->rotation().y += 0.008f * getScene()->getAnimationRatio();
    });
  }

private:
  LinesMeshPtr lorenz;

}; // end of class LorenzAttractorScene


BABYLON_REGISTER_SAMPLE("Meshes", LorenzAttractorScene)
} // end of namespace Samples
} // end of namespace BABYLON