#ifndef BABYLON_MISC_OPTIMIZATION_SHADOWS_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_SHADOWS_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowsOptimization : public SceneOptimization {

public:
  ShadowsOptimization(int priority = 0);
  ~ShadowsOptimization(); // = default

  bool apply(Scene* scene) override;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SHADOWS_OPTIMIZATION_H
