#include <babylon/math/color3.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>
#include <babylon/math/color4.h>
#include <babylon/math/scalar.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Color3::Color3(float red, float green, float blue) : r{red}, g{green}, b{blue}
{
}

Color3::Color3(const Color3& otherColor)
    : r{otherColor.r}, g{otherColor.g}, b{otherColor.b}
{
}

Color3::Color3(Color3&& otherColor)
    : r{::std::move(otherColor.r)}
    , g{::std::move(otherColor.g)}
    , b{::std::move(otherColor.b)}
{
}

Color3& Color3::operator=(const Color3& otherColor)
{
  if (&otherColor != this) {
    r = otherColor.r;
    g = otherColor.g;
    b = otherColor.b;
  }

  return *this;
}

Color3& Color3::operator=(Color3&& otherColor)
{
  if (&otherColor != this) {
    r = ::std::move(otherColor.r);
    g = ::std::move(otherColor.g);
    b = ::std::move(otherColor.b);
  }

  return *this;
}

Color3::~Color3()
{
}

Color3 Color3::copy() const
{
  return Color3(*this);
}

unique_ptr_t<Color3> Color3::clone() const
{
  return ::std::make_unique<Color3>(*this);
}

string_t Color3::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

const char* Color3::getClassName() const
{
  return "Color3";
}

int Color3::getHashCode() const
{
  float hash = r;
  hash       = ::std::pow((hash * 397), g);
  hash       = ::std::pow((hash * 397), b);
  return static_cast<int>(hash);
}

/**  Operators **/
const Color3& Color3::toArray(Float32Array& array, unsigned int index) const
{
  if (array.size() < index + 3) {
    array.resize(index + 3);
  }

  array[index]     = r;
  array[index + 1] = g;
  array[index + 2] = b;

  return *this;
}

Color4 Color3::toColor4(float alpha) const
{
  return Color4(r, g, b, alpha);
}

Float32Array Color3::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

float Color3::toLuminance() const
{
  return r * 0.3f + g * 0.59f + b * 0.11f;
}

Color3 Color3::multiply(const Color3& otherColor) const
{
  return Color3(r * otherColor.r, g * otherColor.g, b * otherColor.b);
}

const Color3& Color3::multiplyToRef(const Color3& otherColor,
                                    Color3& result) const
{
  result.r = r * otherColor.r;
  result.g = g * otherColor.g;
  result.b = b * otherColor.b;

  return *this;
}

bool Color3::equals(const Color3& otherColor) const
{
  return stl_util::almost_equal(r, otherColor.r)
         && stl_util::almost_equal(g, otherColor.g)
         && stl_util::almost_equal(b, otherColor.b);
}

bool Color3::equalsFloats(float red, float green, float blue) const
{
  return stl_util::almost_equal(r, red) && stl_util::almost_equal(g, green)
         && stl_util::almost_equal(b, blue);
}

Color3 Color3::scale(float iscale) const
{
  return Color3(r * iscale, g * iscale, b * iscale);
}

const Color3& Color3::scaleToRef(float iscale, Color3& result) const
{
  result.r = r * iscale;
  result.g = g * iscale;
  result.b = b * iscale;

  return *this;
}

Color3 Color3::add(const Color3& otherColor) const
{
  return Color3(r + otherColor.r, g + otherColor.g, b + otherColor.b);
}

const Color3& Color3::addToRef(const Color3& otherColor, Color3& result) const
{
  result.r = r + otherColor.r;
  result.g = g + otherColor.g;
  result.b = b + otherColor.b;

  return *this;
}

Color3 Color3::subtract(const Color3& otherColor) const
{
  return Color3(r - otherColor.r, g - otherColor.g, b - otherColor.b);
}

const Color3& Color3::subtractToRef(const Color3& otherColor,
                                    Color3& result) const
{
  result.r = r - otherColor.r;
  result.g = g - otherColor.g;
  result.b = b - otherColor.b;

  return *this;
}

Color3& Color3::copyFrom(const Color3& source)
{
  r = source.r;
  g = source.g;
  b = source.b;

  return *this;
}

Color3& Color3::copyFromFloats(float red, float green, float blue)
{
  r = red;
  g = green;
  b = blue;

  return *this;
}

Color3& Color3::set(float red, float green, float blue)
{
  return copyFromFloats(red, green, blue);
}

