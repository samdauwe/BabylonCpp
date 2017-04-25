#include <babylon/tools/optimization/shadows_optimization.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

ShadowsOptimization::ShadowsOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

ShadowsOptimization::~ShadowsOptimization()
{
}

bool ShadowsOptimization::apply(Scene* scene)
{
  scene->setShadowsEnabled(false);
  return true;
}

} // end of namespace BABYLON
