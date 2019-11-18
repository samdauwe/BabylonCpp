#ifndef BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
#define BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BezierCurveEase;
using BezierCurveEasePtr = std::shared_ptr<BezierCurveEase>;

/**
 * @brief Easing function with a bezier shape (see link below).
 * @see http://cubic-bezier.com/#.17,.67,.83,.67
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT BezierCurveEase : public EasingFunction {

public:
  template <typename... Ts>
  static BezierCurveEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<BezierCurveEase>(new BezierCurveEase(std::forward<Ts>(args)...));
  }
  ~BezierCurveEase() override; // = default

  /**
   * @brief Hidden
   */
  [[nodiscard]] float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates a bezier function.
   * @see http://cubic-bezier.com/#.17,.67,.83,.67
   * @param x1 Defines the x component of the start tangent in the bezier curve
   * @param y1 Defines the y component of the start tangent in the bezier curve
   * @param x2 Defines the x component of the end tangent in the bezier curve
   * @param y2 Defines the y component of the end tangent in the bezier curve
   */
  BezierCurveEase(float x1 = 0.f, float y1 = 0.f, float x2 = 1.f, float y2 = 1.f);

public:
  /**
   * Defines the x component of the start tangent in the bezier curve
   */
  float x1;

  /**
   * Defines the y component of the start tangent in the bezier curve
   */
  float y1;

  /**
   * Defines the x component of the end tangent in the bezier curve
   */
  float x2;

  /**
   * Defines the y component of the end tangent in the bezier curve
   */
  float y2;

}; // end of class BezierCurveEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_BEZIER_CURVE_EASE_H
