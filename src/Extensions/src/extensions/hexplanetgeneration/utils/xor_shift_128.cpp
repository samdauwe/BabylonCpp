#include <babylon/extensions/hexplanetgeneration/utils/xor_shift_128.h>

#include <cmath>

namespace BABYLON {
namespace Extensions {

XorShift128::XorShift128(size_t x, size_t y, size_t z, size_t w)
{
  reseed(x, y, z, w);
}

XorShift128::~XorShift128()
{
}

size_t XorShift128::next()
{
  size_t t = x_ ^ ((x_ << 11) & 0x7FFFFFFF);
  x_       = y_;
  y_       = z_;
  z_       = w_;
  w_       = (w_ ^ (w_ >> 19)) ^ (t ^ (t >> 8));
  return w_;
}

float XorShift128::unit()
{
  double _unit = static_cast<double>(next()) / static_cast<double>(0x80000000);
  return static_cast<float>(_unit);
}

float XorShift128::unitInclusive()
{
  double _unit = static_cast<double>(next()) / static_cast<double>(0x7FFFFFFF);
  return static_cast<float>(_unit);
}

size_t XorShift128::integer(size_t min, size_t max)
{
  return integerExclusive(min, max + 1);
}

size_t XorShift128::integerExclusive(size_t min, size_t max)
{
  return static_cast<size_t>(std::floor(unit() * (max - min)) + min);
}

float XorShift128::real(float min, float max)
{
  return unit() * (max - min) + min;
}

float XorShift128::realInclusive(float min, float max)
{
  return unitInclusive() * (max - min) + min;
}

void XorShift128::reseed(size_t x, size_t y, size_t z, size_t w)
{
  x_ = (x ? x : 123456789);
  y_ = (y ? y : 362436069);
  z_ = (z ? z : 521288629);
  w_ = (w ? w : 88675123);
}

} // end of namespace Extensions
} // namespace BABYLON
