#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_XOR_SHIFT_128_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_XOR_SHIFT_128_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/utils/irandom_function.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT XorShift128 : public IRandomFunction {
  XorShift128(size_t x = 0, size_t y = 0, size_t z = 0, size_t w = 0);
  ~XorShift128() override; // = default

  size_t next() override;
  float unit() override;
  float unitInclusive() override;
  size_t integer(size_t min, size_t max) override;
  size_t integerExclusive(size_t min, size_t max) override;
  float real(float min, float max) override;
  float realInclusive(float min, float max) override;
  void reseed(size_t x, size_t y, size_t z, size_t w) final;

  size_t x_;
  size_t y_;
  size_t z_;
  size_t w_;
}; // end of struct XorShift128

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_XOR_SHIFT_128_H
