#include <babylon/math/spherical_harmonics.h>

#include <babylon/math/color3.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

SphericalHarmonics::SphericalHarmonics()
    : l00{Vector3::Zero()}
    , l1_1{Vector3::Zero()}
    , l10{Vector3::Zero()}
    , l11{Vector3::Zero()}
    , l2_2{Vector3::Zero()}
    , l2_1{Vector3::Zero()}
    , l20{Vector3::Zero()}
    , l21{Vector3::Zero()}
    , lL22{Vector3::Zero()}
{
}

SphericalHarmonics::SphericalHarmonics(const SphericalHarmonics& other)
    : l00{other.l00}
    , l1_1{other.l1_1}
    , l10{other.l10}
    , l11{other.l11}
    , l2_2{other.l2_2}
    , l2_1{other.l2_1}
    , l20{other.l20}
    , l21{other.l21}
    , lL22{other.lL22}
{
}

SphericalHarmonics::SphericalHarmonics(SphericalHarmonics&& other)
    : l00{std::move(other.l00)}
    , l1_1{std::move(other.l1_1)}
    , l10{std::move(other.l10)}
    , l11{std::move(other.l11)}
    , l2_2{std::move(other.l2_2)}
    , l2_1{std::move(other.l2_1)}
    , l20{std::move(other.l20)}
    , l21{std::move(other.l21)}
    , lL22{std::move(other.lL22)}
{
}

SphericalHarmonics& SphericalHarmonics::
operator=(const SphericalHarmonics& other)
{
  if (&other != this) {
    l00  = other.l00;
    l1_1 = other.l1_1;
    l10  = other.l10;
    l11  = other.l11;
    l2_2 = other.l2_2;
    l2_1 = other.l2_1;
    l20  = other.l20;
    l21  = other.l21;
    lL22 = other.lL22;
  }

  return *this;
}

SphericalHarmonics& SphericalHarmonics::operator=(SphericalHarmonics&& other)
{
  if (&other != this) {
    l00  = std::move(other.l00);
    l1_1 = std::move(other.l1_1);
    l10  = std::move(other.l10);
    l11  = std::move(other.l11);
    l2_2 = std::move(other.l2_2);
    l2_1 = std::move(other.l2_1);
    l20  = std::move(other.l20);
    l21  = std::move(other.l21);
    lL22 = std::move(other.lL22);
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
  return std::make_unique<SphericalHarmonics>(*this);
}

void SphericalHarmonics::addLight(const Vector3& direction, const Color3& color,
                                  float deltaSolidAngle)
{
  const Vector3 colorVector(color.r, color.g, color.b);
  const Vector3 c = colorVector.scale(deltaSolidAngle);

  l00 = l00.add(c.scale(0.282095f));

  l1_1 = l1_1.add(c.scale(0.488603f * direction.y));
  l10  = l10.add(c.scale(0.488603f * direction.z));
  l11  = l11.add(c.scale(0.488603f * direction.x));

  l2_2 = l2_2.add(c.scale(1.092548f * direction.x * direction.y));
  l2_1 = l2_1.add(c.scale(1.092548f * direction.y * direction.z));
  l21  = l21.add(c.scale(1.092548f * direction.x * direction.z));

  l20  = l20.add(c.scale(0.315392f * (3.f * direction.z * direction.z - 1.f)));
  lL22 = lL22.add(c.scale(
    0.546274f * (direction.x * direction.x - direction.y * direction.y)));
}

void SphericalHarmonics::scale(float scale)
{
  l00  = l00.scale(scale);
  l1_1 = l1_1.scale(scale);
  l10  = l10.scale(scale);
  l11  = l11.scale(scale);
  l2_2 = l2_2.scale(scale);
  l2_1 = l2_1.scale(scale);
  l20  = l20.scale(scale);
  l21  = l21.scale(scale);
  lL22 = lL22.scale(scale);
}

void SphericalHarmonics::convertIncidentRadianceToIrradiance()
{
  // Constant (Band 0)
  l00 = l00.scale(3.141593f);

  // Linear (Band 1)
  l1_1 = l1_1.scale(2.094395f);
  l10  = l10.scale(2.094395f);
  l11  = l11.scale(2.094395f);

  // Quadratic (Band 2)
  l2_2 = l2_2.scale(0.785398f);
  l2_1 = l2_1.scale(0.785398f);
  l20  = l20.scale(0.785398f);
  l21  = l21.scale(0.785398f);
  lL22 = lL22.scale(0.785398f);
}

void SphericalHarmonics::convertIrradianceToLambertianRadiance()
{
  scale(1.f / Math::PI);

  // The resultant SH now represents outgoing radiance, so includes the Lambert
  // 1/pi normalisation factor but without albedo (rho) applied
  // (The pixel shader must apply albedo after texture fetches, etc).
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
  result.lL22
    = polynomial.xx.scale(1.16538f).subtract(polynomial.yy.scale(1.16538f));

  result.scale(Math::PI);

  return result;
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
  Vector3::FromArrayToRef(data[8], 0, sh.lL22);
  return sh;
}

} // end of namespace BABYLON
