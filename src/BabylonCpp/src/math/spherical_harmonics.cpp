#include <babylon/math/spherical_harmonics.h>

#include <cmath>

#include <babylon/math/color3.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

const std::array<float, 9> SphericalHarmonics::SH3ylmBasisConstants{
  std::sqrt(1.f / (4.f * Math::PI)), // l00

  -std::sqrt(3.f / (4.f * Math::PI)), // l1_1
  std::sqrt(3.f / (4.f * Math::PI)),  // l10
  -std::sqrt(3.f / (4.f * Math::PI)), // l11

  std::sqrt(15.f / (4.f * Math::PI)),  // l2_2
  -std::sqrt(15.f / (4.f * Math::PI)), // l2_1
  std::sqrt(5.f / (16.f * Math::PI)),  // l20
  -std::sqrt(15.f / (4.f * Math::PI)), // l21
  std::sqrt(15.f / (16.f * Math::PI)), // l22
};

const std::array<SphericalHarmonics::Vector3Callback, 9>
  SphericalHarmonics::SH3ylmBasisTrigonometricTerms{
    [](const Vector3 & /*direction*/) -> float { return 1.f; }, // l00

    [](const Vector3& direction) -> float { return direction.y; }, // l1_1
    [](const Vector3& direction) -> float { return direction.z; }, // l10
    [](const Vector3& direction) -> float { return direction.x; }, // l11

    [](const Vector3& direction) -> float {
      return direction.x * direction.y;
    }, // l2_2
    [](const Vector3& direction) -> float {
      return direction.y * direction.z;
    }, // l2_1
    [](const Vector3& direction) -> float {
      return 3.f * direction.z * direction.z - 1.f;
    }, // l20
    [](const Vector3& direction) -> float {
      return direction.x * direction.z;
    }, // l21
    [](const Vector3& direction) -> float {
      return direction.x * direction.x - direction.y * direction.y;
    }, // l22
  };

const std::function<float(unsigned int lm, const Vector3& direction)>
  SphericalHarmonics::applySH3
  = [](unsigned int lm, const Vector3& direction) -> float {
  return SH3ylmBasisConstants[lm]
         * SH3ylmBasisTrigonometricTerms[lm](direction);
};

const std::array<float, 9> SphericalHarmonics::SHCosKernelConvolution{
  Math::PI,

  2.f * Math::PI / 3.f,
  2.f * Math::PI / 3.f,
  2.f * Math::PI / 3.f,

  Math::PI / 4.f,
  Math::PI / 4.f,
  Math::PI / 4.f,
  Math::PI / 4.f,
  Math::PI / 4.f,
};

SphericalHarmonics::SphericalHarmonics()
    : preScaled{false}
    , l00{Vector3::Zero()}
    , l1_1{Vector3::Zero()}
    , l10{Vector3::Zero()}
    , l11{Vector3::Zero()}
    , l2_2{Vector3::Zero()}
    , l2_1{Vector3::Zero()}
    , l20{Vector3::Zero()}
    , l21{Vector3::Zero()}
    , l22{Vector3::Zero()}
{
}

SphericalHarmonics::SphericalHarmonics(const SphericalHarmonics& other) = default;

SphericalHarmonics::SphericalHarmonics(SphericalHarmonics&& other) = default;

SphericalHarmonics& SphericalHarmonics::operator=(const SphericalHarmonics& other) = default;

SphericalHarmonics& SphericalHarmonics::operator=(SphericalHarmonics&& other) = default;

SphericalHarmonics::~SphericalHarmonics() = default;

SphericalHarmonics SphericalHarmonics::copy() const
{
  return SphericalHarmonics(*this);
}

std::unique_ptr<SphericalHarmonics> SphericalHarmonics::clone() const
{
  return std::make_unique<SphericalHarmonics>(*this);
}

void SphericalHarmonics::addLight(const Vector3& direction, const Color3& color,
                                  float deltaSolidAngle)
{
  Vector3 colorVector(color.r, color.g, color.b);
  const auto c = colorVector.scale(deltaSolidAngle);

  l00 = l00.add(c.scale(applySH3(0, direction)));

  l1_1 = l1_1.add(c.scale(applySH3(1, direction)));
  l10  = l10.add(c.scale(applySH3(2, direction)));
  l11  = l11.add(c.scale(applySH3(3, direction)));

  l2_2 = l2_2.add(c.scale(applySH3(4, direction)));
  l2_1 = l2_1.add(c.scale(applySH3(5, direction)));
  l20  = l20.add(c.scale(applySH3(6, direction)));
  l21  = l21.add(c.scale(applySH3(7, direction)));
  l22  = l22.add(c.scale(applySH3(8, direction)));
}

