#include <babylon/tools/optimization/shadows_optimization.h>

#include <babylon/engines/scene.h>

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
  scene->shadowsEnabled = false;
  return true;
}

} // end of namespace BABYLON
