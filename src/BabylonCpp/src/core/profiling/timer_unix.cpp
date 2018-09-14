#include <babylon/core/profiling/timer_unix.h>

namespace BABYLON {

Timer::Timer() : startTimeInMicroSec{0}, endTimeInMicroSec{0}, stopped{false}
{
  startCount.tv_sec = startCount.tv_usec = 0;
  endCount.tv_sec = endCount.tv_usec = 0;
}

Timer::~Timer()
{
}

void Timer::start()
{
  stopped = false; // reset stop flag
  gettimeofday(&startCount, nullptr);
}

void Timer::stop()
{
  stopped = true; // set timer stopped flag
  gettimeofday(&endCount, nullptr);
}

double Timer::getElapsedTimeInMicroSec()
{

  if (!stopped) {
    gettimeofday(&endCount, nullptr);
  }

  startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
  endTimeInMicroSec   = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;

  return endTimeInMicroSec - startTimeInMicroSec;
}

double Timer::getElapsedTimeInMilliSec()
{
  return getElapsedTimeInMicroSec() * 0.001;
}

double Timer::getElapsedTimeInSec()
{
  return getElapsedTimeInMicroSec() * 0.000001;
}

double Timer::getElapsedTime()
{
  return getElapsedTimeInSec();
}

} // end of namespace BABYLON
