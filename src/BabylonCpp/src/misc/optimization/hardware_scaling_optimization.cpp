#include <babylon/misc/optimization/hardware_scaling_optimization.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

HardwareScalingOptimization::HardwareScalingOptimization(int iPriority, float iMaximumScale,
                                                         float iStep)
    : SceneOptimization{iPriority}
    , maximumScale{iMaximumScale}
    , step{iStep}
    , _currentScale{-1.f}
    , _directionOffset{1.f}
{
}

HardwareScalingOptimization::~HardwareScalingOptimization() = default;

std::string HardwareScalingOptimization::getDescription() const
{
  return "Setting hardware scaling level to " + std::to_string(_currentScale);
}

bool HardwareScalingOptimization::apply(Scene* scene, SceneOptimizer* /*optimizer*/)
{
  if (_currentScale == -1.f) {
    _currentScale = scene->getEngine()->getHardwareScalingLevel();
    if (_currentScale > maximumScale) {
      _directionOffset = -1;
    }
  }

  _currentScale += _directionOffset * step;

  scene->getEngine()->setHardwareScalingLevel(_currentScale);

  return _directionOffset == 1.f ? _currentScale >= maximumScale : _currentScale <= maximumScale;
}

} // end of namespace BABYLON
