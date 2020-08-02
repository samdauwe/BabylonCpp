#include <babylon/misc/optimization/particles_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

ParticlesOptimization::ParticlesOptimization(int iPriority) : SceneOptimization{iPriority}
{
}

ParticlesOptimization::~ParticlesOptimization() = default;

std::string ParticlesOptimization::getDescription() const
{
  return "Turning particles on/off";
}

bool ParticlesOptimization::apply(Scene* scene, SceneOptimizer* optimizer)
{
  scene->particlesEnabled = optimizer->isInImprovementMode();
  return true;
}

} // end of namespace BABYLON
