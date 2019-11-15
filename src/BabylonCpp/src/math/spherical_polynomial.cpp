#include <babylon/math/spherical_polynomial.h>

#include <babylon/math/color3.h>

namespace BABYLON {

SphericalPolynomial::SphericalPolynomial()
    : x{Vector3::Zero()}
    , y{Vector3::Zero()}
    , z{Vector3::Zero()}
    , xx{Vector3::Zero()}
    , yy{Vector3::Zero()}
    , zz{Vector3::Zero()}
    , xy{Vector3::Zero()}
    , yz{Vector3::Zero()}
    , zx{Vector3::Zero()}
    , _harmonics{std::nullopt}
{
}

SphericalPolynomial::SphericalPolynomial(const SphericalPolynomial& other) = default;

SphericalPolynomial::SphericalPolynomial(SphericalPolynomial&& other) = default;

SphericalPolynomial& SphericalPolynomial::operator=(const SphericalPolynomial& other) = default;

SphericalPolynomial& SphericalPolynomial::operator=(SphericalPolynomial&& other) = default;

SphericalPolynomial::~SphericalPolynomial() = default;

SphericalPolynomial SphericalPolynomial::copy() const
{
  return SphericalPolynomial(*this);
}

std::unique_ptr<SphericalPolynomial> SphericalPolynomial::clone() const
{
  return std::make_unique<SphericalPolynomial>(*this);
}

SphericalHarmonics& SphericalPolynomial::preScaledHarmonics()
{
  if (!_harmonics.has_value()) {
    _harmonics = SphericalHarmonics::FromPolynomial(*this);
  }
  if (!_harmonics->preScaled) {
    _harmonics->preScaleForRendering();
  }
  return *_harmonics;
}

void SphericalPolynomial::addAmbient(const Color3& color)
{
  const Vector3 colorVector(color.r, color.g, color.b);
  xx = xx.add(colorVector);
  yy = yy.add(colorVector);
  zz = zz.add(colorVector);
}

void SphericalPolynomial::scaleInPlace(float scale)
{
  x.scaleInPlace(scale);
  y.scaleInPlace(scale);
  z.scaleInPlace(scale);
  xx.scaleInPlace(scale);
  yy.scaleInPlace(scale);
  zz.scaleInPlace(scale);
  yz.scaleInPlace(scale);
  zx.scaleInPlace(scale);
  xy.scaleInPlace(scale);
}

SphericalPolynomial
SphericalPolynomial::FromHarmonics(const SphericalHarmonics& harmonics)
{
  SphericalPolynomial result;
  result._harmonics = harmonics;

  result.x = harmonics.l11.scale(1.02333f).scale(-1.f);
  result.y = harmonics.l1_1.scale(1.02333f).scale(-1.f);
  result.z = harmonics.l10.scale(1.02333f);

  result.xx = harmonics.l00.scale(0.886277f)
                .subtract(harmonics.l20.scale(0.247708f))
                .add(harmonics.l22.scale(0.429043f));
  result.yy = harmonics.l00.scale(0.886277f)
                .subtract(harmonics.l20.scale(0.247708f))
                .subtract(harmonics.l22.scale(0.429043f));
  result.zz
    = harmonics.l00.scale(0.886277f).add(harmonics.l20.scale(0.495417f));

  result.yz = harmonics.l2_1.scale(0.858086f).scale(-1.f);
  result.zx = harmonics.l21.scale(0.858086f).scale(-1.f);
  result.xy = harmonics.l2_2.scale(0.858086f);

  result.scaleInPlace(1.f / Math::PI);

  return result;
}

SphericalPolynomial
SphericalPolynomial::FromArray(const std::vector<Float32Array>& data)
{
  SphericalPolynomial sp;
  if (data.size() < 9) {
    return sp;
  }

  Vector3::FromArrayToRef(data[0], 0, sp.x);
  Vector3::FromArrayToRef(data[1], 0, sp.y);
  Vector3::FromArrayToRef(data[2], 0, sp.z);
  Vector3::FromArrayToRef(data[3], 0, sp.xx);
  Vector3::FromArrayToRef(data[4], 0, sp.yy);
  Vector3::FromArrayToRef(data[5], 0, sp.zz);
  Vector3::FromArrayToRef(data[6], 0, sp.yz);
  Vector3::FromArrayToRef(data[7], 0, sp.zx);
  Vector3::FromArrayToRef(data[8], 0, sp.xy);
  return sp;
}

} // end of namespace BABYLON
