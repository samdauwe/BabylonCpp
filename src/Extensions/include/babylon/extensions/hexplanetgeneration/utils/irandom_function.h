#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_IRANDOM_FUNCTION_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_IRANDOM_FUNCTION_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Extensions {

struct IRandomFunction {
  virtual ~IRandomFunction() = default;
  virtual size_t next()                                       = 0;
  virtual float unit()                                        = 0;
  virtual float unitInclusive()                               = 0;
  virtual size_t integer(size_t min, size_t max)              = 0;
  virtual size_t integerExclusive(size_t min, size_t max)     = 0;
  virtual float real(float min, float max)                    = 0;
  virtual float realInclusive(float min, float max)           = 0;
  virtual void reseed(size_t x, size_t y, size_t z, size_t w) = 0;
}; // end of struct IRandomFunction

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_IRANDOM_FUNCTION_H
