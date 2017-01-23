#ifndef BABYLON_TOOLS_OPTIMIZATION_SHADOWS_OPTIMIZATION_H
#define BABYLON_TOOLS_OPTIMIZATION_SHADOWS_OPTIMIZATION_H

#include <babylon/babylon_global.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowsOptimization : public SceneOptimization {

public:
  ShadowsOptimization(int priority = 0);
  ~ShadowsOptimization();

  bool apply(Scene* scene) override;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_SHADOWS_OPTIMIZATION_H
