#ifndef BABYLON_PARTICLES_LIGHTED_SPS_SCENE_H
#define BABYLON_PARTICLES_LIGHTED_SPS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class PointLight;
class SolidParticle;
class SolidParticleSystem;
using PointLightPtr          = std::shared_ptr<PointLight>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

namespace Samples {

/**
 * @brief Lighted SPS Scene. Solid particle system with a light going through
 * it.
 * @see https://playground.babylonjs.com/#2FPT1A#183
 * @see https://doc.babylonjs.com/how_to/solid_particle_system
 */
class LightedSPSScene : public IRenderableScene {

public:
  LightedSPSScene(ICanvas* iCanvas);
  ~LightedSPSScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief Custom position function for SPS creation.
   */
  void _positionFunction(SolidParticle* particle, unsigned int i,
                         unsigned int s);

private:
  float _time;
  size_t _nb;   // nb of triangles
  size_t _fact; // cube size
  PointLightPtr _pl;
  SolidParticleSystemPtr _sps;

}; // end of struct LightedSPSScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_LIGHTED_SPS_SCENE_H
