#ifndef BABYLON_MATH_SPHERICAL_HARMONICS_H
#define BABYLON_MATH_SPHERICAL_HARMONICS_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SphericalHarmonics {

public:
  SphericalHarmonics();
  SphericalHarmonics(const SphericalHarmonics& other);
  SphericalHarmonics(SphericalHarmonics&& other);
  SphericalHarmonics& operator=(const SphericalHarmonics& other);
  SphericalHarmonics& operator=(SphericalHarmonics&& other);
  ~SphericalHarmonics();
  SphericalHarmonics copy() const;
  unique_ptr_t<SphericalHarmonics> clone() const;

  void addLight(const Vector3& direction, const Color3& color,
                float deltaSolidAngle);
  void scale(float scale);
  void convertIncidentRadianceToIrradiance();
  void convertIrradianceToLambertianRadiance();

  static SphericalHarmonics
  getsphericalHarmonicsFromPolynomial(const SphericalPolynomial& polynomial);

public:
  Vector3 L00;
  Vector3 L1_1;
  Vector3 L10;
  Vector3 L11;
  Vector3 L2_2;
  Vector3 L2_1;
  Vector3 L20;
  Vector3 L21;
  Vector3 L22;

}; // end of class SphericalHarmonics

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SPHERICAL_HARMONICS_H
