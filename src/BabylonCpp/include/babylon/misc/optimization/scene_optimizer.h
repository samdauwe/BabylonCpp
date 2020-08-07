#ifndef BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_H
#define BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>
#include <babylon/misc/optimization/scene_optimizer_options.h>

namespace BABYLON {

class SceneOptimizer;
using SceneOptimizerPtr = std::shared_ptr<SceneOptimizer>;

/**
 * @brief Class used to run optimizations in order to reach a target frame rate.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT SceneOptimizer : public IDisposable {

public:
  /**
   * @brief Creates a new SceneOptimizer.
   * @param scene defines the scene to work on
   * @param options defines the options to use with the SceneOptimizer
   * @param autoGeneratePriorities defines if priorities must be generated and not read from
   * SceneOptimization property (true by default)
   * @param improvementMode defines if the scene optimizer must run the maximum optimization while
   * staying over a target frame instead of trying to reach the target framerate (false by default)
   */
  SceneOptimizer(Scene* scene, const std::optional<SceneOptimizerOptions>& options = std::nullopt,
                 bool autoGeneratePriorities = true, bool improvementMode = false);
  ~SceneOptimizer(); // = default

  /**
   * @brief Stops the current optimizer.
   */
  void stop();

  /**
   * @brief Reset the optimizer to initial step (current priority level = 0).
   */
  void reset();

  /**
   * @brief Start the optimizer. By default it will try to reach a specific framerate
   * but if the optimizer is set with improvementMode === true then it will run all optimiatiation
   * while frame rate is above the target frame rate
   */
  void start();

  /**
   * @brief Release all resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Helper function to create a SceneOptimizer with one single line of code.
   * @param scene defines the scene to work on
   * @param options defines the options to use with the SceneOptimizer
   * @param onSuccess defines a callback to call on success
   * @param onFailure defines a callback to call on failure
   * @returns the new SceneOptimizer object
   */
  static SceneOptimizerPtr
  OptimizeAsync(Scene* scene, const std::optional<SceneOptimizerOptions>& options = std::nullopt,
                const std::function<void()>& onSuccess = nullptr,
                const std::function<void()>& onFailure = nullptr);

protected:
  /**
   * @brief Gets a boolean indicating if the optimizer is in improvement mode.
   */
  bool get_isInImprovementMode() const;

  /**
   * @brief Gets the current priority level (0 at start).
   */
  int get_currentPriorityLevel() const;

  /**
   * @brief Gets the current frame rate checked by the SceneOptimizer.
   */
  float get_currentFrameRate() const;

  /**
   * @brief Gets the current target frame rate (60 by default).
   */
  float get_targetFrameRate() const;

  /**
   * @brief Sets the current target frame rate (60 by default).
   */
  void set_targetFrameRate(float value);

  /**
   * @brief Gets the current interval between two checks (every 2000ms by default).
   */
  int get_trackerDuration() const;

  /**
   * @brief Sets the current interval between two checks (every 2000ms by default).
   */
  void set_trackerDuration(int value);

  /**
   * @brief Gets the list of active optimizations.
   */
  std::vector<SceneOptimization>& get_optimizations();

private:
  void _checkCurrentState();

public:
  /**
   * Defines an observable called when the optimizer reaches the target frame rate
   */
  Observable<SceneOptimizer> onSuccessObservable;

  /**
   * Defines an observable called when the optimizer enables an optimization
   */
  Observable<SceneOptimization> onNewOptimizationAppliedObservable;

  /**
   * Defines an observable called when the optimizer is not able to reach the target frame rate
   */
  Observable<SceneOptimizer> onFailureObservable;

  /**
   * Gets a boolean indicating if the optimizer is in improvement mode
   */
  ReadOnlyProperty<SceneOptimizer, bool> isInImprovementMode;

  /**
   * Gets the current priority level (0 at start)
   */
  ReadOnlyProperty<SceneOptimizer, int> currentPriorityLevel;

  /**
   * Gets the current frame rate checked by the SceneOptimizer
   */
  ReadOnlyProperty<SceneOptimizer, float> currentFrameRate;

  /**
   * Gets or sets the current target frame rate (60 by default)
   */
  Property<SceneOptimizer, float> targetFrameRate;

  /**
   * Gets or sets the current interval between two checks (every 2000ms by default)
   */
  Property<SceneOptimizer, int> trackerDuration;

  /**
   * Gets the list of active optimizations
   */
  ReadOnlyProperty<SceneOptimizer, std::vector<SceneOptimization>> optimizations;

private:
  bool _isRunning;
  SceneOptimizerOptions _options;
  Scene* _scene;
  int _currentPriorityLevel;
  float _targetFrameRate;
  int _trackerDuration;
  float _currentFrameRate;
  Observer<Scene>::Ptr _sceneDisposeObserver;
  bool _improvementMode;

}; // end of class SceneOptimizer

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_H
