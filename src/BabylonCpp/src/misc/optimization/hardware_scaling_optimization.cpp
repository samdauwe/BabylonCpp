#include <babylon/misc/optimization/hardware_scaling_optimization.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

HardwareScalingOptimization::HardwareScalingOptimization(int iPriority,
                                                         int iMaximumSize)
    : SceneOptimization{iPriority}, maximumScale{iMaximumSize}, _currentScale{1}
{
}

HardwareScalingOptimization::~HardwareScalingOptimization() = default;

bool HardwareScalingOptimization::apply(Scene* scene)
{
  ++_currentScale;

  scene->getEngine()->setHardwareScalingLevel(_currentScale);

  return _currentScale >= maximumScale;
}

} // end of namespace BABYLON
