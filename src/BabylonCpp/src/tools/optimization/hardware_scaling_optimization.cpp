#include <babylon/tools/optimization/hardware_scaling_optimization.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

HardwareScalingOptimization::HardwareScalingOptimization(int iPriority,
                                                         int iMaximumSize)
    : SceneOptimization{iPriority}
    , maximumScale{iMaximumSize}
    , _currentScale{1.f}
{
}

HardwareScalingOptimization::~HardwareScalingOptimization()
{
}

bool HardwareScalingOptimization::apply(Scene* scene)
{
  ++_currentScale;

  scene->getEngine()->setHardwareScalingLevel(_currentScale);

  return _currentScale >= maximumScale;
}

} // end of namespace BABYLON
