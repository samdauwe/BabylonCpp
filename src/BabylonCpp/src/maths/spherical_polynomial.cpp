#include <babylon/maths/spherical_polynomial.h>

#include <babylon/maths/color3.h>
#include <babylon/maths/tmp_vectors.h>

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
  TmpVectors::Vector3Array[0].copyFromFloats(color.r, color.g, color.b);
  auto& colorVector = TmpVectors::Vector3Array[0];
  xx.addInPlace(colorVector);
  yy.addInPlace(colorVector);
  zz.addInPlace(colorVector);
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

SphericalPolynomial& SphericalPolynomial::updateFromHarmonics(const SphericalHarmonics& harmonics)
{
  _harmonics = harmonics;

  x.copyFrom(harmonics.l11);
  x.scaleInPlace(1.02333f).scaleInPlace(-1.f);
  y.copyFrom(harmonics.l1_1);
  y.scaleInPlace(1.02333f).scaleInPlace(-1.f);
  z.copyFrom(harmonics.l10);
  z.scaleInPlace(1.02333f);

  xx.copyFrom(harmonics.l00);
  TmpVectors::Vector3Array[0].copyFrom(harmonics.l20).scaleInPlace(0.247708f);
  TmpVectors::Vector3Array[1].copyFrom(harmonics.l22).scaleInPlace(0.429043f);
  xx.scaleInPlace(0.886277f)
    .subtractInPlace(TmpVectors::Vector3Array[0])
    .addInPlace(TmpVectors::Vector3Array[1]);
  yy.copyFrom(harmonics.l00);
  yy.scaleInPlace(0.886277f)
    .subtractInPlace(TmpVectors::Vector3Array[0])
    .subtractInPlace(TmpVectors::Vector3Array[1]);
  zz.copyFrom(harmonics.l00);
  TmpVectors::Vector3Array[0].copyFrom(harmonics.l20).scaleInPlace(0.495417f);
  zz.scaleInPlace(0.886277f).addInPlace(TmpVectors::Vector3Array[0]);

  yz.copyFrom(harmonics.l2_1);
  yz.scaleInPlace(0.858086f).scaleInPlace(-1.f);
  zx.copyFrom(harmonics.l21);
  zx.scaleInPlace(0.858086f).scaleInPlace(-1.f);
  xy.copyFrom(harmonics.l2_2);
  xy.scaleInPlace(0.858086f);

  scaleInPlace(1.f / Math::PI);

  return *this;
}

SphericalPolynomial SphericalPolynomial::FromHarmonics(const SphericalHarmonics& harmonics)
{
  SphericalPolynomial result;
  return result.updateFromHarmonics(harmonics);
}

SphericalPolynomial SphericalPolynomial::FromArray(const std::vector<Float32Array>& data)
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
