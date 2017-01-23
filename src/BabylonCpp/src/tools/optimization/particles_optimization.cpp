#include <babylon/tools/optimization/particles_optimization.h>

#include <babylon/engine/scene.h>

namespace BABYLON {

ParticlesOptimization::ParticlesOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

ParticlesOptimization::~ParticlesOptimization()
{
}

bool ParticlesOptimization::apply(Scene* scene)
{
  scene->particlesEnabled = false;
  return true;
}

} // end of namespace BABYLON
