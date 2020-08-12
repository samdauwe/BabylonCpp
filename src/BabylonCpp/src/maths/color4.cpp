#include <babylon/maths/color4.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/scalar.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

Color4::Color4(float red, float green, float blue, float alpha)
    : r{red}, g{green}, b{blue}, a{alpha}
{
}

Color4::Color4(const Color3& otherColor) : r{otherColor.r}, g{otherColor.g}, b{otherColor.b}, a{1.f}
{
}

Color4::Color4(const Color4&) = default;

Color4::Color4(Color3&& otherColor)
{
  r = otherColor.r;
  g = otherColor.g;
  b = otherColor.b;
  a = 1.f;
}

Color4::Color4(Color4&&) = default;

Color4& Color4::operator=(const Color3& otherColor)
{
  r = otherColor.r;
  g = otherColor.g;
  b = otherColor.b;
  a = 1.f;

  return *this;
}

Color4& Color4::operator=(const Color4& otherColor) = default;

Color4& Color4::operator=(Color3&& otherColor)
{
  r = std::move(otherColor.r);
  g = std::move(otherColor.g);
  b = std::move(otherColor.b);
  a = 1.f;

  return *this;
}

Color4& Color4::operator=(Color4&& otherColor) = default;

Color4::~Color4() = default;

Color4 Color4::copy() const
{
  return Color4(*this);
}

std::unique_ptr<Color4> Color4::clone() const
{
  return std::make_unique<Color4>(*this);
}

const char* Color4::getClassName() const
{
  return "Color4";
}

int Color4::getHashCode() const
{
  float hash = r;
  hash       = std::pow((hash * 397), g);
  hash       = std::pow((hash * 397), b);
  hash       = std::pow((hash * 397), a);
  return static_cast<int>(hash);
}

/** Operators **/
Color4& Color4::addInPlace(const Color4& right)
{
  r += right.r;
  g += right.g;
  b += right.b;
  a += right.a;

  return *this;
}

Float32Array Color4::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

const Color4& Color4::toArray(Float32Array& array, unsigned int index) const
{
  if (array.size() < index + 4) {
    array.resize(index + 4);
  }

  array[index]     = r;
  array[index + 1] = g;
  array[index + 2] = b;
  array[index + 3] = a;

  return *this;
}

Color4& Color4::fromArray(const Float32Array& array, unsigned int offset)
{
  Color4::FromArrayToRef(array, offset, *this);
  return *this;
}

bool Color4::equals(const Color4& otherColor) const
{
  return stl_util::almost_equal(r, otherColor.r) && stl_util::almost_equal(g, otherColor.g)
         && stl_util::almost_equal(b, otherColor.b) && stl_util::almost_equal(a, otherColor.a);
}

Color4 Color4::add(const Color4& right) const
{
  return Color4(r + right.r, g + right.g, b + right.b, a + right.a);
}

Color4 Color4::subtract(const Color4& right) const
{
  return Color4(r - right.r, g - right.g, b - right.b, a - right.a);
}

const Color4& Color4::subtractToRef(const Color4& right, Color4& result) const
{
  result.r = r - right.r;
  result.g = g - right.g;
  result.b = b - right.b;
  result.a = a - right.a;

  return *this;
}

Color4 Color4::scale(float iscale) const
{
  return Color4(r * iscale, g * iscale, b * iscale, a * iscale);
}

const Color4& Color4::scaleToRef(int iscale, Color4& result) const
{
  result.r = r * static_cast<float>(iscale);
  result.g = g * static_cast<float>(iscale);
  result.b = b * static_cast<float>(iscale);
  result.a = a * static_cast<float>(iscale);

  return *this;
}

const Color4& Color4::scaleToRef(float iscale, Color4& result) const
{
  result.r = r * iscale;
  result.g = g * iscale;
  result.b = b * iscale;
  result.a = a * iscale;

  return *this;
}

const Color4& Color4::scaleAndAddToRef(float scale, Color4& result) const
{
  result.r += r * scale;
  result.g += g * scale;
  result.b += b * scale;
  result.a += a * scale;
  return *this;
}

const Color4& Color4::clampToRef(Color4& result, float min, float max) const
{
  result.r = Scalar::Clamp(r, min, max);
  result.g = Scalar::Clamp(g, min, max);
  result.b = Scalar::Clamp(b, min, max);
  result.a = Scalar::Clamp(a, min, max);

  return *this;
}

Color4 Color4::multiply(const Color4& color) const
{
  return Color4(r * color.r, g * color.g, b * color.b, a * color.a);
}

Color4 Color4::multiplyToRef(const Color4& color, Color4& result) const
{
  result.r = r * color.r;
  result.g = g * color.g;
  result.b = b * color.b;
  result.a = a * color.a;

  return result;
}

std::string Color4::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

Color4& Color4::copyFrom(const Color4& source)
{
  r = source.r;
  g = source.g;
  b = source.b;
  a = source.a;

  return *this;
}

Color4& Color4::copyFromFloats(float red, float green, float blue, float alpha)
{
  r = red;
  g = green;
  b = blue;
  a = alpha;

  return *this;
}

Color4& Color4::set(float red, float green, float blue, float alpha)
{
  return copyFromFloats(red, green, blue, alpha);
}

