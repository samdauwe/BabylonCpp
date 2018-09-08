#ifndef BABYLON_MATH_SPHERICAL_HARMONICS_H
#define BABYLON_MATH_SPHERICAL_HARMONICS_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Color3;
class SphericalPolynomial;

/**
 * @brief Class representing spherical harmonics coefficients to the 3rd degree.
 */
class BABYLON_SHARED_EXPORT SphericalHarmonics {

public:
  SphericalHarmonics();
  SphericalHarmonics(const SphericalHarmonics& other);
  SphericalHarmonics(SphericalHarmonics&& other);
  SphericalHarmonics& operator=(const SphericalHarmonics& other);
  SphericalHarmonics& operator=(SphericalHarmonics&& other);
  ~SphericalHarmonics();
  SphericalHarmonics copy() const;
  std::unique_ptr<SphericalHarmonics> clone() const;

  /**
   * @brief Adds a light to the spherical harmonics.
   * @param direction the direction of the light
   * @param color the color of the light
   * @param deltaSolidAngle the delta solid angle of the light
   */
  void addLight(const Vector3& direction, const Color3& color,
                float deltaSolidAngle);

  /**
   * @brief Scales the spherical harmonics by the given amount.
   * @param scale the amount to scale
   */
  void scale(float scale);

  /**
   * @brief Convert from incident radiance (Li) to irradiance (E) by applying
   * convolution with the cosine-weighted hemisphere.
   *
   * ```
   * E_lm = A_l * L_lm
   * ```
   *
   * In spherical harmonics this convolution amounts to scaling factors for each
   * frequency band. This corresponds to equation 5 in "An Efficient
   * Representation for Irradiance Environment Maps", where the scaling factors
   * are given in equation 9.
   */
  void convertIncidentRadianceToIrradiance();

  /**
   * @brief Convert from irradiance to outgoing radiance for Lambertian BDRF,
   * suitable for efficient shader evaluation.
   *
   * ```
   * L = (1/pi) * E * rho
   * ```
   *
   * This is done by an additional scale by 1/pi, so is a fairly trivial
   * operation but important conceptually.
   */
  void convertIrradianceToLambertianRadiance();

  /**
   * @brief Gets the spherical harmonics from polynomial.
   * @param polynomial the spherical polynomial
   * @returns the spherical harmonics
   */
  static SphericalHarmonics
  FromPolynomial(const SphericalPolynomial& polynomial);

  /**
   * @brief Constructs a spherical harmonics from an array.
   * @param data defines the 9x3 coefficients (l00, l1-1, l10, l11, l2-2, l2-1,
   * l20, l21, l22)
   * @returns the spherical harmonics
   */
  static SphericalHarmonics FromArray(const std::vector<Float32Array>& data);

public:
  /**
   * The l0,0 coefficients of the spherical harmonics
   */
  Vector3 l00;

  /**
   * The l1,-1 coefficients of the spherical harmonics
   */
  Vector3 l1_1;

  /**
   * The l1,0 coefficients of the spherical harmonics
   */
  Vector3 l10;

  /**
   * The l1,1 coefficients of the spherical harmonics
   */
  Vector3 l11;

  /**
   * The l2,-2 coefficients of the spherical harmonics
   */
  Vector3 l2_2;

  /**
   * The l2,-1 coefficients of the spherical harmonics
   */
  Vector3 l2_1;

  /**
   * The l2,0 coefficients of the spherical harmonics
   */
  Vector3 l20;

  /**
   * The l2,1 coefficients of the spherical harmonics
   */
  Vector3 l21;

  /**
   * The l2,2 coefficients of the spherical harmonics
   */
  Vector3 lL22;

}; // end of class SphericalHarmonics

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SPHERICAL_HARMONICS_H
