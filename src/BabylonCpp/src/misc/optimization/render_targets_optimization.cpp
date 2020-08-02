#include <babylon/misc/optimization/render_targets_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

RenderTargetsOptimization::RenderTargetsOptimization(int iPriority) : SceneOptimization{iPriority}
{
}

RenderTargetsOptimization::~RenderTargetsOptimization() = default;

std::string RenderTargetsOptimization::getDescription() const
{
  return "Turning render targets off";
}

bool RenderTargetsOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  scene->renderTargetsEnabled = optimizer->isInImprovementMode();
  return true;
}

} // end of namespace BABYLON
