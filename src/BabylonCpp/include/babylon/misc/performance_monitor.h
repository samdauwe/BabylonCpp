#ifndef BABYLON_MISC_PERFORMANCE_MONITOR_H
#define BABYLON_MISC_PERFORMANCE_MONITOR_H

#include <babylon/babylon_api.h>
#include <babylon/misc/rolling_average.h>

namespace BABYLON {

/**
 * @brief Performance monitor tracks rolling average frame-time and frame-time
 * variance over a user defined sliding-window
 */
class BABYLON_SHARED_EXPORT PerformanceMonitor {

public:
  /**
   * @brief Constructor
   * @param frameSampleSize The number of samples required to saturate the
   * sliding window
   */
  PerformanceMonitor(std::size_t frameSampleSize = 30);
  ~PerformanceMonitor(); // = default

  /**
   * @brief Samples current frame
   */
  void sampleFrame();

  /**
   * @brief Samples current frame
   * @param timeMs A timestamp in milliseconds of the current frame to compare
   * with other frames
   */
  void sampleFrame(high_res_time_point_t timeMs);

  /**
   * @brief Returns the average frame time in milliseconds over the sliding
   * window (or the subset of frames sampled so far)
   */
  float averageFrameTime() const;

  /**
   * @brief Returns the variance frame time in milliseconds over the sliding
   * window (or the subset of frames sampled so far)
   */
  float averageFrameTimeVariance() const;

  /**
   * @brief Returns the frame time of the most recent frame
   */
  float instantaneousFrameTime() const;

  /**
   * @brief Returns the average framerate in frames per second over the sliding
   * window (or the subset of frames sampled so far)
   */
  float averageFPS() const;

  /**
   * @brief Returns the average framerate in frames per second using the most
   * recent frame time
   */
  float instantaneousFPS() const;

  /**
   * @brief Returns true if enough samples have been taken to completely fill
   * the sliding window
   */
  bool isSaturated() const;

  /**
   * @brief Enables contributions to the sliding window sample set
   */
  void enable();

  /**
   * @brief Disables contributions to the sliding window sample set
   * Samples will not be interpolated over the disabled period
   */
  void disable();

  /**
   * @brief Returns true if sampling is enabled
   */
  bool isEnabled() const;

  /**
   * @brief Resets performance monitor
   */
  void reset();

private:
  bool _enabled;
  RollingAverage _rollingFrameTime;
  std::optional<high_res_time_point_t> _lastFrameTimeMs;

}; // end of struct PerformanceMonitor

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_PERFORMANCE_MONITOR_H
