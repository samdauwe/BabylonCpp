#ifndef BABYLON_MATH_SPHERICAL_POLYNOMIAL_H
#define BABYLON_MATH_SPHERICAL_POLYNOMIAL_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SphericalPolynomial {

public:
  SphericalPolynomial();
  SphericalPolynomial(const SphericalPolynomial& other);
  SphericalPolynomial(SphericalPolynomial&& other);
  SphericalPolynomial& operator=(const SphericalPolynomial& other);
  SphericalPolynomial& operator=(SphericalPolynomial&& other);
  ~SphericalPolynomial();
  SphericalPolynomial copy() const;
  std::unique_ptr<SphericalPolynomial> clone() const;

  void addAmbient(const Color3& color);
  void scale(float iScale);

  /** Statics **/
  static SphericalPolynomial
  getSphericalPolynomialFromHarmonics(const SphericalHarmonics& harmonics);

public:
  Vector3 x;
  Vector3 y;
  Vector3 z;
  Vector3 xx;
  Vector3 yy;
  Vector3 zz;
  Vector3 xy;
  Vector3 yz;
  Vector3 zx;

}; // end of class SphericalPolynomial

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SPHERICAL_POLYNOMIAL_H
