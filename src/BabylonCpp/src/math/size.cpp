#include <babylon/math/size.h>

namespace BABYLON {

Size::Size(int _width, int _height) : ISize(_width, _height)
{
}

Size::Size(const Size& otherSize) : ISize(otherSize.width, otherSize.height)
{
}

Size::Size(Size&& otherSize) : ISize(otherSize.width, otherSize.height)
{
  otherSize.width  = 0;
  otherSize.height = 0;
}

Size::~Size()
{
}

Size& Size::operator=(const Size& otherSize)
{
  if (&otherSize != this) {
    width  = otherSize.width;
    height = otherSize.height;
  }

  return *this;
}

Size& Size::operator=(Size&& otherSize)
{
  if (&otherSize != this) {
    width  = std::move(otherSize.width);
    height = std::move(otherSize.height);
  }

  return *this;
}

Size Size::copy() const
{
  return Size(*this);
}

std::unique_ptr<Size> Size::clone() const
{
  return std::make_unique<Size>(*this);
}

std::ostream& operator<<(std::ostream& os, const Size& size)
{
  os << "{\"W\":" << size.width << ",\"H\":" << size.height << "}";
  return os;
}

std::string Size::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

const char* Size::getClassName() const
{
  return "Size";
}

int Size::getHashCode() const
{
  int hash = width;
  hash     = (hash * 397) ^ height;
  return hash;
}

void Size::copyFrom(const Size& src)
{
  width  = src.width;
  height = src.height;
}

Size& Size::copyFromFloats(float iWidth, float iHeight)
{
  width  = static_cast<int>(iWidth);
  height = static_cast<int>(iHeight);

  return *this;
}

Size& Size::set(float iWidth, float iHeight)
{
  return copyFromFloats(iWidth, iHeight);
}

Size Size::multiplyByFloats(float w, float h) const
{
  return Size(static_cast<int>(width * w), static_cast<int>(height * h));
}

bool Size::equals(const Size& other)
{
  return (width == other.width) && (height == other.height);
}

int Size::surface() const
{
  return width * height;
}

Size Size::Zero()
{
  return Size(0, 0);
}

Size Size::add(const Size& otherSize) const
{
  return Size(width + otherSize.width, height + otherSize.height);
}

Size Size::subtract(const Size& otherSize) const
{
  return Size(width - otherSize.width, height - otherSize.height);
}

Size Size::scale(float scale) const
{
  const float w = static_cast<float>(width) * scale;
  const float h = static_cast<float>(height) * scale;

  return Size(static_cast<int>(w), static_cast<int>(h));
}

Size Size::Lerp(const Size& start, const Size& end, float amount)
{
  const float w = static_cast<float>(start.width)
                  + (static_cast<float>(end.width - start.width) * amount);
  const float h = static_cast<float>(start.height)
                  + (static_cast<float>(end.height - start.height) * amount);

  return Size(static_cast<int>(w), static_cast<int>(h));
}

} // end of namespace BABYLON
