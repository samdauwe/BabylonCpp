#ifndef BABYLON_LIGHTS_SPOT_LIGHT_H
#define BABYLON_LIGHTS_SPOT_LIGHT_H

#include <babylon/babylon_api.h>
#include <babylon/lights/shadow_light.h>

namespace BABYLON {

class BaseTexture;
class SpotLight;
using SpotLightPtr   = std::shared_ptr<SpotLight>;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

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
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static SpotLightPtr New(Ts&&... args)
  {
    auto light
      = std::shared_ptr<SpotLight>(new SpotLight(std::forward<Ts>(args)...));
    light->addToScene(light);

    return light;
  }
  ~SpotLight(); // = default

  Type type() const override;

  /**
   * @brief Returns the string "SpotLight".
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Returns the integer 2.
   * @returns The light Type id as a constant defines in Light.LIGHTTYPEID_x
   */
  unsigned int getTypeID() const override;

  /**
   * @brief Sets the passed Effect object with the SpotLight transfomed position
   * (or position if not parented) and normalized direction.
   * @param effect The effect to update
   * @param lightIndex The index of the light in the effect to update
   * @returns The spot light
   */
  void transferToEffect(const EffectPtr& effect,
                        const std::string& lightIndex) override;

  /**
   * @brief Sets the passed Effect "effect" with the Light information.
   * @param effect The effect to update
   * @param lightDataUniformName The uniform used to store light data (position
   * or direction)
   * @returns The light
   */
  SpotLight& transferToNodeMaterialEffect(
    const EffectPtr& effect, const std::string& lightDataUniformName) override;

  /**
   * @brief Disposes the light and the associated resources.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Prepares the list of defines specific to the light type.
   * @param defines the list of defines
   * @param lightIndex defines the index of the light for the effect
   */
  void prepareLightSpecificDefines(MaterialDefines& defines,
                                   unsigned int lightIndex) override;

protected:
  /**
   * @brief Creates a SpotLight object in the scene. A spot light is a simply
   * light oriented cone. It can cast shadows. Documentation :
   * https://doc.babylonjs.com/babylon101/lights
   * @param name The light friendly name
   * @param position The position of the spot light in the scene
   * @param direction The direction of the light in the scene
   * @param angle The cone angle of the light in Radians
   * @param exponent The light decay speed with the distance from the emission
   * spot
   * @param scene The scene the lights belongs to
   */
  SpotLight(const std::string& name, const Vector3& position,
            const Vector3& direction, float angle, float exponent,
            Scene* scene);

  /**
   * @brief Gets the cone angle of the spot light in Radians.
   */
  float get_angle() const;

  /**
   * @brief Sets the cone angle of the spot light in Radians.
   */
  void set_angle(float value);

  /**
   * @brief Only used in gltf falloff mode, this defines the angle where
   * the directional falloff will start before cutting at angle which could be
   * seen as outer angle.
   */
  float get_innerAngle() const;

  /**
   * @brief Only used in gltf falloff mode, this defines the angle where
   * the directional falloff will start before cutting at angle which could be
   * seen as outer angle.
   */
  void set_innerAngle(float value);

  /**
   * @brief Allows scaling the angle of the light for shadow generation only.
   */
  float get_shadowAngleScale() const;

  /**
   * @brief Allows scaling the angle of the light for shadow generation only.
   */
  void set_shadowAngleScale(float value);

  /**
   * @brief Allows reading the projecton texture.
   */
  Matrix& get_projectionTextureMatrix();

  /**
   * @brief Gets the near clip of the Spotlight for texture projection.
   */
  float get_projectionTextureLightNear() const;

  /**
   * @brief Sets the near clip of the Spotlight for texture projection.
   */
  void set_projectionTextureLightNear(float value);

  /**
   * @brief Gets the far clip of the Spotlight for texture projection.
   */
  float get_projectionTextureLightFar() const;

  /**
   * @brief Sets the far clip of the Spotlight for texture projection.
   */
  void set_projectionTextureLightFar(float value);

  /**
   * @brief Gets the Up vector of the Spotlight for texture projection.
   */
  Vector3& get_projectionTextureUpDirection();

  /**
   * @brief Sets the Up vector of the Spotlight for texture projection.
   */
  void set_projectionTextureUpDirection(const Vector3& value);

  /**
   * @brief Gets the projection texture of the light.
   */
  BaseTexturePtr& get_projectionTexture();

  /**
   * @brief Sets the projection texture of the light.
   */
  void set_projectionTexture(const BaseTexturePtr& value);

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
    const std::vector<AbstractMesh*>& renderList) override;

  void _computeProjectionTextureViewLightMatrix();
  void _computeProjectionTextureProjectionLightMatrix();

  /**
   * @brief Main function for light texture projection matrix computing.
   */
  void _computeProjectionTextureMatrix();

  void _buildUniformLayout() override;

private:
  void _computeAngleValues();

public:
  /**
   * The light decay speed with the distance from the emission spot
   */
  float exponent;

  /**
   * The cone angle of the spot light in Radians
   */
  Property<SpotLight, float> angle;

  /**
   * Only used in gltf falloff mode, this defines the angle where the
   * directional falloff will start before cutting at angle which could be seen
   * as outer angle.
   */
  Property<SpotLight, float> innerAngle;

  /**
   * Allows scaling the angle of the light for shadow generation only
   */
  Property<SpotLight, float> shadowAngleScale;

  /**
   * Allows reading the projecton texture
   */
  ReadOnlyProperty<SpotLight, Matrix> projectionTextureMatrix;

  /**
   * The near clip of the Spotlight for texture projection
   */
  Property<SpotLight, float> projectionTextureLightNear;

  /**
   * The far clip of the Spotlight for texture projection
   */
  Property<SpotLight, float> projectionTextureLightFar;

  /**
   * The Up vector of the Spotlight for texture projection
   */
  Property<SpotLight, Vector3> projectionTextureUpDirection;

  /**
   * The projection texture of the light
   */
  Property<SpotLight, BaseTexturePtr> projectionTexture;

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
  float _innerAngle;
  float _cosHalfAngle;
  float _lightAngleScale;
  float _lightAngleOffset;

  std::optional<float> _shadowAngleScale;
  Matrix _projectionTextureMatrix;
  BaseTexturePtr _projectionTexture;
  bool _projectionTextureViewLightDirty;
  bool _projectionTextureProjectionLightDirty;
  bool _projectionTextureDirty;
  Vector3 _projectionTextureViewTargetVector;
  Matrix _projectionTextureViewLightMatrix;
  Matrix _projectionTextureProjectionLightMatrix;
  Matrix _projectionTextureScalingMatrix;

  static bool NodeConstructorAdded;

}; // end of class SpotLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SPOT_LIGHT_H
