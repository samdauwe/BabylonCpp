#ifndef BABYLON_PARTICLES_TRIANGLES_SPS_SCENE_H
#define BABYLON_PARTICLES_TRIANGLES_SPS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
class SolidParticle;
class SolidParticleSystem;
using ArcRotateCameraPtr     = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr          = std::shared_ptr<PointLight>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

namespace Samples {

/**
 * @brief Triangles SPS Scene. A lot of triangles with SPS. Example
 * demonstrating how to use solid particle system to create a colorful cube.
 * @see https://www.babylonjs-playground.com/#2FPT1A#5
 * @see https://doc.babylonjs.com/how_to/solid_particle_system
 */
class TrianglesSPSScene : public IRenderableScene {

public:
  TrianglesSPSScene(ICanvas* iCanvas);
  ~TrianglesSPSScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief Custom position function for SPS creation.
   */
  void _positionFunction(SolidParticle* particle, unsigned int i,
                         unsigned int s);

private:
  size_t _nb;   // nb of triangles
  size_t _fact; // cube size
  bool _enableHemisphericLight;
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;
  SolidParticleSystemPtr _sps;

}; // end of struct TrianglesSPSScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_TRIANGLES_SPS_SCENE_H
