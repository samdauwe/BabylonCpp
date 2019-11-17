#ifndef BABYLON_MISC_OPTIMIZATION_POST_PROCESS_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_POST_PROCESS_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PostProcessesOptimization : public SceneOptimization {

public:
  PostProcessesOptimization(int priority = 0);
  ~PostProcessesOptimization() override; // = default

  bool apply(Scene* scene) override;

}; // end of class PostProcessesOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_POST_PROCESS_OPTIMIZATION_H
