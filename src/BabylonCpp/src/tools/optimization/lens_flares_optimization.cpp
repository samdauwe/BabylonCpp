#include <babylon/tools/optimization/lens_flares_optimization.h>

#include <babylon/engines/scene.h>

namespace BABYLON {

LensFlaresOptimization::LensFlaresOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

LensFlaresOptimization::~LensFlaresOptimization()
{
}

bool LensFlaresOptimization::apply(Scene* scene)
{
  scene->lensFlaresEnabled = false;
  return true;
}

} // end of namespace BABYLON
