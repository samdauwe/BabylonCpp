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
    : x{std::move(other.x)}
    , y{std::move(other.y)}
    , z{std::move(other.z)}
    , xx{std::move(other.xx)}
    , yy{std::move(other.yy)}
    , zz{std::move(other.zz)}
    , xy{std::move(other.xy)}
    , yz{std::move(other.yz)}
    , zx{std::move(other.zx)}
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
    std::swap(x, other.x);
    std::swap(y, other.y);
    std::swap(z, other.z);
    std::swap(xx, other.xx);
    std::swap(yy, other.yy);
    std::swap(zz, other.zz);
    std::swap(xy, other.xy);
    std::swap(yz, other.yz);
    std::swap(zx, other.zx);
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

std::unique_ptr<SphericalPolynomial> SphericalPolynomial::clone() const
{
  return std_util::make_unique<SphericalPolynomial>(*this);
}

void SphericalPolynomial::addAmbient(const Color3& color)
{
  Vector3 colorVector(color.r, color.g, color.b);
  xx = xx.add(colorVector);
  yy = yy.add(colorVector);
  zz = zz.add(colorVector);
}

SphericalPolynomial SphericalPolynomial::getSphericalPolynomialFromHarmonics(
  const SphericalHarmonics& harmonics)
{
  SphericalPolynomial result;

  result.x = harmonics.L11.scale(1.02333f);
  result.y = harmonics.L1_1.scale(1.02333f);
  result.z = harmonics.L10.scale(1.02333f);

  result.xx = harmonics.L00.scale(0.886277f)
                .subtract(harmonics.L20.scale(0.247708f))
                .add(harmonics.L22.scale(0.429043f));
  result.yy = harmonics.L00.scale(0.886277f)
                .subtract(harmonics.L20.scale(0.247708f))
                .subtract(harmonics.L22.scale(0.429043f));
  result.zz
    = harmonics.L00.scale(0.886277f).add(harmonics.L20.scale(0.495417f));

  result.yz = harmonics.L2_1.scale(0.858086f);
  result.zx = harmonics.L21.scale(0.858086f);
  result.xy = harmonics.L2_2.scale(0.858086f);

  return result;
}

} // end of namespace BABYLON