std::string Color4::toHexString(bool returnAsColor3) const
{
  const int intR = static_cast<int>(r * 255) | 0;
  const int intG = static_cast<int>(g * 255) | 0;
  const int intB = static_cast<int>(b * 255) | 0;

  if (returnAsColor3) {
    std::ostringstream ostream;
    ostream << "#" << Scalar::ToHex(intR) << Scalar::ToHex(intG) << Scalar::ToHex(intB);
    return ostream.str();
  }

  const int intA = static_cast<int>(a * 255) | 0;
  std::ostringstream ostream;
  ostream << "#" << Scalar::ToHex(intR) << Scalar::ToHex(intG) << Scalar::ToHex(intB)
          << Scalar::ToHex(intA);
  return ostream.str();
}

Color4 Color4::toLinearSpace() const
{
  Color4 convertedColor;
  toLinearSpaceToRef(convertedColor);
  return convertedColor;
}

const Color4& Color4::toLinearSpaceToRef(Color4& convertedColor) const
{
  convertedColor.r = std::pow(r, Math::ToLinearSpace);
  convertedColor.g = std::pow(g, Math::ToLinearSpace);
  convertedColor.b = std::pow(b, Math::ToLinearSpace);
  convertedColor.a = a;
  return *this;
}

Color4 Color4::toGammaSpace() const
{
  Color4 convertedColor;
  toGammaSpaceToRef(convertedColor);
  return convertedColor;
}

const Color4& Color4::toGammaSpaceToRef(Color4& convertedColor) const
{
  convertedColor.r = std::pow(r, Math::ToGammaSpace);
  convertedColor.g = std::pow(g, Math::ToGammaSpace);
  convertedColor.b = std::pow(b, Math::ToGammaSpace);
  convertedColor.a = a;
  return *this;
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Color4& color)
{
  os << "{\"R\":" << color.r << ",\"G\":" << color.g << ",\"B\":" << color.b << ",\"A\":" << color.a
     << "}";
  return os;
}

bool operator==(const Color4& lhs, const Color4& rhs)
{
  return lhs.equals(rhs);
}

bool operator!=(const Color4& lhs, const Color4& rhs)
{
  return !(lhs == rhs);
}

Color4 Color4::operator+(const Color4& right) const
{
  return add(right);
}

Color4& Color4::operator+=(const Color4& right)
{
  return addInPlace(right);
}

Color4 Color4::operator-(const Color4& right) const
{
  return subtract(right);
}

Color4 Color4::operator*(float iscale) const
{
  return scale(iscale);
}

/** Statics **/
Color4 Color4::FromHexString(const std::string& hex)
{
  if (hex.substr(0, 1) != "#" || hex.size() != 9) {
    return Color4(0, 0, 0, 0);
  }

  return Color4::FromInts(static_cast<int>(strtol(hex.substr(1, 2).c_str(), nullptr, 16)),
                          static_cast<int>(strtol(hex.substr(3, 2).c_str(), nullptr, 16)),
                          static_cast<int>(strtol(hex.substr(5, 2).c_str(), nullptr, 16)),
                          static_cast<int>(strtol(hex.substr(7, 2).c_str(), nullptr, 16)));
}

Color4 Color4::Lerp(const Color4& left, const Color4& right, float amount)
{
  Color4 result(0.f, 0.f, 0.f, 0.f);
  Color4::LerpToRef(left, right, amount, result);

  return result;
}

void Color4::LerpToRef(const Color4& left, const Color4& right, float amount, Color4& result)
{
  result.r = left.r + (right.r - left.r) * amount;
  result.g = left.g + (right.g - left.g) * amount;
  result.b = left.b + (right.b - left.b) * amount;
  result.a = left.a + (right.a - left.a) * amount;
}

Color4 Color4::FromColor3(const Color3& color3, float alpha)
{
  return Color4(color3.r, color3.g, color3.b, alpha);
}

Color4 Color4::FromArray(const Float32Array& array, unsigned int offset)
{
  if ((size_t)(offset + 3) < array.size())
    return Color4(array[offset], array[offset + 1], array[offset + 2], array[offset + 3]);
  else
    return Color4(array[offset], array[offset + 1], array[offset + 2], 1.f);
}

void Color4::FromArrayToRef(const Float32Array& array, unsigned int offset, Color4& result)
{
  result.r = array[offset];
  result.g = array[offset + 1];
  result.b = array[offset + 2];
  result.a = array[offset + 3];
}

Color4 Color4::FromInts(int r, int g, int b, int a)
{
  return Color4(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f);
}

Float32Array Color4::CheckColors4(const Float32Array& colors, size_t count)
{
  // Check if color3 was used
  if (colors.size() == count * 3) {
    Float32Array colors4;
    colors4.resize(count * 4);
    for (size_t index = 0; index < colors.size(); index += 3) {
      size_t newIndex       = (index / 3) * 4;
      colors4[newIndex]     = colors[index];
      colors4[newIndex + 1] = colors[index + 1];
      colors4[newIndex + 2] = colors[index + 2];
      colors4[newIndex + 3] = 1.f;
    }

    return colors4;
  }

  return colors;
}

} // end of namespace BABYLON
