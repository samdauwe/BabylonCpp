#include <babylon/tools/optimization/post_processes_optimization.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

PostProcessesOptimization::PostProcessesOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

PostProcessesOptimization::~PostProcessesOptimization()
{
}

bool PostProcessesOptimization::apply(Scene* scene)
{
  scene->postProcessesEnabled = false;
  return true;
}

} // end of namespace BABYLON
