#include <babylon/misc/optimization/scene_optimizer_options.h>

#include <babylon/misc/optimization/custom_optimization.h>
#include <babylon/misc/optimization/hardware_scaling_optimization.h>
#include <babylon/misc/optimization/lens_flares_optimization.h>
#include <babylon/misc/optimization/merge_meshes_optimization.h>
#include <babylon/misc/optimization/particles_optimization.h>
#include <babylon/misc/optimization/post_processes_optimization.h>
#include <babylon/misc/optimization/render_targets_optimization.h>
#include <babylon/misc/optimization/shadows_optimization.h>
#include <babylon/misc/optimization/texture_optimization.h>

namespace BABYLON {

SceneOptimizerOptions::SceneOptimizerOptions(float iTargetFrameRate, int iTrackerDuration)
    : targetFrameRate{iTargetFrameRate}, trackerDuration{iTrackerDuration}
{
}

SceneOptimizerOptions::SceneOptimizerOptions(const SceneOptimizerOptions& other) = default;

SceneOptimizerOptions::SceneOptimizerOptions(SceneOptimizerOptions&& other) = default;

SceneOptimizerOptions& SceneOptimizerOptions::operator=(const SceneOptimizerOptions& other)
  = default;

SceneOptimizerOptions& SceneOptimizerOptions::operator=(SceneOptimizerOptions&& other) = default;

SceneOptimizerOptions::~SceneOptimizerOptions() = default;

SceneOptimizerOptions& SceneOptimizerOptions::addOptimization(const SceneOptimization& optimization)
{
  optimizations.emplace_back(optimization);
  return *this;
}

SceneOptimizerOptions& SceneOptimizerOptions::addCustomOptimization(
  const std::function<bool(Scene* scene, SceneOptimizer* optimizer)>& onApply,
  const std::function<std::string()>& onGetDescription, int priority)
{
  CustomOptimization optimization(priority);
  optimization.onApply          = onApply;
  optimization.onGetDescription = onGetDescription;

  optimizations.emplace_back(optimization);
  return *this;
}

SceneOptimizerOptions SceneOptimizerOptions::LowDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.addOptimization(MergeMeshesOptimization(priority));
  result.addOptimization(ShadowsOptimization(priority));
  result.addOptimization(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(PostProcessesOptimization(priority));
  result.addOptimization(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(TextureOptimization(priority, 1024));

  return result;
}

SceneOptimizerOptions SceneOptimizerOptions::ModerateDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.addOptimization(MergeMeshesOptimization(priority));
  result.addOptimization(ShadowsOptimization(priority));
  result.addOptimization(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(PostProcessesOptimization(priority));
  result.addOptimization(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(TextureOptimization(priority, 512));

  // Next priority
  ++priority;
  result.addOptimization(RenderTargetsOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(HardwareScalingOptimization(priority, 2.f));

  return result;
}

SceneOptimizerOptions SceneOptimizerOptions::HighDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.addOptimization(MergeMeshesOptimization(priority));
  result.addOptimization(ShadowsOptimization(priority));
  result.addOptimization(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(PostProcessesOptimization(priority));
  result.addOptimization(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(TextureOptimization(priority, 256));

  // Next priority
  ++priority;
  result.addOptimization(RenderTargetsOptimization(priority));

  // Next priority
  ++priority;
  result.addOptimization(HardwareScalingOptimization(priority, 4.f));

  return result;
}

} // end of namespace BABYLON
