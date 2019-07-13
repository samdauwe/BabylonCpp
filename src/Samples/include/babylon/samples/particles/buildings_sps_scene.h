#ifndef BABYLON_PARTICLES_BUILDINGS_SPS_SCENE_H
#define BABYLON_PARTICLES_BUILDINGS_SPS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class PointLight;
class SolidParticle;
class SolidParticleSystem;
using ArcRotateCameraPtr     = std::shared_ptr<ArcRotateCamera>;
using MeshPtr                = std::shared_ptr<Mesh>;
using PointLightPtr          = std::shared_ptr<PointLight>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

namespace Samples {

/**
 * @brief Buldings SPS Scene. A lot of buildings with SPS. Example
 * demonstrating how to use solid particle system to create a city.
 * @see https://playground.babylonjs.com/#2FPT1A#56
 * @see https://doc.babylonjs.com/how_to/solid_particle_system
 */
class BuildingsSPSScene : public IRenderableScene {

public:
  BuildingsSPSScene(ICanvas* iCanvas);
  ~BuildingsSPSScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief Custom position function for SPS creation.
   */
  void _positionFunction(SolidParticle* particle, unsigned int i,
                         unsigned int s);

private:
  float _k;
  float _fact; // density
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;
  MeshPtr _ground;
  SolidParticleSystemPtr _sps;

}; // end of class BuildingsSPSScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_BUILDINGS_SPS_SCENE_H
