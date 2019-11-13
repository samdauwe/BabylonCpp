#ifndef BABYLON_MISC_OPTIMIZATION_PARTICLES_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_PARTICLES_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ParticlesOptimization : public SceneOptimization {

public:
  ParticlesOptimization(int priority = 0);
  ~ParticlesOptimization(); // = default

  bool apply(Scene* scene) override;

}; // end of class ParticlesOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_PARTICLES_OPTIMIZATION_H
