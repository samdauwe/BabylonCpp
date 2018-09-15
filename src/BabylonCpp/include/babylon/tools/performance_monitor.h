#ifndef BABYLON_TOOLS_PERFORMANCE_MONITOR_H
#define BABYLON_TOOLS_PERFORMANCE_MONITOR_H

#include <babylon/babylon_api.h>
#include <babylon/tools/rolling_average.h>

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
  ~PerformanceMonitor();

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
   * @return Average frame time in milliseconds
   */
  float averageFrameTime() const;

  /**
   * @brief Returns the variance frame time in milliseconds over the sliding
   * window (or the subset of frames sampled so far)
   * @return Frame time variance in milliseconds squared
   */
  float averageFrameTimeVariance() const;

  /**
   * @brief Returns the frame time of the most recent frame
   * @return Frame time in milliseconds
   */
  float instantaneousFrameTime() const;

  /**
   * @brief Returns the average framerate in frames per second over the sliding
   * window (or the subset of frames sampled so far)
   * @return Framerate in frames per second
   */
  float averageFPS() const;

  /**
   * @brief Returns the average framerate in frames per second using the most
   * recent frame time
   * @return Framerate in frames per second
   */
  float instantaneousFPS() const;

  /**
   * @brief Returns true if enough samples have been taken to completely fill
   * the sliding window
   * @return true if saturated
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
   * @return true if enabled
   */
  bool isEnabled() const;

  /**
   * @brief Resets performance monitor
   */
  void reset();

protected:
  bool _enabled;
  RollingAverage _rollingFrameTime;
  std::optional<high_res_time_point_t> _lastFrameTimeMs;
  std::optional<high_res_time_point_t> _lastChangeTimeMs;

}; // end of struct PerformanceMonitor

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_PERFORMANCE_MONITOR_H
