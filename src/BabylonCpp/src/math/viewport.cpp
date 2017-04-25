#include <babylon/math/viewport.h>

#include <babylon/engine/engine.h>

namespace BABYLON {

Viewport::Viewport()
{
}

Viewport::Viewport(int ix, int iy, int iwidth, int iheight)
    : x{ix}, y{iy}, width{iwidth}, height{iheight}
{
}

Viewport::Viewport(const Viewport& otherViewport)
    : x{otherViewport.x}
    , y{otherViewport.y}
    , width{otherViewport.width}
    , height{otherViewport.height}
{
}

Viewport::Viewport(Viewport&& otherViewport)
    : x{std::move(otherViewport.x)}
    , y{std::move(otherViewport.y)}
    , width{std::move(otherViewport.width)}
    , height{std::move(otherViewport.height)}
{
}

Viewport& Viewport::operator=(const Viewport& otherViewport)
{
  if (&otherViewport != this) {
    x      = otherViewport.x;
    y      = otherViewport.y;
    width  = otherViewport.width;
    height = otherViewport.height;
  }

  return *this;
}

Viewport& Viewport::operator=(Viewport&& otherViewport)
{
  if (&otherViewport != this) {
    x      = std::move(otherViewport.x);
    y      = std::move(otherViewport.y);
    width  = std::move(otherViewport.width);
    height = std::move(otherViewport.height);
  }

  return *this;
}

Viewport::~Viewport()
{
}

Viewport Viewport::copy() const
{
  return Viewport(*this);
}

std::unique_ptr<Viewport> Viewport::clone() const
{
  return std_util::make_unique<Viewport>(*this);
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

} // end of namespace BABYLON
