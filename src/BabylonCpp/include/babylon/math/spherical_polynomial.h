#ifndef BABYLON_MATH_SPHERICAL_POLYNOMIAL_H
#define BABYLON_MATH_SPHERICAL_POLYNOMIAL_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/spherical_harmonics.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Color3;
class SphericalHarmonics;

/**
 * @brief Class representing spherical polynomial coefficients to the 3rd
 * degree.
 */
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

  /**
   * @brief Gets the spherical harmonics used to create the polynomials.
   */
  SphericalHarmonics& preScaledHarmonics();

  /**
   * @brief Adds an ambient color to the spherical polynomial.
   * @param color the color to add
   */
  void addAmbient(const Color3& color);

  /**
   * @brief Scales the spherical polynomial by the given amount.
   * @param scale the amount to scale
   */
  void scaleInPlace(float scale);

  /** Statics **/

  /**
   * @brief Gets the spherical polynomial from harmonics.
   * @param harmonics the spherical harmonics
   * @returns the spherical polynomial
   */
  static SphericalPolynomial FromHarmonics(const SphericalHarmonics& harmonics);

  /**
   * @brief Constructs a spherical polynomial from an array.
   * @param data defines the 9x3 coefficients (x, y, z, xx, yy, zz, yz, zx, xy)
   * @returns the spherical polynomial
   */
  static SphericalPolynomial FromArray(const std::vector<Float32Array>& data);

public:
  /**
   * The x coefficients of the spherical polynomial
   */
  Vector3 x;

  /**
   * The y coefficients of the spherical polynomial
   */
  Vector3 y;

  /**
   * The z coefficients of the spherical polynomial
   */
  Vector3 z;

  /**
   * The xx coefficients of the spherical polynomial
   */
  Vector3 xx;

  /**
   * The yy coefficients of the spherical polynomial
   */
  Vector3 yy;

  /**
   * The zz coefficients of the spherical polynomial
   */
  Vector3 zz;

  /**
   * The xy coefficients of the spherical polynomial
   */
  Vector3 xy;

  /**
   * The yz coefficients of the spherical polynomial
   */
  Vector3 yz;

  /**
   * The zx coefficients of the spherical polynomial
   */
  Vector3 zx;

private:
  std::optional<SphericalHarmonics> _harmonics;

}; // end of class SphericalPolynomial

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SPHERICAL_POLYNOMIAL_H
