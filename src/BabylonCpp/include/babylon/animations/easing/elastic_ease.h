#ifndef BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
#define BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H

#include <memory>

#include <babylon/animations/easing/easing_function.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class ElasticEase;
using ElasticEasePtr = std::shared_ptr<ElasticEase>;

/**
 * @brief Easing function with an elastic shape (see link below).
 * @see https://easings.net/#easeInElastic
 * @see http://doc.babylonjs.com/babylon101/animations#easing-functions
 */
class BABYLON_SHARED_EXPORT ElasticEase : public EasingFunction {

public:
  template <typename... Ts>
  static ElasticEasePtr New(Ts&&... args)
  {
    return std::shared_ptr<ElasticEase>(new ElasticEase(std::forward<Ts>(args)...));
  }
  ~ElasticEase() override; // = default

  /**
   * @brief Hidden
   */
  [[nodiscard]] float easeInCore(float gradient) const override;

protected:
  /**
   * @brief Instantiates an elastic easing function.
   * @see https://easings.net/#easeInElastic
   * @param oscillations Defines the number of oscillations
   * @param springiness Defines the amplitude of the oscillations
   */
  ElasticEase(int oscillations = 3, int springiness = 3);

public:
  /**
   * Defines the number of oscillations
   */
  float oscillations;

  /**
   * Defines the amplitude of the oscillations
   */
  float springiness;

}; // end of class ElasticEase

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_EASING_ELASTIC_EASE_H
