#include <babylon/misc/optimization/custom_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

CustomOptimization::CustomOptimization(int iPriority)
    : SceneOptimization{iPriority}, onApply{nullptr}, onGetDescription{nullptr}
{
}

CustomOptimization::~CustomOptimization() = default;

std::string CustomOptimization::getDescription() const
{
  if (onGetDescription) {
    return onGetDescription();
  }

  return "Running user defined callback";
}

bool CustomOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  if (onApply) {
    return onApply(scene, optimizer);
  }
  return true;
}

} // end of namespace BABYLON
