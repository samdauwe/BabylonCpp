#ifndef BABYLON_TOOLS_COLOR_GRADIENT_H
#define BABYLON_TOOLS_COLOR_GRADIENT_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>
#include <babylon/tools/ivalue_gradient.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief Class used to store color gradient.
 */
class BABYLON_SHARED_EXPORT ColorGradient : public IValueGradient {

public:
  ColorGradient();
  ~ColorGradient();

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
  Nullable<Color4> color2;

}; // end of class ColorGradient

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_COLOR_GRADIENT_H
