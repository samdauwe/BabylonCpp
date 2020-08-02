#include <babylon/misc/optimization/post_processes_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

PostProcessesOptimization::PostProcessesOptimization(int iPriority) : SceneOptimization{iPriority}
{
}

PostProcessesOptimization::~PostProcessesOptimization() = default;

std::string PostProcessesOptimization::getDescription() const
{
  return "Turning post-processes on/off";
}

bool PostProcessesOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  scene->postProcessesEnabled = optimizer->isInImprovementMode();
  return true;
}

} // end of namespace BABYLON
