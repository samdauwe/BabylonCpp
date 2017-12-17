#include <babylon/extensions/hexplanetgeneration/terrain/color.h>

#include <babylon/math/color3.h>

namespace BABYLON {
namespace Extensions {

Color::Color(float maxHeight)
    : _maxHeight{maxHeight}
    , _parchmentFactor{0.f}
    , _terrainGradient{Gradient<Color3>(_maxHeight + 1.f)}
    , _terrainGradient2D{ColorGradient2D(256)}
{
}

Color::~Color()
{
}

void Color::setupGradient()
{
  _terrainGradient = Gradient<Color3>(_maxHeight + 1.f);

  // Deep Ocean
  _terrainGradient.addStop(0.f, Color3(0, 0, 153.f / 255.f));

  // Sea
  _terrainGradient.addStop(0.4f,
                           Color3(0.f / 255.f, 102.f / 255.f, 255.f / 255.f));

  // Coast
  _terrainGradient.addStop(0.46f,
                           Color3(0.f / 255.f, 120.f / 255.f, 255.f / 255.f));

  // Coastal Water
  _terrainGradient.addStop(0.49f,
                           Color3(153.f / 255.f, 204.f / 255.f, 255.f / 255.f));

  // Coastal Land
  _terrainGradient.addStop(0.5f,
                           Color3(255.f / 255.f, 255.f / 255.f, 204.f / 255.f));

  // Grasslands
  _terrainGradient.addStop(0.51f,
                           Color3(51.f / 255.f, 204.f / 255.f, 51.f / 255.f));

  // Mountain
  _terrainGradient.addStop(0.65f,
                           Color3(153.f / 255.f, 102.f / 255.f, 51.f / 255.f));

  // Mountain Peaks
  _terrainGradient.addStop(0.72f,
                           Color3(230.f / 255.f, 255.f / 255.f, 255.f / 255.f));

  // Mountain Peaks
  _terrainGradient.addStop(1.f,
                           Color3(230.f / 255.f, 255.f / 255.f, 255.f / 255.f));

  _terrainGradient.calculate();

  _terrainGradient2D = ColorGradient2D(256);

  _terrainGradient2D.addColorStop(
    0.f, 0.f, Color3(70.f / 255.f, 97.f / 255.f, 16.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.2f, 0.f, Color3(92.f / 255.f, 139.f / 255.f, 38.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.4f, 0.f, Color3(110.f / 255.f, 156.f / 255.f, 42.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.6f, 0.f, Color3(81.f / 255.f, 156.f / 255.f, 52.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.8f, 0.f, Color3(125.f / 255.f, 184.f / 255.f, 137.f / 255.f));
  _terrainGradient2D.addColorStop(
    1.f, 0.f, Color3(230.f / 255.f, 247.f / 255.f, 234.f / 255.f));

  _terrainGradient2D.addColorStop(
    0.f, 0.25f, Color3(70.f / 255.f, 97.f / 255.f, 16.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.2f, 0.25f, Color3(84.f / 255.f, 117.f / 255.f, 30.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.4f, 0.25f, Color3(80.f / 255.f, 156.f / 255.f, 42.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.6f, 0.25f, Color3(42.f / 255.f, 156.f / 255.f, 52.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.8f, 0.25f, Color3(95.f / 255.f, 154.f / 255.f, 107.f / 255.f));
  _terrainGradient2D.addColorStop(
    1.f, 0.25f, Color3(230.f / 255.f, 247.f / 255.f, 234.f / 255.f));

  _terrainGradient2D.addColorStop(
    0.f, 0.5f, Color3(132.f / 255.f, 194.f / 255.f, 8.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.2f, 0.5f, Color3(96.f / 255.f, 191.f / 255.f, 8.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.4f, 0.5f, Color3(34.f / 255.f, 139.f / 255.f, 34.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.6f, 0.5f, Color3(12.f / 255.f, 125.f / 255.f, 12.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.8f, 0.5f, Color3(65.f / 255.f, 124.f / 255.f, 77.f / 255.f));
  _terrainGradient2D.addColorStop(
    1.f, 0.5f, Color3(230.f / 255.f, 247.f / 255.f, 234.f / 255.f));

  _terrainGradient2D.addColorStop(
    0.f, 0.75f, Color3(255.f / 255.f, 230.f / 255.f, 120.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.2f, 0.75f, Color3(255.f / 255.f, 230.f / 255.f, 100.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.4f, 0.75f, Color3(130.f / 255.f, 156.f / 255.f, 11.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.6f, 0.75f, Color3(57.f / 255.f, 125.f / 255.f, 12.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.8f, 0.75f, Color3(35.f / 255.f, 94.f / 255.f, 47.f / 255.f));
  _terrainGradient2D.addColorStop(
    1.f, 0.75f, Color3(230.f / 255.f, 247.f / 255.f, 234.f / 255.f));

  _terrainGradient2D.addColorStop(
    0.f, 1.f, Color3(255.f / 255.f, 237.f / 255.f, 179.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.2f, 1.f, Color3(255.f / 255.f, 211.f / 255.f, 94.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.4f, 1.f, Color3(178.f / 255.f, 189.f / 255.f, 62.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.6f, 1.f, Color3(155.f / 255.f, 189.f / 255.f, 62.f / 255.f));
  _terrainGradient2D.addColorStop(
    0.8f, 1.f, Color3(88.f / 255.f, 148.f / 255.f, 68.f / 255.f));
  _terrainGradient2D.addColorStop(
    1.f, 1.f, Color3(230.f / 255.f, 247.f / 255.f, 234.f / 255.f));

  _terrainGradient2D.calculate();
}

Color3 Color::getColor(float heightNorm, float rainNorm, float tempNorm)
{
  float invHeight = (1.f - (heightNorm * _parchmentFactor));

  auto color2D
    = _terrainGradient2D.getColor(1.f - tempNorm, 1.f - (rainNorm * invHeight));
  auto color1D = _terrainGradient.getValue(static_cast<size_t>(heightNorm));

  // Handle Ocean coloring
  if (heightNorm < 0.5f) {
    return color1D;
  }
  // Coastal coloring to ensure smooth gradient from beach to mainland
  else if (heightNorm >= 0.5f && heightNorm <= 0.505f) {
    return Color3::Lerp(color1D, color2D, 0.5f);
  }
  // Interpolate mountain ranges into Whittaker coloring scheme
  else if (heightNorm > 0.6f) {
    // If we are a snowy region, color a more rocky hue to the mountains
    if (tempNorm < 0.2f && heightNorm < 0.70f) {
      Color3 rockyColor(70.f / 255.f, 90.f / 255.f, 100.f / 255.f);
      color1D = Color3::Lerp(color1D, rockyColor, 1.f - tempNorm);
    }
    // 8.3 scaling factor comes from 0.72 as the beginning of mountain peaks, so
    // 1.0 / (.72 - .6)
    return Color3::Lerp(color2D, color1D, (heightNorm - 0.6f) * 8.3f);
  }

  return color2D;
}

Color3 Color::get2DColor(float u, float v)
{
  return _terrainGradient2D.getColor(u, v);
}

} // end of namespace Extensions
} // end of namespace BABYLON
