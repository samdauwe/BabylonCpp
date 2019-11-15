#ifndef BABYLON_MISC_ROLLING_AVERAGE_H
#define BABYLON_MISC_ROLLING_AVERAGE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief RollingAverage
 *
 * Utility to efficiently compute the rolling average and variance over a
 * sliding window of samples
 */
class BABYLON_SHARED_EXPORT RollingAverage {

public:
  /**
   * @brief Constructor
   * @param length The number of samples required to saturate the sliding window
   */
  RollingAverage(size_t length);
  ~RollingAverage(); // = default

  /**
   * @brief Adds a sample to the sample set
   * @param v The sample value
   */
  void add(float v);

  /**
   * @brief Returns previously added values or null if outside of history or
   * outside the sliding window domain
   * @param i Index in history. For example, pass 0 for the most recent value
   * and 1 for the value before that
   * @return Value previously recorded with add() or null if outside of range
   */
  float history(size_t i) const;

  /**
   * @brief Returns true if enough samples have been taken to completely fill
   * the sliding window
   * @return true if sample-set saturated
   */
  bool isSaturated() const;

  /**
   * @brief Resets the rolling average (equivalent to 0 samples taken so far)
   */
  void reset();

protected:
  /**
   * @brief Wraps a value around the sample range boundaries
   * @param i Position in sample range, for example if the sample length is 5,
   * and i is -3, then 2 will be returned.
   * @return Wrapped position in sample range
   */
  std::size_t _wrapPosition(std::size_t i) const;

public:
  /**
   * Current average
   */
  float average;
  /**
   * Current variance
   */
  float variance;

protected:
  Float32Array _samples;
  std::size_t _sampleCount;
  std::size_t _pos;
  float _m2; // sum of squares of differences from the (current) mean

}; // end of struct RollingAverage

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_ROLLING_AVERAGE_H
