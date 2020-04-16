#ifndef BABYLON_MISC_COLOR_GRADIENT_H
#define BABYLON_MISC_COLOR_GRADIENT_H

#include <babylon/babylon_api.h>
#include <babylon/maths/color4.h>
#include <babylon/misc/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store color4 gradient.
 */
class BABYLON_SHARED_EXPORT ColorGradient : public IValueGradient {

public:
  /**
   * @brief Creates a new color4 gradient.
   * @param gradient gets or sets the gradient value (between 0 and 1)
   * @param color1 gets or sets first associated color
   * @param color2 gets or sets first second color
   */
  ColorGradient(float gradient, const Color4& color1,
                const std::optional<Color4>& color2 = std::nullopt);
  ~ColorGradient(); // = default

  /**
   * @brief Will get a color picked randomly between color1 and color2.
   * If color2 is undefined then color1 will be used
   * @param result defines the target Color4 to store the result in
   */
  void getColorToRef(Color4& result);

public:
  /**
   * Gets or sets first associated color
   */
  Color4 color1;

  /**
   * Gets or sets second associated color
   */
  std::optional<Color4> color2;

}; // end of class ColorGradient

bool operator==(const ColorGradient& lhs, const ColorGradient& rhs);
bool operator!=(const ColorGradient& lhs, const ColorGradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_COLOR_GRADIENT_H
