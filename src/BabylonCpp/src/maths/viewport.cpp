#include <babylon/maths/viewport.h>

#include <babylon/engines/engine.h>

namespace BABYLON {

Viewport::Viewport() = default;

Viewport::Viewport(float ix, float iy, float iwidth, float iheight)
    : x{ix}, y{iy}, width{iwidth}, height{iheight}
{
}

Viewport::Viewport(const Viewport& otherViewport) = default;

Viewport::Viewport(Viewport&& otherViewport) = default;

Viewport& Viewport::operator=(const Viewport& otherViewport) = default;

Viewport& Viewport::operator=(Viewport&& otherViewport) = default;

Viewport::~Viewport() = default;

Viewport Viewport::copy() const
{
  return Viewport(*this);
}

std::unique_ptr<Viewport> Viewport::clone() const
{
  return std::make_unique<Viewport>(*this);
}

std::ostream& operator<<(std::ostream& os, const Viewport& viewport)
{
  os << "{\"X\":" << viewport.x << ",\"Y\":" << viewport.y
     << ",\"Width\":" << viewport.width << ",\"Height\":" << viewport.height
     << "}";
  return os;
}

/** Methods **/
Viewport Viewport::toGlobal(int renderWidth, int renderHeight) const
{
  return Viewport(x * renderWidth, y * renderHeight, width * renderWidth,
                  height * renderHeight);
}

Viewport& Viewport::toGlobalToRef(int renderWidth, int renderHeight,
                                  Viewport& ref)
{
  ref.x      = x * renderWidth;
  ref.y      = y * renderHeight;
  ref.width  = width * renderWidth;
  ref.height = height * renderHeight;
  return *this;
}

} // end of namespace BABYLON
