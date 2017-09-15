#ifndef BABYLON_LIGHTS_SPOT_LIGHT_H
#define BABYLON_LIGHTS_SPOT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/shadow_light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SpotLight : public ShadowLight {

public:
  template <typename... Ts>
  static SpotLight* New(Ts&&... args)
  {
    auto light = new SpotLight(::std::forward<Ts>(args)...);
    light->addToScene(static_cast<unique_ptr_t<Light>>(light));

    return light;
  }
  ~SpotLight();

  IReflect::Type type() const override;

  /**
   * @brief Returns the string "SpotLight".
   */
  const char* getClassName() const override;

  /**
   * @brief Returns the integer 2.
   */
  unsigned int getTypeID() const override;

  float angle() const;
  void setAngle(float value);

  /**
   * @brief Allows scaling the angle of the light for shadow generation only.
   */
  float shadowAngleScale() const;

  /**
   * @brief Allows scaling the angle of the light for shadow generation only.
   */
  void setShadowAngleScale(float value);

  /**
   * @brief Sets the passed Effect object with the SpotLight transfomed position
   * (or position if not parented) and normalized direction.
   */
  void transferToEffect(Effect* effect, const string_t& lightIndex) override;

protected:
  /**
   * @brief Creates a SpotLight object in the scene with the passed parameters :
   * - `position` (Vector3) is the initial SpotLight position,
   * - `direction` (Vector3) is the initial SpotLight direction,
   * - `angle` (float, in radians) is the spot light cone angle,
   * - `exponent` (float) is the light decay speed with the distance from the
   *                      emission spot.
   * A spot light is a simply light oriented cone.
   * It can cast shadows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  SpotLight(const string_t& name, const Vector3& position,
            const Vector3& direction, float angle, float exponent,
            Scene* scene);

  /**
   * @brief Sets the passed matrix "matrix" as perspective projection matrix for
   * the shadows and the passed view matrix with the fov equal to the SpotLight
   * angle and and aspect ratio of 1.0.
   * @returns the SpotLight.
   */
  void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const vector_t<AbstractMesh*>& renderList) override;

  void _buildUniformLayout() override;

public:
  float exponent;

private:
  float _angle;
  Nullable<float> _shadowAngleScale;

}; // end of class SpotLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SPOT_LIGHT_H
