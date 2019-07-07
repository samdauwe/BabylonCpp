#ifndef BABYLON_MATH_SPHERICAL_HARMONICS_H
#define BABYLON_MATH_SPHERICAL_HARMONICS_H

#include <functional>
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
  // https://dickyjim.wordpress.com/2013/09/04/spherical-harmonics-for-beginners/
  // http://silviojemma.com/public/papers/lighting/spherical-harmonic-lighting.pdf
  // https://www.ppsloan.org/publications/StupidSH36.pdf
  // http://cseweb.ucsd.edu/~ravir/papers/envmap/envmap.pdf
  // https://www.ppsloan.org/publications/SHJCGT.pdf
  // https://www.ppsloan.org/publications/shdering.pdf
  // https://google.github.io/filament/Filament.md.html#annex/sphericalharmonics
  // https://patapom.com/blog/SHPortal/
  // https://imdoingitwrong.wordpress.com/2011/04/14/spherical-harmonics-wtf/

  // Using real SH basis:
  //  m>0             m   m
  // y   = sqrt(2) * K * P * cos(m*phi) * cos(theta)
  //  l               l   l
  //
  //  m<0             m   |m|
  // y   = sqrt(2) * K * P * sin(m*phi) * cos(theta)
  //  l               l   l
  //
  //  m=0   0   0
  // y   = K * P * trigono terms
  //  l     l   l
  //
  //  m       (2l + 1)(l - |m|)!
  // K = sqrt(------------------)
  //  l           4pi(l + |m|)!
  //
  // and P by recursion:
  //
  // P00(x) = 1
  // P01(x) = x
  // Pll(x) = (-1^l)(2l - 1)!!(1-x*x)^(1/2)
  //          ((2l - 1)x[Pl-1/m]-(l + m - 1)[Pl-2/m])
  // Plm(x) = ---------------------------------------
  //                         l - m
  // Leaving the trigonometric terms aside we can precompute the constants to :
  static const std::array<float, 9> SH3ylmBasisConstants;

  // cm = cos(m * phi)
  // sm = sin(m * phi)
  // {x,y,z} = {cos(phi)sin(theta), sin(phi)sin(theta), cos(theta)}
  // By recursion on using trigo identities:
  using Vector3Callback = std::function<float(const Vector3& direction)>;
  static const std::array<Vector3Callback, 9> SH3ylmBasisTrigonometricTerms;

  // Wrap the full compute
  static const std::function<float(unsigned int lm, const Vector3& direction)>
    applySH3;

  // Derived from the integration of the a kernel convolution to SH.
  // Great explanation here:
  // https://patapom.com/blog/SHPortal/#about-distant-radiance-and-irradiance-environments
  static const std::array<float, 9> SHCosKernelConvolution;

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
  void scaleInPlace(float scale);

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
   * @brief Integrates the reconstruction coefficients directly in to the SH
   * preventing further required operations at run time.
   *
   * This is simply done by scaling back the SH with Ylm constants parameter.
   * The trigonometric part being applied by the shader at run time.
   */
  void preScaleForRendering();

  /**
   * @brief Constructs a spherical harmonics from an array.
   * @param data defines the 9x3 coefficients (l00, l1-1, l10, l11, l2-2, l2-1,
   * l20, l21, l22)
   * @returns the spherical harmonics
   */
  static SphericalHarmonics FromArray(const std::vector<Float32Array>& data);

  // Keep for references.
  /**
   * @brief Gets the spherical harmonics from polynomial
   * @param polynomial the spherical polynomial
   * @returns the spherical harmonics
   */
  static SphericalHarmonics
  FromPolynomial(const SphericalPolynomial& polynomial);

public:
  /**
   * Defines whether or not the harmonics have been prescaled for rendering
   */
  bool preScaled;

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
  Vector3 l22;

}; // end of class SphericalHarmonics

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SPHERICAL_HARMONICS_H
