#ifndef BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT HardwareScalingOptimization : public SceneOptimization {

public:
  HardwareScalingOptimization(int priority = 0, int maximumSize = 2);
  ~HardwareScalingOptimization() override; // = default

  bool apply(Scene* scene) override;

public:
  int maximumScale;

private:
  int _currentScale;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H
