#include <babylon/materials/color_curves.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

ColorCurves::ColorCurves()
    : _dirty{true}
    , _tempColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , _globalCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _highlightsCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _midtonesCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _shadowsCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _positiveCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _negativeCurve{Color4(0.f, 0.f, 0.f, 0.f)}
    , _globalHue{30.f}
    , _globalDensity{0.f}
    , _globalSaturation{0.f}
    , _globalExposure{0.f}
    , _highlightsHue{30.f}
    , _highlightsDensity{0.f}
    , _highlightsSaturation{0.f}
    , _highlightsExposure{0.f}
    , _midtonesHue{30.f}
    , _midtonesDensity{0.f}
    , _midtonesSaturation{0.f}
    , _midtonesExposure{0.f}
    , _shadowsHue{30.f}
    , _shadowsDensity{0.f}
    , _shadowsSaturation{0.f}
    , _shadowsExposure{0.f}
{
}

ColorCurves::ColorCurves(const ColorCurves& other)
    : _dirty{other._dirty}
    , _tempColor{other._tempColor}
    , _globalCurve{other._globalCurve}
    , _highlightsCurve{other._highlightsCurve}
    , _midtonesCurve{other._midtonesCurve}
    , _shadowsCurve{other._shadowsCurve}
    , _positiveCurve{other._positiveCurve}
    , _negativeCurve{other._negativeCurve}
    , _globalHue{other._globalHue}
    , _globalDensity{other._globalDensity}
    , _globalSaturation{other._globalSaturation}
    , _globalExposure{other._globalExposure}
    , _highlightsHue{other._highlightsHue}
    , _highlightsDensity{other._highlightsDensity}
    , _highlightsSaturation{other._highlightsSaturation}
    , _highlightsExposure{other._highlightsExposure}
    , _midtonesHue{other._midtonesHue}
    , _midtonesDensity{other._midtonesDensity}
    , _midtonesSaturation{other._midtonesSaturation}
    , _midtonesExposure{other._midtonesExposure}
    , _shadowsHue{other._shadowsHue}
    , _shadowsDensity{other._shadowsDensity}
    , _shadowsSaturation{other._shadowsSaturation}
    , _shadowsExposure{other._shadowsExposure}
{
}

ColorCurves::ColorCurves(ColorCurves&& other)
{
  *this = std::move(other);
}

ColorCurves& ColorCurves::operator=(const ColorCurves& other)
{
  if (&other != this) {
    _dirty                = other._dirty;
    _tempColor            = other._tempColor;
    _globalCurve          = other._globalCurve;
    _highlightsCurve      = other._highlightsCurve;
    _midtonesCurve        = other._midtonesCurve;
    _shadowsCurve         = other._shadowsCurve;
    _positiveCurve        = other._positiveCurve;
    _negativeCurve        = other._negativeCurve;
    _globalHue            = other._globalHue;
    _globalDensity        = other._globalDensity;
    _globalSaturation     = other._globalSaturation;
    _globalExposure       = other._globalExposure;
    _highlightsHue        = other._highlightsHue;
    _highlightsDensity    = other._highlightsDensity;
    _highlightsSaturation = other._highlightsSaturation;
    _highlightsExposure   = other._highlightsExposure;
    _midtonesHue          = other._midtonesHue;
    _midtonesDensity      = other._midtonesDensity;
    _midtonesSaturation   = other._midtonesSaturation;
    _midtonesExposure     = other._midtonesExposure;
    _shadowsHue           = other._shadowsHue;
    _shadowsDensity       = other._shadowsDensity;
    _shadowsSaturation    = other._shadowsSaturation;
    _shadowsExposure      = other._shadowsExposure;
  }

  return *this;
}

ColorCurves& ColorCurves::operator=(ColorCurves&& other)
{
  if (&other != this) {
    _dirty                = std::move(other._dirty);
    _tempColor            = std::move(other._tempColor);
    _globalCurve          = std::move(other._globalCurve);
    _highlightsCurve      = std::move(other._highlightsCurve);
    _midtonesCurve        = std::move(other._midtonesCurve);
    _shadowsCurve         = std::move(other._shadowsCurve);
    _positiveCurve        = std::move(other._positiveCurve);
    _negativeCurve        = std::move(other._negativeCurve);
    _globalHue            = std::move(other._globalHue);
    _globalDensity        = std::move(other._globalDensity);
    _globalSaturation     = std::move(other._globalSaturation);
    _globalExposure       = std::move(other._globalExposure);
    _highlightsHue        = std::move(other._highlightsHue);
    _highlightsDensity    = std::move(other._highlightsDensity);
    _highlightsSaturation = std::move(other._highlightsSaturation);
    _highlightsExposure   = std::move(other._highlightsExposure);
    _midtonesHue          = std::move(other._midtonesHue);
    _midtonesDensity      = std::move(other._midtonesDensity);
    _midtonesSaturation   = std::move(other._midtonesSaturation);
    _midtonesExposure     = std::move(other._midtonesExposure);
    _shadowsHue           = std::move(other._shadowsHue);
    _shadowsDensity       = std::move(other._shadowsDensity);
    _shadowsSaturation    = std::move(other._shadowsSaturation);
    _shadowsExposure      = std::move(other._shadowsExposure);
  }

  return *this;
}

