#ifndef BABYLON_CORE_PROFILING_TIMER_H
#define BABYLON_CORE_PROFILING_TIMER_H
#ifdef _WIN32
#include <babylon/babylon_api.h>
#include <windows.h>

namespace BABYLON {

/**
 * @brief High Resolution Timer.
 * This timer is able to measure the elapsed time with 1 micro-second accuracy
 * on Windows systems
 */
class BABYLON_SHARED_EXPORT Timer {

public:
  /**
   * @brief Default constructor.
   */
  Timer();

  /**
   * @brief default destructor.
   */
  ~Timer();

  /**
   * @brief Starts the timer.
   */
  void start();

  /**
   * @brief Stops the timer.
   */
  void stop();

  /**
   * @brief Get elapsed time in seconds.
   * @return Elapsed time in seconds.
   */
  double getElapsedTime();

  /**
   * @brief Get elapsed time in second (same as getElapsedTime).
   * @return Elapsed time in second.
   */
  double getElapsedTimeInSec();

  /**
   * @brief Get elapsed time in milli-seconds.
   * @return Elapsed time in milli-seconds.
   */
  double getElapsedTimeInMilliSec();

  /**
   * @brief Get elapsed time in micro-seconds.
   * @return Elapsed time in micro-seconds.
   */
  double getElapsedTimeInMicroSec();

private:
  // Starting time in micro-second
  double startTimeInMicroSec;
  // Ending time in micro-second
  double endTimeInMicroSec;
  // stop flag
  bool stopped;
  LARGE_INTEGER frequency;
  LARGE_INTEGER startCount;
  LARGE_INTEGER endCount;

}; // end of class Timer

} // end of namespace BABYLON
#endif // #ifdef _WIN32
#endif // BABYLON_CORE_PROFILING_TIMER_H
