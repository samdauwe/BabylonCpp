#include <babylon/misc/optimization/particles_optimization.h>

#include <babylon/engines/scene.h>

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
