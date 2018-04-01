#ifndef BABYLON_LIGHTS_SPOT_LIGHT_H
#define BABYLON_LIGHTS_SPOT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/shadow_light.h>

namespace BABYLON {

/**
 * @brief A spot light is defined by a position, a direction, an angle, and an
 * exponent. These values define a cone of light starting from the position,
 * emitting toward the direction. The angle, in radians, defines the size (field
 * of illumination) of the spotlight's conical beam, and the exponent defines
 * the speed of the decay of the light with distance (reach). Documentation:
 * https://doc.babylonjs.com/babylon101/lights
 */
class BABYLON_SHARED_EXPORT SpotLight : public ShadowLight {

public:
  template <typename... Ts>
  static SpotLight* New(Ts&&... args)
  {
    auto light = new SpotLight(::std::forward<Ts>(args)...);
    light->addToScene(static_cast<unique_ptr_t<Light>>(light));

    return light;
  }
  ~SpotLight() override;

  IReflect::Type type() const override;

  /**
   * @brief Returns the string "SpotLight".
   * @returns the class name
   */
  const string_t getClassName() const override;

  /**
   * @brief Returns the integer 2.
   * @returns The light Type id as a constant defines in Light.LIGHTTYPEID_x
   */
  unsigned int getTypeID() const override;

  /**
   * @brief Gets the cone angle of the spot light in Radians.
   */
  float angle() const;

  /**
   * @brief Sets the cone angle of the spot light in Radians.
   */
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
   * @brief Allows reading the projecton texture.
   */
  Matrix& projectionTextureMatrix();
  const Matrix& projectionTextureMatrix() const;

  /**
   * @brief Gets the near clip of the Spotlight for texture projection.
   */
  float projectionTextureLightNear() const;
  /**
   * @brief Sets the near clip of the Spotlight for texture projection.
   */
  void setProjectionTextureLightNear(float value);

  /**
   * @brief Gets the far clip of the Spotlight for texture projection.
   */
  float projectionTextureLightFar() const;

  /**
   * @brief Sets the far clip of the Spotlight for texture projection.
   */
  void setProjectionTextureLightFar(float value);

  /**
   * @brief Gets the Up vector of the Spotlight for texture projection.
   */
  Vector3& projectionTextureUpDirection();
  const Vector3& projectionTextureUpDirection() const;

  /**
   * @brief Sets the Up vector of the Spotlight for texture projection.
   */
  void setProjectionTextureUpDirection(const Vector3& value);

  /**
   * @brief Gets the projection texture of the light.
   */
  BaseTexture* projectionTexture() const;
  /**
   * @brief Sets the projection texture of the light.
   */
  void setProjectionTexture(BaseTexture* value);

  /**
   * @brief Sets the passed Effect object with the SpotLight transfomed position
   * (or position if not parented) and normalized direction.
   * @param effect The effect to update
   * @param lightIndex The index of the light in the effect to update
   * @returns The spot light
   */
  void transferToEffect(Effect* effect, const string_t& lightIndex) override;

  /**
   * @brief Disposes the light and the associated resources.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Creates a SpotLight object in the scene. A spot light is a simply
   * light oriented cone. It can cast shadows. Documentation :
   * http://doc.babylonjs.com/tutorials/lights
   * @param name The light friendly name
   * @param position The position of the spot light in the scene
   * @param direction The direction of the light in the scene
   * @param angle The cone angle of the light in Radians
   * @param exponent The light decay speed with the distance from the emission
   * spot
   * @param scene The scene the lights belongs to
   */
  SpotLight(const string_t& name, const Vector3& position,
            const Vector3& direction, float angle, float exponent,
            Scene* scene);

  /**
   * @brief Overrides the direction setter to recompute the projection texture
   * view light Matrix.
   */
  void _setDirection(const Vector3& value) override;

  /**
   * @brief Overrides the position setter to recompute the projection texture
   * view light Matrix.
   */
  void _setPosition(const Vector3& value) override;

  /**
   * @brief Sets the passed matrix "matrix" as perspective projection matrix for
   * the shadows and the passed view matrix with the fov equal to the SpotLight
   * angle and and aspect ratio of 1.0.
   * @returns the SpotLight.
   */
  void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const vector_t<AbstractMesh*>& renderList) override;

  void _computeProjectionTextureViewLightMatrix();
  void _computeProjectionTextureProjectionLightMatrix();

  /**
   * @brief Main function for light texture projection matrix computing.
   */
  void _computeProjectionTextureMatrix();

  void _buildUniformLayout() override;

public:
  /**
   * The light decay speed with the distance from the emission spot.
   */
  float exponent;

protected:
  float _projectionTextureLightNear;
  float _projectionTextureLightFar;
  Vector3 _projectionTextureUpDirection;

private:
  /**
   * upVector , rightVector and direction will form the coordinate system for
   * this spot light. These three vectors will be used as projection matrix when
   * doing texture projection.
   *
   * Also we have the following rules always holds:
   * direction cross up   = right
   * right cross direction = up
   * up cross right       = forward
   *
   * light_near and light_far will control the range of the texture projection.
   * If a plane is out of the range in spot light space, there is no texture
   * projection.
   */
  float _angle;
  Nullable<float> _shadowAngleScale;
  Matrix _projectionTextureMatrix;
  BaseTexture* _projectionTexture;
  bool _projectionTextureViewLightDirty;
  bool _projectionTextureProjectionLightDirty;
  bool _projectionTextureDirty;
  Vector3 _projectionTextureViewTargetVector;
  Matrix _projectionTextureViewLightMatrix;
  Matrix _projectionTextureProjectionLightMatrix;
  Matrix _projectionTextureScalingMatrix;

}; // end of class SpotLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SPOT_LIGHT_H
