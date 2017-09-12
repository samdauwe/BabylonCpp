#include <babylon/tools/optimization/scene_optimizer_options.h>

#include <babylon/tools/optimization/hardware_scaling_optimization.h>
#include <babylon/tools/optimization/lens_flares_optimization.h>
#include <babylon/tools/optimization/merge_meshes_optimization.h>
#include <babylon/tools/optimization/particles_optimization.h>
#include <babylon/tools/optimization/post_processes_optimization.h>
#include <babylon/tools/optimization/render_targets_optimization.h>
#include <babylon/tools/optimization/shadows_optimization.h>
#include <babylon/tools/optimization/texture_optimization.h>

namespace BABYLON {

SceneOptimizerOptions::SceneOptimizerOptions(float iTargetFrameRate,
                                             int iTrackerDuration)
    : targetFrameRate{iTargetFrameRate}, trackerDuration{iTrackerDuration}
{
}

SceneOptimizerOptions::SceneOptimizerOptions(const SceneOptimizerOptions& other)
    : optimizations{other.optimizations}
    , targetFrameRate{other.targetFrameRate}
    , trackerDuration{other.trackerDuration}
{
}

SceneOptimizerOptions::SceneOptimizerOptions(SceneOptimizerOptions&& other)
    : optimizations{::std::move(other.optimizations)}
    , targetFrameRate{::std::move(other.targetFrameRate)}
    , trackerDuration{::std::move(other.trackerDuration)}
{
}

SceneOptimizerOptions& SceneOptimizerOptions::
operator=(const SceneOptimizerOptions& other)
{
  if (&other != this) {
    optimizations   = other.optimizations;
    targetFrameRate = other.targetFrameRate;
    trackerDuration = other.trackerDuration;
  }

  return *this;
}

SceneOptimizerOptions& SceneOptimizerOptions::
operator=(SceneOptimizerOptions&& other)
{
  if (&other != this) {
    optimizations   = ::std::move(other.optimizations);
    targetFrameRate = ::std::move(other.targetFrameRate);
    trackerDuration = ::std::move(other.trackerDuration);
  }

  return *this;
}

SceneOptimizerOptions::~SceneOptimizerOptions()
{
}

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
