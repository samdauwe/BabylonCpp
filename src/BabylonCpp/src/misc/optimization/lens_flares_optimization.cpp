#include <babylon/misc/optimization/lens_flares_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

LensFlaresOptimization::LensFlaresOptimization(int iPriority) : SceneOptimization{iPriority}
{
}

LensFlaresOptimization::~LensFlaresOptimization() = default;

std::string LensFlaresOptimization::getDescription() const
{
  return "Turning lens flares on/off";
}

bool LensFlaresOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  scene->lensFlaresEnabled = optimizer->isInImprovementMode();
  return true;
}

} // end of namespace BABYLON
