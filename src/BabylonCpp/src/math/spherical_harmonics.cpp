#include <babylon/math/spherical_harmonics.h>

#include <babylon/math/color3.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

SphericalHarmonics::SphericalHarmonics()
    : L00{Vector3::Zero()}
    , L1_1{Vector3::Zero()}
    , L10{Vector3::Zero()}
    , L11{Vector3::Zero()}
    , L2_2{Vector3::Zero()}
    , L2_1{Vector3::Zero()}
    , L20{Vector3::Zero()}
    , L21{Vector3::Zero()}
    , L22{Vector3::Zero()}
{
}

SphericalHarmonics::SphericalHarmonics(const SphericalHarmonics& other)
    : L00{other.L00}
    , L1_1{other.L1_1}
    , L10{other.L10}
    , L11{other.L11}
    , L2_2{other.L2_2}
    , L2_1{other.L2_1}
    , L20{other.L20}
    , L21{other.L21}
    , L22{other.L22}
{
}

SphericalHarmonics::SphericalHarmonics(SphericalHarmonics&& other)
    : L00{::std::move(other.L00)}
    , L1_1{::std::move(other.L1_1)}
    , L10{::std::move(other.L10)}
    , L11{::std::move(other.L11)}
    , L2_2{::std::move(other.L2_2)}
    , L2_1{::std::move(other.L2_1)}
    , L20{::std::move(other.L20)}
    , L21{::std::move(other.L21)}
    , L22{::std::move(other.L22)}
{
}

SphericalHarmonics& SphericalHarmonics::
operator=(const SphericalHarmonics& other)
{
  if (&other != this) {
    L00  = other.L00;
    L1_1 = other.L1_1;
    L10  = other.L10;
    L11  = other.L11;
    L2_2 = other.L2_2;
    L2_1 = other.L2_1;
    L20  = other.L20;
    L21  = other.L21;
    L22  = other.L22;
  }

  return *this;
}

SphericalHarmonics& SphericalHarmonics::operator=(SphericalHarmonics&& other)
{
  if (&other != this) {
    L00  = ::std::move(other.L00);
    L1_1 = ::std::move(other.L1_1);
    L10  = ::std::move(other.L10);
    L11  = ::std::move(other.L11);
    L2_2 = ::std::move(other.L2_2);
    L2_1 = ::std::move(other.L2_1);
    L20  = ::std::move(other.L20);
    L21  = ::std::move(other.L21);
    L22  = ::std::move(other.L22);
  }

  return *this;
}

SphericalHarmonics::~SphericalHarmonics()
{
}

SphericalHarmonics SphericalHarmonics::copy() const
{
  return SphericalHarmonics(*this);
}

std::unique_ptr<SphericalHarmonics> SphericalHarmonics::clone() const
{
  return ::std::make_unique<SphericalHarmonics>(*this);
}

void SphericalHarmonics::addLight(const Vector3& direction, const Color3& color,
                                  float deltaSolidAngle)
{
  const Vector3 colorVector(color.r, color.g, color.b);
  const Vector3 c = colorVector.scale(deltaSolidAngle);

  L00 = L00.add(c.scale(0.282095f));

  L1_1 = L1_1.add(c.scale(0.488603f * direction.y));
  L10  = L10.add(c.scale(0.488603f * direction.z));
  L11  = L11.add(c.scale(0.488603f * direction.x));

  L2_2 = L2_2.add(c.scale(1.092548f * direction.x * direction.y));
  L2_1 = L2_1.add(c.scale(1.092548f * direction.y * direction.z));
  L21  = L21.add(c.scale(1.092548f * direction.x * direction.z));

  L20 = L20.add(c.scale(0.315392f * (3.f * direction.z * direction.z - 1.f)));
  L22 = L22.add(c.scale(
    0.546274f * (direction.x * direction.x - direction.y * direction.y)));
}

void SphericalHarmonics::scale(float scale)
{
  L00  = L00.scale(scale);
  L1_1 = L1_1.scale(scale);
  L10  = L10.scale(scale);
  L11  = L11.scale(scale);
  L2_2 = L2_2.scale(scale);
  L2_1 = L2_1.scale(scale);
  L20  = L20.scale(scale);
  L21  = L21.scale(scale);
  L22  = L22.scale(scale);
}

void SphericalHarmonics::convertIncidentRadianceToIrradiance()
{
  // Convert from incident radiance (Li) to irradiance (E) by applying
  // convolution with the cosine-weighted hemisphere.
  //
  //      E_lm = A_l * L_lm
  //
  // In spherical harmonics this convolution amounts to scaling factors for each
  // frequency band.
  // This corresponds to equation 5 in "An Efficient Representation for
  // Irradiance Environment Maps", where
  // the scaling factors are given in equation 9.

  // Constant (Band 0)
  L00 = L00.scale(3.141593f);

  // Linear (Band 1)
  L1_1 = L1_1.scale(2.094395f);
  L10  = L10.scale(2.094395f);
  L11  = L11.scale(2.094395f);

  // Quadratic (Band 2)
  L2_2 = L2_2.scale(0.785398f);
  L2_1 = L2_1.scale(0.785398f);
  L20  = L20.scale(0.785398f);
  L21  = L21.scale(0.785398f);
  L22  = L22.scale(0.785398f);
}

void SphericalHarmonics::convertIrradianceToLambertianRadiance()
{
  // Convert from irradiance to outgoing radiance for Lambertian BDRF, suitable
  // for efficient shader evaluation.
  //      L = (1/pi) * E * rho
  //
  // This is done by an additional scale by 1/pi, so is a fairly trivial
  // operation but important conceptually.

  scale(1.f / Math::PI);

  // The resultant SH now represents outgoing radiance, so includes the Lambert
  // 1/pi normalisation factor but without albedo (rho) applied
  // (The pixel shader must apply albedo after texture fetches, etc).
}

SphericalHarmonics SphericalHarmonics::getsphericalHarmonicsFromPolynomial(
  const SphericalPolynomial& polynomial)
{
  SphericalHarmonics result;

  result.L00 = polynomial.xx.scale(0.376127f)
                 .add(polynomial.yy.scale(0.376127f))
                 .add(polynomial.zz.scale(0.376126f));
  result.L1_1 = polynomial.y.scale(0.977204f);
  result.L10  = polynomial.z.scale(0.977204f);
  result.L11  = polynomial.x.scale(0.977204f);
  result.L2_2 = polynomial.xy.scale(1.16538f);
  result.L2_1 = polynomial.yz.scale(1.16538f);
  result.L20  = polynomial.zz.scale(1.34567f)
                 .subtract(polynomial.xx.scale(0.672834f))
                 .subtract(polynomial.yy.scale(0.672834f));
  result.L21 = polynomial.zx.scale(1.16538f);
  result.L22
    = polynomial.xx.scale(1.16538f).subtract(polynomial.yy.scale(1.16538f));

  result.scale(Math::PI);

  return result;
}

} // end of namespace BABYLON
