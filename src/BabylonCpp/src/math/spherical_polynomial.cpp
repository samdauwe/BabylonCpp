#include <babylon/math/spherical_polynomial.h>

#include <babylon/math/color3.h>
#include <babylon/math/spherical_harmonics.h>

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
{
}

SphericalPolynomial::SphericalPolynomial(const SphericalPolynomial& other)
    : x{other.x}
    , y{other.y}
    , z{other.z}
    , xx{other.xx}
    , yy{other.yy}
    , zz{other.zz}
    , xy{other.xy}
    , yz{other.yz}
    , zx{other.zx}
{
}

SphericalPolynomial::SphericalPolynomial(SphericalPolynomial&& other)
    : x{::std::move(other.x)}
    , y{::std::move(other.y)}
    , z{::std::move(other.z)}
    , xx{::std::move(other.xx)}
    , yy{::std::move(other.yy)}
    , zz{::std::move(other.zz)}
    , xy{::std::move(other.xy)}
    , yz{::std::move(other.yz)}
    , zx{::std::move(other.zx)}
{
}

SphericalPolynomial& SphericalPolynomial::
operator=(const SphericalPolynomial& other)
{
  if (&other != this) {
    x  = other.x;
    y  = other.y;
    z  = other.z;
    xx = other.xx;
    yy = other.yy;
    zz = other.zz;
    xy = other.xy;
    yz = other.yz;
    zx = other.zx;
  }

  return *this;
}

SphericalPolynomial& SphericalPolynomial::operator=(SphericalPolynomial&& other)
{
  if (&other != this) {
    x  = ::std::move(other.x);
    y  = ::std::move(other.y);
    z  = ::std::move(other.z);
    xx = ::std::move(other.xx);
    yy = ::std::move(other.yy);
    zz = ::std::move(other.zz);
    xy = ::std::move(other.xy);
    yz = ::std::move(other.yz);
    zx = ::std::move(other.zx);
  }

  return *this;
}

SphericalPolynomial::~SphericalPolynomial()
{
}

SphericalPolynomial SphericalPolynomial::copy() const
{
  return SphericalPolynomial(*this);
}

unique_ptr_t<SphericalPolynomial> SphericalPolynomial::clone() const
{
  return ::std::make_unique<SphericalPolynomial>(*this);
}

void SphericalPolynomial::addAmbient(const Color3& color)
{
  const Vector3 colorVector(color.r, color.g, color.b);
  xx = xx.add(colorVector);
  yy = yy.add(colorVector);
  zz = zz.add(colorVector);
}

void SphericalPolynomial::scale(float iScale)
{
  x  = x.scale(iScale);
  y  = y.scale(iScale);
  z  = z.scale(iScale);
  xx = xx.scale(iScale);
  yy = yy.scale(iScale);
  zz = zz.scale(iScale);
  yz = yz.scale(iScale);
  zx = zx.scale(iScale);
  xy = xy.scale(iScale);
}

SphericalPolynomial
SphericalPolynomial::FromHarmonics(const SphericalHarmonics& harmonics)
{
  SphericalPolynomial result;

  result.x = harmonics.l11.scale(1.02333f);
  result.y = harmonics.l1_1.scale(1.02333f);
  result.z = harmonics.l10.scale(1.02333f);

  result.xx = harmonics.l00.scale(0.886277f)
                .subtract(harmonics.l20.scale(0.247708f))
                .add(harmonics.lL22.scale(0.429043f));
  result.yy = harmonics.l00.scale(0.886277f)
                .subtract(harmonics.l20.scale(0.247708f))
                .subtract(harmonics.lL22.scale(0.429043f));
  result.zz
    = harmonics.l00.scale(0.886277f).add(harmonics.l20.scale(0.495417f));

  result.yz = harmonics.l2_1.scale(0.858086f);
  result.zx = harmonics.l21.scale(0.858086f);
  result.xy = harmonics.l2_2.scale(0.858086f);

  result.scale(1.f / Math::PI);

  return result;

  return result;
}

SphericalPolynomial
SphericalPolynomial::FromArray(const vector_t<Float32Array>& data)
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
