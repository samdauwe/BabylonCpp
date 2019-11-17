#ifndef BABYLON_UTILS_RANDOM_COLOR_H
#define BABYLON_UTILS_RANDOM_COLOR_H

#include <array>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace randomcolor {

enum class SaturationType {
  RANDOM     = 0,
  MONOCHROME = 1,
  UNDEFINED  = 2
}; // end of enum class SaturationType

enum class Luminosity {
  BRIGHT    = 0,
  LIGHT     = 1,
  DARK      = 2,
  RANDOM    = 3,
  UNDEFINED = 4
}; // end of enum class Luminosity

enum class Color {
  MONOCHROME = 0,
  RED        = 1,
  ORANGE     = 2,
  YELLOW     = 3,
  GREEN      = 4,
  BLUE       = 5,
  PURPLE     = 6,
  PINK       = 7
}; // end of enum class Color

class BABYLON_SHARED_EXPORT Range {

public:
  Range();
  Range(float start, float end);
  Range(const Range& otherRange);            // Copy constructor
  Range(Range&& otherRange);                 // Move constructor
  Range& operator=(const Range& otherRange); // Copy assignment operator
  Range& operator=(Range&& otherRange);      // Move assignment operator
  ~Range();                                  // = default
  friend std::ostream& operator<<(std::ostream& os, const Range& range);

  bool contains(float value);
  [[nodiscard]] float start() const;
  [[nodiscard]] float end() const;

private:
  float _start;
  float _end;

}; // end of class Range

class BABYLON_SHARED_EXPORT ColorInfo {

public:
  ColorInfo();
  ColorInfo(const Range& hueRange, const Range& saturationRange, const Range& brightnessRange,
            const std::vector<Range>& lowerBounds);
  ColorInfo(const ColorInfo& other);            // Copy constructor
  ColorInfo(ColorInfo&& other);                 // Move constructor
  ColorInfo& operator=(const ColorInfo& other); // Copy assignment operator
  ColorInfo& operator=(ColorInfo&& other);      // Move assignment operator
  ~ColorInfo();                                 // = default

  [[nodiscard]] bool hasHueRange() const;
  Range& getHueRange();
  [[nodiscard]] const Range& getHueRange() const;
  void setHueRange(const Range& hueRange);
  Range& getSaturationRange();
  [[nodiscard]] const Range& getSaturationRange() const;
  void setSaturationRange(const Range& saturationRange);
  Range& getBrightnessRange();
  [[nodiscard]] const Range& getBrightnessRange() const;
  void setBrightnessRange(const Range& brightnessRange);
  std::vector<Range>& getLowerBounds();
  [[nodiscard]] const std::vector<Range>& getLowerBounds() const;
  void setLowerBounds(const std::vector<Range>& lowerBounds);
  [[nodiscard]] bool isValid() const;

private:
  Range _hueRange;
  bool _hasHueRange;
  Range _saturationRange;
  Range _brightnessRange;
  std::vector<Range> _lowerBounds;
  bool _isValid;

}; // end of class ColorInfo

class BABYLON_SHARED_EXPORT Options {

public:
  Options();
  ~Options();

  [[nodiscard]] int getHue() const;
  void setHue(int hue);
  [[nodiscard]] SaturationType getSaturationType() const;
  void setSaturationType(const SaturationType& saturationType);
  [[nodiscard]] Luminosity getLuminosity() const;
  void setLuminosity(const Luminosity& luminosity);

private:
  int _hue;
  SaturationType _saturationType;
  Luminosity _luminosity;

}; // end of class Options

/**
 * @brief A class for generating attractive random colors. Port of ran­dom­Color
 * (A color gen­er­a­tor for JavaScript) to C++.
 * ref: https://github.com/davidmerfield/randomColor
 *      https://github.com/lzyzsd/AndroidRandomColor
 */
class BABYLON_SHARED_EXPORT RandomColor {

public:
  RandomColor();
  ~RandomColor();

  /** Statics */
  /**
   * Convert HSV to RGB color space
   *     Hue component, range: [0 .. 360)
   *     Saturation component, range: [0...1]
   *     Value component, range: [0...1]
   * If hsv values are out of range, they are pinned.
   *
   * HSV to RGB conversion formula:
   * (ref: http://www.rapidtables.com/convert/color/hsv-to-rgb.htm)
   *
   *  When 0 <= H < 360, 0 <=S <= 1 and 0 <= V <= 1:
   *   C = V × S
   *   X = C × (1 - |(H / 60deg) mod 2 - 1|)
   *   m = V - C
   *            / (C,X,0) ,   0deg <= H <  60deg
   *            | (X,C,0) ,  60deg <= H < 120deg
   *            | (0,C,X) , 120deg <= H < 180deg
   *   (R,G,B) <| (0,X,C) , 180deg <= H < 240deg
   *            | (X,0,C) , 240deg <= H < 300deg
   *            \ (C,0,X) , 300deg <= H < 360deg
   *   (R,G,B) = ((R'+m)x255, (G'+m)x255, (B'+m)x255)
   *
   * @param hsv  3 element array which holds the input HSV components.
   * @return the resulting rgb color as array,
   *         each component in range: [0 .. 255)
   */
  static std::array<unsigned int, 3> HSVToRGB(float hue, float saturation, float value);
  static std::string GetColorName(const Color& color);

  /** Member functions */
  std::array<unsigned int, 3> randomColor();
  std::array<unsigned int, 3> randomColor(float value, const SaturationType& saturationType,
                                          const Luminosity& luminosity);
  std::vector<std::array<unsigned int, 3>> randomColor(int count);
  std::array<unsigned int, 3> randomColor(const Color& color);
  std::vector<std::array<unsigned int, 3>> random(const Color& color, int count);
  void defineColor(const std::string& name, const std::vector<Range>& lowerBounds);
  void defineColor(const std::string& name, const Range& hueRange,
                   const std::vector<Range>& lowerBounds);

private:
  std::array<unsigned int, 3> getColor(float hue, float saturation, float brightness);
  float pickHue(float hue);
  float doPickHue(const Range& hueRange);
  float pickHue(const std::string& name);
  Range getHueRange(float number);
  Range getHueRange(const std::string& name);
  float pickSaturation(float hue, const SaturationType& saturationType,
                       const Luminosity& luminosity);
  float pickSaturation(const Color& color, const SaturationType& saturationType,
                       const Luminosity& luminosity);
  float pickSaturation(const ColorInfo& colorInfo, const SaturationType& saturationType,
                       const Luminosity& luminosity);
  float pickBrightness(float hue, float saturation, const Luminosity& luminosity);
  float pickBrightness(const Color& color, float saturation, const Luminosity& luminosity);
  float pickBrightness(const ColorInfo& colorInfo, float saturation, const Luminosity& luminosity);
  float getMinimumBrightness(const ColorInfo& colorInfo, float saturation);
  ColorInfo getColorInfo(float hue);
  float randomWithin(const Range& range);
  void loadColorBounds();

private:
  std::unordered_map<std::string, ColorInfo> _colors;

}; // end of class RandomColor

} // end of namespace randomcolor
} // end of namespace BABYLON

#endif // end of BABYLON_UTILS_RANDOM_COLOR_H
