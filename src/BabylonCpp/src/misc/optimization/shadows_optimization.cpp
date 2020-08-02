#include <babylon/misc/optimization/shadows_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

ShadowsOptimization::ShadowsOptimization(int iPriority) : SceneOptimization{iPriority}
{
}

ShadowsOptimization::~ShadowsOptimization() = default;

std::string ShadowsOptimization::getDescription() const
{
  return "Turning shadows on/off";
}

bool ShadowsOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  scene->shadowsEnabled = optimizer->isInImprovementMode();
  return true;
}

} // end of namespace BABYLON
