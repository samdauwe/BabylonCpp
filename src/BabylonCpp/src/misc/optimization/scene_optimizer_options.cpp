#include <babylon/misc/optimization/scene_optimizer_options.h>

#include <babylon/misc/optimization/hardware_scaling_optimization.h>
#include <babylon/misc/optimization/lens_flares_optimization.h>
#include <babylon/misc/optimization/merge_meshes_optimization.h>
#include <babylon/misc/optimization/particles_optimization.h>
#include <babylon/misc/optimization/post_processes_optimization.h>
#include <babylon/misc/optimization/render_targets_optimization.h>
#include <babylon/misc/optimization/shadows_optimization.h>
#include <babylon/misc/optimization/texture_optimization.h>

namespace BABYLON {

SceneOptimizerOptions::SceneOptimizerOptions(float iTargetFrameRate,
                                             int iTrackerDuration)
    : targetFrameRate{iTargetFrameRate}, trackerDuration{iTrackerDuration}
{
}

SceneOptimizerOptions::SceneOptimizerOptions(const SceneOptimizerOptions& other) = default;

SceneOptimizerOptions::SceneOptimizerOptions(SceneOptimizerOptions&& other) = default;

SceneOptimizerOptions& SceneOptimizerOptions::
operator=(const SceneOptimizerOptions& other) = default;

SceneOptimizerOptions& SceneOptimizerOptions::
operator=(SceneOptimizerOptions&& other) = default;

SceneOptimizerOptions::~SceneOptimizerOptions() = default;

SceneOptimizerOptions
SceneOptimizerOptions::LowDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.optimizations.emplace_back(MergeMeshesOptimization(priority));
  result.optimizations.emplace_back(ShadowsOptimization(priority));
  result.optimizations.emplace_back(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(PostProcessesOptimization(priority));
  result.optimizations.emplace_back(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(TextureOptimization(priority, 1024));

  return result;
}

SceneOptimizerOptions
SceneOptimizerOptions::ModerateDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.optimizations.emplace_back(MergeMeshesOptimization(priority));
  result.optimizations.emplace_back(ShadowsOptimization(priority));
  result.optimizations.emplace_back(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(PostProcessesOptimization(priority));
  result.optimizations.emplace_back(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(TextureOptimization(priority, 512));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(RenderTargetsOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(HardwareScalingOptimization(priority, 2));

  return result;
}

SceneOptimizerOptions
SceneOptimizerOptions::HighDegradationAllowed(float targetFrameRate)
{
  SceneOptimizerOptions result(targetFrameRate);

  int priority = 0;
  result.optimizations.emplace_back(MergeMeshesOptimization(priority));
  result.optimizations.emplace_back(ShadowsOptimization(priority));
  result.optimizations.emplace_back(LensFlaresOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(PostProcessesOptimization(priority));
  result.optimizations.emplace_back(ParticlesOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(TextureOptimization(priority, 256));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(RenderTargetsOptimization(priority));

  // Next priority
  ++priority;
  result.optimizations.emplace_back(HardwareScalingOptimization(priority, 4));

  return result;
}

} // end of namespace BABYLON
