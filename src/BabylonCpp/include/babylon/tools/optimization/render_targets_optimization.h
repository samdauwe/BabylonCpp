#ifndef BABYLON_TOOLS_OPTIMIZATION_RENDER_TARGETS_OPTIMIZATION_H
#define BABYLON_TOOLS_OPTIMIZATION_RENDER_TARGETS_OPTIMIZATION_H

#include <babylon/babylon_global.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RenderTargetsOptimization
  : public SceneOptimization {

public:
  RenderTargetsOptimization(int priority = 0);
  ~RenderTargetsOptimization();

  bool apply(Scene* scene) override;

}; // end of class RenderTargetsOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_RENDER_TARGETS_OPTIMIZATION_H