ColorCurves::~ColorCurves()
{
}

float ColorCurves::globalHue() const
{
  return _globalHue;
}

void ColorCurves::setGlobalHue(float value)
{
  _globalHue = value;
  _dirty     = true;
}

float ColorCurves::globalDensity() const
{
  return _globalDensity;
}

void ColorCurves::setGlobalDensity(float value)
{
  _globalDensity = value;
  _dirty         = true;
}

float ColorCurves::globalSaturation() const
{
  return _globalSaturation;
}

void ColorCurves::setGlobalSaturation(float value)
{
  _globalSaturation = value;
  _dirty            = true;
}

float ColorCurves::highlightsHue() const
{
  return _highlightsHue;
}

void ColorCurves::setHighlightsHue(float value)
{
  _highlightsHue = value;
  _dirty         = true;
}

float ColorCurves::highlightsDensity() const
{
  return _highlightsDensity;
}

void ColorCurves::setHighlightsDensity(float value)
{
  _highlightsDensity = value;
  _dirty             = true;
}

float ColorCurves::highlightsSaturation() const
{
  return _highlightsSaturation;
}

void ColorCurves::setHighlightsSaturation(float value)
{
  _highlightsSaturation = value;
  _dirty                = true;
}

float ColorCurves::highlightsExposure() const
{
  return _highlightsExposure;
}

void ColorCurves::setHighlightsExposure(float value)
{
  _highlightsExposure = value;
  _dirty              = true;
}

float ColorCurves::midtonesHue() const
{
  return _midtonesHue;
}

void ColorCurves::setMidtonesHue(float value)
{
  _midtonesHue = value;
  _dirty       = true;
}

float ColorCurves::midtonesDensity() const
{
  return _midtonesDensity;
}

void ColorCurves::setMidtonesDensity(float value)
{
  _midtonesDensity = value;
  _dirty           = true;
}

float ColorCurves::midtonesSaturation() const
{
  return _midtonesSaturation;
}

void ColorCurves::setMidtonesSaturation(float value)
{
  _midtonesSaturation = value;
  _dirty              = true;
}

float ColorCurves::midtonesExposure() const
{
  return _midtonesExposure;
}

void ColorCurves::setMidtonesExposure(float value)
{
  _midtonesExposure = value;
  _dirty            = true;
}

float ColorCurves::shadowsHue() const
{
  return _shadowsHue;
}

void ColorCurves::setShadowsHue(float value)
{
  _shadowsHue = value;
  _dirty      = true;
}

float ColorCurves::shadowsDensity() const
{
  return _shadowsDensity;
}

void ColorCurves::setShadowsDensity(float value)
{
  _shadowsDensity = value;
  _dirty          = true;
}

float ColorCurves::shadowsSaturation() const
{
  return _shadowsSaturation;
}

void ColorCurves::setShadowsSaturation(float value)
{
  _shadowsSaturation = value;
  _dirty             = true;
}

float ColorCurves::shadowsExposure() const
{
  return _shadowsExposure;
}

void ColorCurves::setShadowsExposure(float value)
{
  _shadowsExposure = value;
  _dirty           = true;
}

