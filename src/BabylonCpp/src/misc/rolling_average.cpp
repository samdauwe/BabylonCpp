#include <babylon/misc/rolling_average.h>

namespace BABYLON {

RollingAverage::RollingAverage(std::size_t length) : _samples{Float32Array(length)}
{
  reset();
}

RollingAverage::~RollingAverage() = default;

void RollingAverage::add(float v)
{
  // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
  auto delta = 0.f;

  // we need to check if we've already wrapped round
  if (isSaturated()) {
    // remove bottom of stack from mean
    const auto bottomValue = _samples[_pos];
    delta                  = bottomValue - average;
    average -= delta / (_sampleCount - 1);
    _m2 -= delta * (bottomValue - average);
  }
  else {
    ++_sampleCount;
  }

  // add new value to mean
  delta = v - average;
  average += delta / (_sampleCount);
  _m2 += delta * (v - average);

  // set the new variance
  variance = _m2 / (_sampleCount - 1);

  _samples[_pos] = v;
  ++_pos;

  _pos %= _samples.size(); // positive wrap around
}

float RollingAverage::history(std::size_t i) const
{
  if ((i >= _sampleCount) || (i >= _samples.size())) {
    return 0.f;
  }

  const auto i0 = _wrapPosition(_pos - 1);
  return _samples[_wrapPosition(i0 - i)];
}

bool RollingAverage::isSaturated() const
{
  return _sampleCount >= _samples.size();
}

void RollingAverage::reset()
{
  average      = 0.f;
  variance     = 0.f;
  _sampleCount = 0;
  _pos         = 0;
  _m2          = 0.f;
}

std::size_t RollingAverage::_wrapPosition(std::size_t i) const
{
  const auto max = _samples.size();
  return ((i % max) + max) % max;
}

} // end of namespace BABYLON
