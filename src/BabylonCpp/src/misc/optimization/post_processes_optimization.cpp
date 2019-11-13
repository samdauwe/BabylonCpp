#include <babylon/misc/optimization/post_processes_optimization.h>

#include <babylon/engines/scene.h>

namespace BABYLON {

PostProcessesOptimization::PostProcessesOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

PostProcessesOptimization::~PostProcessesOptimization() = default;

bool PostProcessesOptimization::apply(Scene* scene)
{
  scene->postProcessesEnabled = false;
  return true;
}

} // end of namespace BABYLON