void ColorCurves::Bind(ColorCurves& colorCurves, Effect* effect)
{
  if (colorCurves._dirty) {
    colorCurves._dirty = false;

    // Fill in global info.
    colorCurves.getColorGradingDataToRef(
      colorCurves._globalHue, colorCurves._globalDensity,
      colorCurves._globalSaturation, colorCurves._globalExposure,
      colorCurves._globalCurve);

    // Compute highlights info.
    colorCurves.getColorGradingDataToRef(
      colorCurves._highlightsHue, colorCurves._highlightsDensity,
      colorCurves._highlightsSaturation, colorCurves._highlightsExposure,
      colorCurves._tempColor);
    colorCurves._tempColor.multiplyToRef(colorCurves._globalCurve,
                                         colorCurves._highlightsCurve);

    // Compute midtones info.
    colorCurves.getColorGradingDataToRef(
      colorCurves._midtonesHue, colorCurves._midtonesDensity,
      colorCurves._midtonesSaturation, colorCurves._midtonesExposure,
      colorCurves._tempColor);
    colorCurves._tempColor.multiplyToRef(colorCurves._globalCurve,
                                         colorCurves._midtonesCurve);

    // Compute shadows info.
    colorCurves.getColorGradingDataToRef(
      colorCurves._shadowsHue, colorCurves._shadowsDensity,
      colorCurves._shadowsSaturation, colorCurves._shadowsExposure,
      colorCurves._tempColor);
    colorCurves._tempColor.multiplyToRef(colorCurves._globalCurve,
                                         colorCurves._shadowsCurve);

    // Compute deltas (neutral is midtones).
    colorCurves._highlightsCurve.subtractToRef(colorCurves._midtonesCurve,
                                               colorCurves._positiveCurve);
    colorCurves._midtonesCurve.subtractToRef(colorCurves._shadowsCurve,
                                             colorCurves._negativeCurve);
  }

  effect->setFloat4("vCameraColorCurvePositive", colorCurves._positiveCurve.r,
                    colorCurves._positiveCurve.g, colorCurves._positiveCurve.b,
                    colorCurves._positiveCurve.a);
  effect->setFloat4("vCameraColorCurveNeutral", colorCurves._midtonesCurve.r,
                    colorCurves._midtonesCurve.g, colorCurves._midtonesCurve.b,
                    colorCurves._midtonesCurve.a);
  effect->setFloat4("vCameraColorCurveNegative", colorCurves._negativeCurve.r,
                    colorCurves._negativeCurve.g, colorCurves._negativeCurve.b,
                    colorCurves._negativeCurve.a);
}

void ColorCurves::PrepareUniforms(std::vector<std::string>& uniformsList)
{
  std_util::concat(uniformsList, {
                                   "vCameraColorCurveNeutral",  //
                                   "vCameraColorCurvePositive", //
                                   "vCameraColorCurveNegative"  //
                                 });
}

void ColorCurves::getColorGradingDataToRef(float iHue, float iDensity,
                                           float iSaturation, float iExposure,
                                           Color4& result)
{
  auto hue        = ColorCurves::clamp(iHue, 0, 360);
  auto density    = ColorCurves::clamp(iDensity, -100, 100);
  auto saturation = ColorCurves::clamp(iSaturation, -100, 100);
  auto exposure   = ColorCurves::clamp(iExposure, -100, 100);

  // Remap the slider/config filter density with non-linear mapping and also
  // scale by half so that the maximum filter density is only 50% control. This
  // provides fine control for small values and reasonable range.
  density = ColorCurves::applyColorGradingSliderNonlinear(density);
  density *= 0.5f;

  exposure = ColorCurves::applyColorGradingSliderNonlinear(exposure);

  if (density < 0) {
    density *= -1.f;
    hue = static_cast<float>(static_cast<int>(hue + 180.f) % 360);
  }

  ColorCurves::fromHSBToRef(hue, density, 50.f + 0.25f * exposure, result);
  result.scaleToRef(2, result);
  result.a = 1.f + 0.01f * saturation;
}

float ColorCurves::applyColorGradingSliderNonlinear(float _value)
{
  float value = _value / 100.f;

  float x = std::abs(value);
  x       = std::pow(x, 2.f);

  if (value < 0.f) {
    x *= -1.f;
  }

  x *= 100.f;

  return x;
}

void ColorCurves::fromHSBToRef(float hue, float saturation, float brightness,
                               Color4& result)
{
  auto h = ColorCurves::clamp(hue, 0, 360);
  auto s = ColorCurves::clamp(saturation / 100.f, 0, 1);
  auto v = ColorCurves::clamp(brightness / 100.f, 0, 1);

  if (std_util::almost_equal(s, 0.f)) {
    result.r = v;
    result.g = v;
    result.b = v;
  }
  else {
    // sector 0 to 5
    h /= 60.f;
    float i = std::floor(h);

    // fractional part of h
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    switch (static_cast<unsigned int>(i)) {
      case 0:
        result.r = v;
        result.g = t;
        result.b = p;
        break;
      case 1:
        result.r = q;
        result.g = v;
        result.b = p;
        break;
      case 2:
        result.r = p;
        result.g = v;
        result.b = t;
        break;
      case 3:
        result.r = p;
        result.g = q;
        result.b = v;
        break;
      case 4:
        result.r = t;
        result.g = p;
        result.b = v;
        break;
      default: // case 5:
        result.r = v;
        result.g = p;
        result.b = q;
        break;
    }
  }

  result.a = 1.f;
}

float ColorCurves::clamp(float value, float min, float max)
{
  return std::min(std::max(value, min), max);
}

std::unique_ptr<ColorCurves> ColorCurves::clone() const
{
  return nullptr;
}

Json::object ColorCurves::serialize() const
{
  return Json::object();
}

std::unique_ptr<ColorCurves> Parse(const Json::value& /*source*/)
{
  return nullptr;
}

} // end of namespace BABYLON
