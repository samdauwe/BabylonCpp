#include <babylon/tools/performance_monitor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/time.h>

namespace BABYLON {

PerformanceMonitor::PerformanceMonitor(std::size_t frameSampleSize)
    : _enabled{true}, _rollingFrameTime{RollingAverage(frameSampleSize)}
{
}

PerformanceMonitor::~PerformanceMonitor()
{
}

void PerformanceMonitor::sampleFrame()
{
  sampleFrame(Time::highresTimepointNow());
}

void PerformanceMonitor::sampleFrame(high_res_time_point_t timeMs)
{
  if (!_enabled) {
    return;
  }

  if (!_lastFrameTimeMs.isNull()) {
    auto dt = Time::fpTimeDiff<float, ::std::milli>(*_lastFrameTimeMs, timeMs);
    _rollingFrameTime.add(dt);
  }

  _lastFrameTimeMs = timeMs;
}

float PerformanceMonitor::averageFrameTime() const
{
  return _rollingFrameTime.average;
}

float PerformanceMonitor::averageFrameTimeVariance() const
{
  return _rollingFrameTime.variance;
}

float PerformanceMonitor::instantaneousFrameTime() const
{
  return _rollingFrameTime.history(0);
}

float PerformanceMonitor::averageFPS() const
{
  return 1000.f / _rollingFrameTime.average;
}

float PerformanceMonitor::instantaneousFPS() const
{
  auto history = _rollingFrameTime.history(0);

  if (stl_util::almost_equal(history, 0.f)) {
    return 0;
  }

  return 1000.f / history;
}

bool PerformanceMonitor::isSaturated() const
{
  return _rollingFrameTime.isSaturated();
}

void PerformanceMonitor::enable()
{
  _enabled = true;
}

void PerformanceMonitor::disable()
{
  _enabled = false;
  // clear last sample to avoid interpolating over the disabled period when next
  // enabled
  _lastFrameTimeMs  = nullptr;
  _lastChangeTimeMs = nullptr;
}

bool PerformanceMonitor::isEnabled() const
{
  return _enabled;
}

void PerformanceMonitor::reset()
{
  // clear last sample to avoid interpolating over the disabled period when next
  // enabled
  _lastFrameTimeMs  = nullptr;
  _lastChangeTimeMs = nullptr;
  // wipe record
  _rollingFrameTime.reset();
}

} // end of namespace BABYLON
