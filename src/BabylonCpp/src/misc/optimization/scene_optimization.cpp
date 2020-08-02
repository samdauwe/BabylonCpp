#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

SceneOptimization::SceneOptimization(int iPriority) : priority{iPriority}
{
}

SceneOptimization::~SceneOptimization() = default;

std::string SceneOptimization::getDescription() const
{
  return "";
}

bool SceneOptimization::apply(Scene* /*scene*/, SceneOptimizer* /*optimizer*/)
{
  return true; // Return true if everything that can be done was applied
}

} // end of namespace BABYLON
