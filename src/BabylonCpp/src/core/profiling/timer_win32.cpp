#include <babylon/core/profiling/timer_win32.h>

namespace BABYLON {

Timer::Timer() : startTimeInMicroSec{0}, endTimeInMicroSec{0}, stopped{false}
{
  QueryPerformanceFrequency(&frequency);
  startCount.QuadPart = 0;
  endCount.QuadPart   = 0;
}

Timer::~Timer()
{
}

void Timer::start()
{
  stopped = false; // reset stop flag
  QueryPerformanceCounter(&startCount);
}

void Timer::stop()
{
  stopped = true; // set timer stopped flag
  QueryPerformanceCounter(&endCount);
}

double Timer::getElapsedTimeInMicroSec()
{

  if (!stopped) {
    QueryPerformanceCounter(&endCount);
  }

  startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
  endTimeInMicroSec   = endCount.QuadPart * (1000000.0 / frequency.QuadPart);

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
