#include <babylon/extensions/hexplanetgeneration/utils/color_gradient_2d.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {
namespace Extensions {

bool ColorStop::operator==(const ColorStop& rhs) const
{
  return stl_util::almost_equal(u, rhs.u) && stl_util::almost_equal(v, rhs.v)
         && (color == rhs.color);
}

ColorGradient2D::ColorGradient2D(float resolution) : _resolution{resolution}
{
  const auto n = static_cast<size_t>(_resolution);
  _gradient.resize(n);
  for (auto& g : _gradient) {
    g.resize(n);
  }
}

ColorGradient2D::~ColorGradient2D()
{
}

void ColorGradient2D::addColorStop(float u, float v, const Color3& color)
{
  ColorStop colorStop{u, v, color};
  _colorStops.emplace_back(colorStop);
}

Color3 ColorGradient2D::getColor(float u, float v) const
{
  const size_t x = static_cast<size_t>(std::floor(u * (_resolution - 1.f)));
  const size_t y = static_cast<size_t>(std::floor(v * (_resolution - 1.f)));

  return _gradient[y][x];
}

void ColorGradient2D::calculate()
{
  if (_colorStops.size() < 4) {
    return;
  }

  for (unsigned int y = 0; y < _resolution; ++y) {
    _gradient[y].clear();
    for (unsigned int x = 0; x < _resolution; ++x) {
      float u = x / _resolution;
      float v = y / _resolution;

      NearestNeighbors neighbors = getNearestNeighbors(u, v);

      // If this point is concentric with a color stop, choose the color stop
      // color
      if (isConcentricWithColorStop(neighbors)) {
        _gradient[y][x] = neighbors.topLeft.color;
        continue;
      }
      else if (onUEdge(neighbors)) {
        auto y1    = neighbors.topLeft.v;
        auto y2    = neighbors.bottomLeft.v;
        auto blerp = (neighbors.bottomLeft.color - neighbors.topLeft.color)
                       * (v - y1) / (y2 - y1)
                     + neighbors.topLeft.color;
        _gradient[y][x] = blerp;
        continue;
      }
      else if (onVEdge(neighbors)) {
        auto x1    = neighbors.topLeft.u;
        auto x2    = neighbors.topRight.u;
        auto blerp = (neighbors.topRight.color - neighbors.topLeft.color)
                       * (u - x1) / (x2 - x1)
                     + neighbors.topLeft.color;
        _gradient[y][x] = blerp;
        continue;
      }

      auto x1    = neighbors.bottomLeft.u;
      auto y1    = neighbors.bottomLeft.v;
      auto x2    = neighbors.topRight.u;
      auto y2    = neighbors.topRight.v;
      auto blerp = (neighbors.bottomLeft.color * (x2 - u) * (y2 - v)
                    + neighbors.bottomRight.color * (u - x1) * (y2 - v)
                    + neighbors.topLeft.color * (x2 - u) * (v - y1)
                    + neighbors.topRight.color * (u - x1) * (v - y1))
                   * (1.f / ((x2 - x1) * (y2 - y1)));
      _gradient[y][x] = blerp;
    }
  }
}

bool ColorGradient2D::isConcentricWithColorStop(
  const NearestNeighbors& neighbors) const
{
  return (neighbors.topRight == neighbors.topLeft
          && neighbors.bottomLeft == neighbors.topLeft
          && neighbors.bottomRight == neighbors.topLeft);
}

bool ColorGradient2D::onUEdge(const NearestNeighbors& neighbors) const
{
  return (neighbors.topLeft == neighbors.topRight
          && neighbors.bottomLeft == neighbors.bottomRight);
}

bool ColorGradient2D::onVEdge(const NearestNeighbors& neighbors) const
{
  return (neighbors.topLeft == neighbors.bottomLeft
          && neighbors.topRight == neighbors.bottomRight);
}

NearestNeighbors ColorGradient2D::getNearestNeighbors(float u, float v) const
{
  NearestNeighbors neighbors;
  bool hasTopLeft     = false;
  bool hasTopRight    = false;
  bool hasBottomLeft  = false;
  bool hasBottomRight = false;

  for (const auto& cs : _colorStops) {
    if (cs.u <= u && cs.v <= v) {
      if (!hasTopLeft
          || (neighbors.topLeft.u <= cs.u && neighbors.topLeft.v <= cs.v)) {
        neighbors.topLeft = cs;
        hasTopLeft        = true;
      }
    }

    if (cs.u >= u && cs.v <= v) {
      if (!hasTopRight
          || (neighbors.topRight.u >= cs.u && neighbors.topRight.v <= cs.v)) {
        neighbors.topRight = cs;
        hasTopRight        = true;
      }
    }

    if (cs.u <= u && cs.v >= v) {
      if (!hasBottomLeft || (neighbors.bottomLeft.u <= cs.u
                             && neighbors.bottomLeft.v >= cs.v)) {
        neighbors.bottomLeft = cs;
        hasBottomLeft        = true;
      }
    }

    if (cs.u >= u && cs.v >= v) {
      if (!hasBottomRight || (neighbors.bottomRight.u >= cs.u
                              && neighbors.bottomRight.v >= cs.v)) {
        neighbors.bottomRight = cs;
        hasBottomRight        = true;
      }
    }
  }
  return neighbors;
}

} // end of namespace Extensions
} // end of namespace BABYLON