string_t Color3::toHexString() const
{
  const int intR = static_cast<int>(r * 255) | 0;
  const int intG = static_cast<int>(g * 255) | 0;
  const int intB = static_cast<int>(b * 255) | 0;

  std::ostringstream ostream;
  ostream << "#" << Scalar::ToHex(intR) << Scalar::ToHex(intG)
          << Scalar::ToHex(intB);
  return ostream.str();
}

Color3 Color3::toLinearSpace()
{
  Color3 convertedColor;
  toLinearSpaceToRef(convertedColor);
  return convertedColor;
}

Color3& Color3::toLinearSpaceToRef(Color3& convertedColor)
{
  convertedColor.r = ::std::pow(r, Math::ToLinearSpace);
  convertedColor.g = ::std::pow(g, Math::ToLinearSpace);
  convertedColor.b = ::std::pow(b, Math::ToLinearSpace);

  return *this;
}

Color3 Color3::toGammaSpace()
{
  Color3 convertedColor;
  toGammaSpaceToRef(convertedColor);
  return convertedColor;
}

Color3& Color3::toGammaSpaceToRef(Color3& convertedColor)
{
  convertedColor.r = ::std::pow(r, Math::ToGammaSpace);
  convertedColor.g = ::std::pow(g, Math::ToGammaSpace);
  convertedColor.b = ::std::pow(b, Math::ToGammaSpace);

  return *this;
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Color3& color)
{
  os << "{\"R\":" << color.r << ",\"G\":" << color.g << ",\"B\":" << color.b
     << "}";
  return os;
}

bool operator==(const Color3& lhs, const Color3& rhs)
{
  return lhs.equals(rhs);
}

bool operator!=(const Color3& lhs, const Color3& rhs)
{
  return !(lhs == rhs);
}

Color3 Color3::operator+(const Color3& otherColor) const
{
  return add(otherColor);
}

Color3 Color3::operator-(const Color3& otherColor) const
{
  return subtract(otherColor);
}

Color3 Color3::operator*(float iscale) const
{
  return scale(iscale);
}

Color3 Color3::operator*(const Color3& otherColor) const
{
  return multiply(otherColor);
}

/** Statics **/
Color3 Color3::FromHexString(const string_t& hex)
{
  if (hex.substr(0, 1) != "#" || hex.size() != 7) {
    return Color3(0.f, 0.f, 0.f);
  }

  return Color3::FromInts(
    static_cast<int>(strtol(hex.substr(1, 2).c_str(), NULL, 16)),
    static_cast<int>(strtol(hex.substr(3, 2).c_str(), NULL, 16)),
    static_cast<int>(strtol(hex.substr(5, 2).c_str(), NULL, 16)));
}

Color3 Color3::FromArray(const Float32Array& array, unsigned int offset)
{
  return Color3(array[offset], array[offset + 1], array[offset + 2]);
}

Color3 Color3::FromInt(int rgb)
{
  return Color3::FromInts((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255);
}

Color3 Color3::FromInts(int r, int g, int b)
{
  return Color3(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f);
}

Color3 Color3::Lerp(const Color3& start, const Color3& end, float amount)
{
  const float r = start.r + ((end.r - start.r) * amount);
  const float g = start.g + ((end.g - start.g) * amount);
  const float b = start.b + ((end.b - start.b) * amount);

  return Color3(r, g, b);
}

Color3 Color3::Red()
{
  return Color3(1.f, 0.f, 0.f);
}

Color3 Color3::Green()
{
  return Color3(0.f, 1.f, 0.f);
}

Color3 Color3::Blue()
{
  return Color3(0.f, 0.f, 1.f);
}

Color3 Color3::Black()
{
  return Color3(0.f, 0.f, 0.f);
}

Color3 Color3::White()
{
  return Color3(1.f, 1.f, 1.f);
}

Color3 Color3::Purple()
{
  return Color3(0.5f, 0.f, 0.5f);
}

Color3 Color3::Magenta()
{
  return Color3(1.f, 0.f, 1.f);
}

Color3 Color3::Yellow()
{
  return Color3(1.f, 1.f, 0.f);
}

Color3 Color3::Cyan()
{
  return Color3(0.f, 1.f, 1.f);
}

Color3 Color3::Gray()
{
  return Color3(0.5f, 0.5f, 0.5f);
}

Color3 Color3::Teal()
{
  return Color3(0.f, 1.f, 1.f);
}

Color3 Color3::Random()
{
  return Color3(Math::random(), Math::random(), Math::random());
}

} // end of namespace BABYLON
