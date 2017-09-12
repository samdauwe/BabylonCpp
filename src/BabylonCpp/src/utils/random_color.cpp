#include <babylon/utils/random_color.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>

namespace BABYLON {
namespace randomcolor {

Range::Range() : _start{0}, _end{0}
{
}

Range::Range(float start, float end) : _start{start}, _end{end}
{
}

Range::Range(const Range& otherRange)
    : _start{otherRange._start}, _end{otherRange._end}
{
}

Range::Range(Range&& otherRange)
{
  *this = ::std::move(otherRange);
}

Range& Range::operator=(const Range& otherRange)
{
  if (&otherRange != this) {
    _start = otherRange._start;
    _end   = otherRange._end;
  }

  return *this;
}

Range& Range::operator=(Range&& otherRange)
{
  if (&otherRange != this) {
    _start = ::std::move(otherRange._start);
    _end   = ::std::move(otherRange._end);
  }

  return *this;
}

Range::~Range()
{
}

std::ostream& operator<<(std::ostream& os, const Range& range)
{
  os << "{start:" << range._start << ",end:" << range._end << "}";
  return os;
}

bool Range::contains(float value)
{
  return value >= _start && value <= _end;
}

float Range::start() const
{
  return _start;
}

float Range::end() const
{
  return _end;
}

////////////////////////////////////////////////////////////////////////////////

ColorInfo::ColorInfo() : _hasHueRange{false}, _isValid{false}
{
}

ColorInfo::ColorInfo(const Range& hueRange, const Range& saturationRange,
                     const Range& brightnessRange,
                     const std::vector<Range>& lowerBounds)
    : _hueRange{hueRange}
    , _hasHueRange{true}
    , _saturationRange{saturationRange}
    , _brightnessRange{brightnessRange}
    , _lowerBounds{lowerBounds}
    , _isValid{true}
{
}

ColorInfo::ColorInfo(const ColorInfo& other)
    : _hueRange{other._hueRange}
    , _hasHueRange{other._hasHueRange}
    , _saturationRange{other._saturationRange}
    , _brightnessRange{other._brightnessRange}
    , _lowerBounds{other._lowerBounds}
    , _isValid{other._isValid}
{
}

ColorInfo::ColorInfo(ColorInfo&& other)
{
  *this = ::std::move(other);
}

ColorInfo& ColorInfo::operator=(const ColorInfo& other)
{
  if (&other != this) {
    _hueRange        = other._hueRange;
    _hasHueRange     = other._hasHueRange;
    _saturationRange = other._saturationRange;
    _brightnessRange = other._brightnessRange;
    _lowerBounds     = other._lowerBounds;
    _isValid         = other._isValid;
  }

  return *this;
}

ColorInfo& ColorInfo::operator=(ColorInfo&& other)
{
  if (&other != this) {
    _hueRange        = ::std::move(other._hueRange);
    _hasHueRange     = ::std::move(other._hasHueRange);
    _saturationRange = ::std::move(other._saturationRange);
    _brightnessRange = ::std::move(other._brightnessRange);
    _lowerBounds     = ::std::move(other._lowerBounds);
    _isValid         = ::std::move(other._isValid);
  }

  return *this;
}

ColorInfo::~ColorInfo()
{
}

bool ColorInfo::hasHueRange() const
{
  return _hasHueRange;
}

Range& ColorInfo::getHueRange()
{
  return _hueRange;
}

const Range& ColorInfo::getHueRange() const
{
  return _hueRange;
}

void ColorInfo::setHueRange(const Range& hueRange)
{
  _hueRange = hueRange;
}

Range& ColorInfo::getSaturationRange()
{
  return _saturationRange;
}

const Range& ColorInfo::getSaturationRange() const
{
  return _saturationRange;
}

void ColorInfo::setSaturationRange(const Range& saturationRange)
{
  _saturationRange = saturationRange;
}

Range& ColorInfo::getBrightnessRange()
{
  return _brightnessRange;
}

const Range& ColorInfo::getBrightnessRange() const
{
  return _brightnessRange;
}

void ColorInfo::setBrightnessRange(const Range& brightnessRange)
{
  _brightnessRange = brightnessRange;
}

std::vector<Range>& ColorInfo::getLowerBounds()
{
  return _lowerBounds;
}

const std::vector<Range>& ColorInfo::getLowerBounds() const
{
  return _lowerBounds;
}

void ColorInfo::setLowerBounds(const std::vector<Range>& lowerBounds)
{
  _lowerBounds = lowerBounds;
}

bool ColorInfo::isValid() const
{
  return _isValid;
}

////////////////////////////////////////////////////////////////////////////////

Options::Options()
    : _hue{0}
    , _saturationType{SaturationType::UNDEFINED}
    , _luminosity{Luminosity::UNDEFINED}
{
}

Options::~Options()
{
}

int Options::getHue() const
{
  return _hue;
}

void Options::setHue(int hue)
{
  _hue = hue;
}

SaturationType Options::getSaturationType() const
{
  return _saturationType;
}

void Options::setSaturationType(const SaturationType& saturationType)
{
  _saturationType = saturationType;
}

Luminosity Options::getLuminosity() const
{
  return _luminosity;
}

void Options::setLuminosity(const Luminosity& luminosity)
{
  _luminosity = luminosity;
}

////////////////////////////////////////////////////////////////////////////////

RandomColor::RandomColor()
{
  loadColorBounds();
}

RandomColor::~RandomColor()
{
}

std::array<unsigned int, 3> RandomColor::HSVToRGB(float hue, float saturation,
                                                  float value)
{
  float fR = 0.f, fG = 0.f, fB = 0.f;
  unsigned int iR = 0, iG = 0, iB = 0;

  float fC      = value * saturation; // Chroma
  float fHPrime = std::fmod(hue / 60.f, 6.f);
  float fX      = fC * (1.f - std::abs(std::fmod(fHPrime, 2.f) - 1.f));
  float fM      = value - fC;

  if (0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0.f;
  }
  else if (1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0.f;
  }
  else if (2 <= fHPrime && fHPrime < 3) {
    fR = 0.f;
    fG = fC;
    fB = fX;
  }
  else if (3 <= fHPrime && fHPrime < 4) {
    fR = 0.f;
    fG = fX;
    fB = fC;
  }
  else if (4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0.f;
    fB = fC;
  }
  else if (5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0.f;
    fB = fX;
  }
  else {
    fR = 0.f;
    fG = 0.f;
    fB = 0.f;
  }

  fR += fM;
  fG += fM;
  fB += fM;

  iR = static_cast<unsigned char>(std::ceil(fR * 255.f));
  iG = static_cast<unsigned char>(std::ceil(fG * 255.f));
  iB = static_cast<unsigned char>(std::ceil(fB * 255.f));

  return {{iR, iG, iB}};
}

std::string RandomColor::GetColorName(const Color& color)
{
  switch (color) {
    case Color::MONOCHROME:
      return "MONOCHROME";
    case Color::RED:
      return "RED";
    case Color::ORANGE:
      return "ORANGE";
    case Color::YELLOW:
      return "YELLOW";
    case Color::GREEN:
      return "GREEN";
    case Color::BLUE:
      return "BLUE";
    case Color::PURPLE:
      return "PURPLE";
    case Color::PINK:
      return "PINK";
    default:
      return "Unkown";
  }
}

std::array<unsigned int, 3> RandomColor::getColor(float hue, float saturation,
                                                  float brightness)
{
  return RandomColor::HSVToRGB(hue, saturation / 100.f, brightness / 100.f);
}

std::array<unsigned int, 3> RandomColor::randomColor()
{
  return randomColor(0, SaturationType::UNDEFINED, Luminosity::UNDEFINED);
}

std::array<unsigned int, 3>
RandomColor::randomColor(float value, const SaturationType& saturationType,
                         const Luminosity& luminosity)
{
  float hue        = value;
  hue              = pickHue(hue);
  float saturation = pickSaturation(hue, saturationType, luminosity);
  float brightness = pickBrightness(hue, saturation, luminosity);

  return getColor(hue, saturation, brightness);
}

std::vector<std::array<unsigned int, 3>> RandomColor::randomColor(int count)
{
  if (count <= 0) {
    return std::vector<std::array<unsigned int, 3>>();
  }

  std::vector<std::array<unsigned int, 3>> colors;
  for (int i = 0; i < count; ++i) {
    colors.emplace_back(randomColor());
  }

  return colors;
}

std::array<unsigned int, 3> RandomColor::randomColor(const Color& color)
{
  float hue = pickHue(RandomColor::GetColorName(color));
  float saturation
    = pickSaturation(color, SaturationType::UNDEFINED, Luminosity::UNDEFINED);
  float brightness = pickBrightness(color, saturation, Luminosity::UNDEFINED);

  return getColor(hue, saturation, brightness);
}

std::vector<std::array<unsigned int, 3>> RandomColor::random(const Color& color,
                                                             int count)
{
  if (count <= 0) {
    return std::vector<std::array<unsigned int, 3>>();
  }

  std::vector<std::array<unsigned int, 3>> colors;
  for (int i = 0; i < count; ++i) {
    colors.emplace_back(randomColor(color));
  }

  return colors;
}

float RandomColor::pickHue(float hue)
{
  Range hueRange = getHueRange(hue);
  return doPickHue(hueRange);
}

float RandomColor::doPickHue(const Range& hueRange)
{
  float hue = randomWithin(hueRange);

  // Instead of storing red as two seperate ranges,
  // we group them, using negative numbers
  if (hue < 0.f) {
    hue = 360.f + hue;
  }

  return hue;
}

float RandomColor::pickHue(const std::string& name)
{
  Range hueRange = getHueRange(name);
  return doPickHue(hueRange);
}

Range RandomColor::getHueRange(float number)
{
  if (number < 360 && number > 0) {
    return Range(number, number);
  }

  return Range(0, 360);
}

Range RandomColor::getHueRange(const std::string& name)
{
  if (stl_util::contains(_colors, name)) {
    return _colors[name].getHueRange();
  }

  return Range(0, 360);
}

float RandomColor::pickSaturation(float hue,
                                  const SaturationType& saturationType,
                                  const Luminosity& luminosity)
{
  return pickSaturation(getColorInfo(hue), saturationType, luminosity);
}

float RandomColor::pickSaturation(const Color& color,
                                  const SaturationType& saturationType,
                                  const Luminosity& luminosity)
{
  const ColorInfo& colorInfo = _colors[RandomColor::GetColorName(color)];
  return pickSaturation(colorInfo, saturationType, luminosity);
}

float RandomColor::pickSaturation(const ColorInfo& colorInfo,
                                  const SaturationType& saturationType,
                                  const Luminosity& luminosity)
{

  switch (saturationType) {
    case SaturationType::RANDOM:
      return randomWithin(Range(0, 100));
    case SaturationType::MONOCHROME:
      return 0;
    case SaturationType::UNDEFINED:
      break;
    default:
      break;
  }

  if (!colorInfo.isValid()) {
    return 0;
  }

  const Range& saturationRange = colorInfo.getSaturationRange();

  float min = saturationRange.start();
  float max = saturationRange.end();

  switch (luminosity) {
    case Luminosity::LIGHT:
      min = 55;
      break;
    case Luminosity::BRIGHT:
      min = max - 10;
      break;
    case Luminosity::DARK:
      max = 55;
      break;
    case Luminosity::RANDOM:
    case Luminosity::UNDEFINED:
    default:
      break;
  }

  return randomWithin(Range(min, max));
}

float RandomColor::pickBrightness(float hue, float saturation,
                                  const Luminosity& luminosity)
{
  ColorInfo colorInfo = getColorInfo(hue);
  return pickBrightness(colorInfo, saturation, luminosity);
}

float RandomColor::pickBrightness(const Color& color, float saturation,
                                  const Luminosity& luminosity)
{
  const ColorInfo& colorInfo = _colors[RandomColor::GetColorName(color)];

  return pickBrightness(colorInfo, saturation, luminosity);
}

float RandomColor::pickBrightness(const ColorInfo& colorInfo, float saturation,
                                  const Luminosity& luminosity)
{
  float min = getMinimumBrightness(colorInfo, saturation);
  float max = 100.f;

  switch (luminosity) {
    case Luminosity::DARK:
      max = min + 20.f;
      break;
    case Luminosity::BRIGHT:
    case Luminosity::LIGHT:
      min = (max + min) / 2.f;
      break;
    case Luminosity::RANDOM:
      min = 0.f;
      max = 100.f;
      break;
    case Luminosity::UNDEFINED:
    default:
      break;
  }

  return randomWithin(Range(min, max));
}

float RandomColor::getMinimumBrightness(const ColorInfo& colorInfo,
                                        float saturation)
{
  if (!colorInfo.isValid()) {
    return 0;
  }

  const std::vector<Range>& lowerBounds = colorInfo.getLowerBounds();
  for (unsigned int i = 0; i < lowerBounds.size() - 1; ++i) {

    float s1 = lowerBounds.at(i).start();
    float v1 = lowerBounds.at(i).end();

    if (i == lowerBounds.size() - 1) {
      break;
    }
    float s2 = lowerBounds.at(i + 1).start();
    float v2 = lowerBounds.at(i + 1).end();

    if (saturation >= s1 && saturation <= s2) {

      float m = v2 - v1 / s2 - s1;
      float b = v1 - m * s1;

      return m * saturation + b;
    }
  }

  return 0.f;
}

ColorInfo RandomColor::getColorInfo(float hue)
{
  // Maps red colors to make picking hue easier
  if (hue >= 334 && hue <= 360) {
    hue -= 360;
  }

  for (auto& item : _colors) {
    ColorInfo& colorInfo = item.second;
    if (colorInfo.hasHueRange() && colorInfo.getHueRange().contains(hue)) {
      return colorInfo;
    }
  }

  return ColorInfo();
}

float RandomColor::randomWithin(const Range& range)
{
  return std::floor(range.start()
                    + Math::randomNumber(0.f, 1.f)
                        * (range.end() + 1.f - (range.start())));
}

void RandomColor::defineColor(const std::string& name,
                              const std::vector<Range>& lowerBounds)
{
  float sMin = lowerBounds.front().start();
  float sMax = lowerBounds.at(lowerBounds.size() - 1).start();
  float bMin = lowerBounds.at(lowerBounds.size() - 1).end();
  float bMax = lowerBounds.front().end();

  _colors[name]
    = ColorInfo(Range(), Range(sMin, sMax), Range(bMin, bMax), lowerBounds);
}

void RandomColor::defineColor(const std::string& name, const Range& hueRange,
                              const std::vector<Range>& lowerBounds)
{
  float sMin = lowerBounds.front().start();
  float sMax = lowerBounds.at(lowerBounds.size() - 1).start();
  float bMin = lowerBounds.at(lowerBounds.size() - 1).end();
  float bMax = lowerBounds.front().end();

  _colors[name]
    = ColorInfo(hueRange, Range(sMin, sMax), Range(bMin, bMax), lowerBounds);
}

void RandomColor::loadColorBounds()
{
  // Monochrome
  defineColor(RandomColor::GetColorName(Color::MONOCHROME), {
                                                              Range(0, 0),  //
                                                              Range(100, 0) //
                                                            });

  // Red
  defineColor(RandomColor::GetColorName(Color::RED), Range(-26, 18),
              {
                Range(20, 100), //
                Range(30, 92),  //
                Range(40, 89),  //
                Range(50, 85),  //
                Range(60, 78),  //
                Range(70, 70),  //
                Range(80, 60),  //
                Range(90, 55),  //
                Range(100, 50)  //
              });

  // Orange
  defineColor(RandomColor::GetColorName(Color::ORANGE), Range(19, 46),
              {
                Range(20, 100), //
                Range(30, 93),  //
                Range(40, 88),  //
                Range(50, 86),  //
                Range(60, 85),  //
                Range(70, 70),  //
                Range(100, 70)  //
              });

  // Yellow
  defineColor(RandomColor::GetColorName(Color::YELLOW), Range(47, 62),
              {
                Range(25, 100), //
                Range(40, 94),  //
                Range(50, 89),  //
                Range(60, 86),  //
                Range(70, 84),  //
                Range(80, 82),  //
                Range(90, 80),  //
                Range(100, 75)  //
              });

  // Green
  defineColor(RandomColor::GetColorName(Color::GREEN), Range(63, 178),
              {
                Range(30, 100), //
                Range(40, 90),  //
                Range(50, 85),  //
                Range(60, 81),  //
                Range(70, 74),  //
                Range(80, 64),  //
                Range(90, 50),  //
                Range(100, 40)  //
              });

  // Blue
  defineColor(RandomColor::GetColorName(Color::BLUE), Range(179, 257),
              {
                Range(20, 100), //
                Range(30, 86),  //
                Range(40, 80),  //
                Range(50, 74),  //
                Range(60, 60),  //
                Range(70, 52),  //
                Range(80, 44),  //
                Range(90, 39),  //
                Range(100, 35)  //
              });

  // Purple
  defineColor(RandomColor::GetColorName(Color::PURPLE), Range(258, 282),
              {
                Range(20, 100), //
                Range(30, 87),  //
                Range(40, 79),  //
                Range(50, 70),  //
                Range(60, 65),  //
                Range(70, 59),  //
                Range(80, 52),  //
                Range(90, 45),  //
                Range(100, 42)  //
              });

  // Pink
  defineColor(RandomColor::GetColorName(Color::PINK), Range(283, 334),
              {
                Range(20, 100), //
                Range(30, 90),  //
                Range(40, 86),  //
                Range(60, 84),  //
                Range(80, 80),  //
                Range(90, 75),  //
                Range(100, 73), //
              });
}

} // end of namespace randomcolor
} // end of namespace BABYLON
