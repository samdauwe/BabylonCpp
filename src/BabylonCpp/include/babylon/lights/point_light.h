#ifndef BABYLON_LIGHTS_POINT_LIGHT_H
#define BABYLON_LIGHTS_POINT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/shadow_light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PointLight : public ShadowLight {

public:
  template <typename... Ts>
  static PointLight* New(Ts&&... args)
  {
    auto light = new PointLight(::std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~PointLight();

  IReflect::Type type() const override;

  /**
   * @brief Getter: In case of direction provided, the shadow will not use a
   * cube texture but simulate a spot shadow as a fallback
   * This specifies what angle the shadow will use to be created.
   *
   * It default to 90 degrees to work nicely with the cube texture generation
   * for point lights shadow maps.
   */
  float shadowAngle() const;

  /**
   * @brief Setter: In case of direction provided, the shadow will not use a
   * cube texture but simulate a spot shadow as a fallback
   * This specifies what angle the shadow will use to be created.
   *
   * It default to 90 degrees to work nicely with the cube texture generation
   * for point lights shadow maps.
   */
  void setShadowAngle(float value);

  Vector3& direction() override;

  /**
   * @brief In case of direction provided, the shadow will not use a cube
   * texture but simulate a spot shadow as a fallback
   */
  void setDirection(const Vector3& value);

  /**
   * @brief Returns the string "PointLight".
   */
  const char* getClassName() const override;

  /**
   * @brief Returns the integer 0.
   */
  unsigned int getTypeID() const override;

  /**
   * @returns True by default.
   */
  bool needCube() const override;

  /**
   * @brief Returns a new Vector3 aligned with the PointLight cube system
   * according to the passed cube face index (integer).
   */
  Vector3 getShadowDirection(unsigned int faceIndex) override;

  /**
   * @brief Sets the passed Effect "effect" with the PointLight transformed
   * position (or position, if none) and passed name (string).
   * Returns the PointLight.
   */
  void transferToEffect(Effect* effect, const std::string& lightIndex) override;

protected:
  /**
   * @brief Creates a PointLight object from the passed name and position
   * (Vector3) and adds it in the scene.
   * A PointLight emits the light in every direction.
   * It can cast shadows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  PointLight(const std::string& name, const Vector3& position, Scene* scene);

  /**
   * @brief Sets the passed matrix "matrix" as a left-handed perspective
   * projection matrix with the following settings:
   * - fov = PI / 2
   * - aspect ratio : 1.0
   * - z-near and far equal to the active camera minZ and maxZ.
   * Returns the PointLight.
   */
  void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

  void _buildUniformLayout() override;

private:
  float _shadowAngle;

}; // end of class PointLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_POINT_LIGHT_H