void SphericalHarmonics::scaleInPlace(float scale)
{
  l00.scaleInPlace(scale);
  l1_1.scaleInPlace(scale);
  l10.scaleInPlace(scale);
  l11.scaleInPlace(scale);
  l2_2.scaleInPlace(scale);
  l2_1.scaleInPlace(scale);
  l20.scaleInPlace(scale);
  l21.scaleInPlace(scale);
  l22.scaleInPlace(scale);
}

void SphericalHarmonics::convertIncidentRadianceToIrradiance()
{
  // Constant (Band 0)
  l00.scaleInPlace(SHCosKernelConvolution[0]);

  // Linear (Band 1)
  l1_1.scaleInPlace(SHCosKernelConvolution[1]);
  l10.scaleInPlace(SHCosKernelConvolution[2]);
  l11.scaleInPlace(SHCosKernelConvolution[3]);

  // Quadratic (Band 2)
  l2_2.scaleInPlace(SHCosKernelConvolution[4]);
  l2_1.scaleInPlace(SHCosKernelConvolution[5]);
  l20.scaleInPlace(SHCosKernelConvolution[6]);
  l21.scaleInPlace(SHCosKernelConvolution[7]);
  l22.scaleInPlace(SHCosKernelConvolution[8]);
}

void SphericalHarmonics::convertIrradianceToLambertianRadiance()
{
  scaleInPlace(1.f / Math::PI);

  // The resultant SH now represents outgoing radiance, so includes the Lambert
  // 1/pi normalisation factor but without albedo (rho) applied
  // (The pixel shader must apply albedo after texture fetches, etc).
}

void SphericalHarmonics::preScaleForRendering()
{
  preScaled = true;

  l00.scaleInPlace(SH3ylmBasisConstants[0]);

  l1_1.scaleInPlace(SH3ylmBasisConstants[1]);
  l10.scaleInPlace(SH3ylmBasisConstants[2]);
  l11.scaleInPlace(SH3ylmBasisConstants[3]);

  l2_2.scaleInPlace(SH3ylmBasisConstants[4]);
  l2_1.scaleInPlace(SH3ylmBasisConstants[5]);
  l20.scaleInPlace(SH3ylmBasisConstants[6]);
  l21.scaleInPlace(SH3ylmBasisConstants[7]);
  l22.scaleInPlace(SH3ylmBasisConstants[8]);
}

SphericalHarmonics
SphericalHarmonics::FromArray(const std::vector<Float32Array>& data)
{
  SphericalHarmonics sh;
  if (data.size() < 9) {
    return sh;
  }

  Vector3::FromArrayToRef(data[0], 0, sh.l00);
  Vector3::FromArrayToRef(data[1], 0, sh.l1_1);
  Vector3::FromArrayToRef(data[2], 0, sh.l10);
  Vector3::FromArrayToRef(data[3], 0, sh.l11);
  Vector3::FromArrayToRef(data[4], 0, sh.l2_2);
  Vector3::FromArrayToRef(data[5], 0, sh.l2_1);
  Vector3::FromArrayToRef(data[6], 0, sh.l20);
  Vector3::FromArrayToRef(data[7], 0, sh.l21);
  Vector3::FromArrayToRef(data[8], 0, sh.l22);
  return sh;
}

SphericalHarmonics
SphericalHarmonics::FromPolynomial(const SphericalPolynomial& polynomial)
{
  SphericalHarmonics result;

  result.l00 = polynomial.xx.scale(0.376127f)
                 .add(polynomial.yy.scale(0.376127f))
                 .add(polynomial.zz.scale(0.376126f));
  result.l1_1 = polynomial.y.scale(0.977204f);
  result.l10  = polynomial.z.scale(0.977204f);
  result.l11  = polynomial.x.scale(0.977204f);
  result.l2_2 = polynomial.xy.scale(1.16538f);
  result.l2_1 = polynomial.yz.scale(1.16538f);
  result.l20  = polynomial.zz.scale(1.34567f)
                 .subtract(polynomial.xx.scale(0.672834f))
                 .subtract(polynomial.yy.scale(0.672834f));
  result.l21 = polynomial.zx.scale(1.16538f);
  result.l22
    = polynomial.xx.scale(1.16538f).subtract(polynomial.yy.scale(1.16538f));

  result.l1_1.scaleInPlace(-1.f);
  result.l11.scaleInPlace(-1.f);
  result.l2_1.scaleInPlace(-1.f);
  result.l21.scaleInPlace(-1.f);

  result.scaleInPlace(Math::PI);

  return result;
}

} // end of namespace BABYLON
