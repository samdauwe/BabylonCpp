#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

SceneOptimization::SceneOptimization(int iPriority) : priority{iPriority}
{
}

SceneOptimization::~SceneOptimization()
{
}

bool SceneOptimization::apply(Scene* /*scene*/)
{
  return true; // Return true if everything that can be done was applied
}

} // end of namespace BABYLON
