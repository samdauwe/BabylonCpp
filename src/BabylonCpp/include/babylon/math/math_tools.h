#ifndef BABYLON_MATH_MATH_TOOLS_H
#define BABYLON_MATH_MATH_TOOLS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MathTools {

public:
  static const float ToGammaSpace;
  static const float ToLinearSpace;
  static const float Epsilon;

public:
  static bool WithinEpsilon(float a, float b, float epsilon = 1.401298E-45f);
  static std::string ToHex(int i);
  static int Sign(float x);
  static float Signf(float x);
  static float Clamp(float value, float min = 0.f, float max = 1.f);

}; // end of class MathTools

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATH_TOOLS_H
