#ifndef BABYLON_LIGHTS_SPOT_LIGHT_H
#define BABYLON_LIGHTS_SPOT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SpotLight : public Light, public IShadowLight {

public:
  template <typename... Ts>
  static SpotLight* New(Ts&&... args)
  {
    auto light = new SpotLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~SpotLight();

  /**
   * @brief Returns the string "SpotLight".
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;
  Scene* getScene() override;

  /**
   * @brief Returns the SpotLight absolute position in the World (Vector3).
   */
  Vector3 getAbsolutePosition() override;

  /**
   * @brief Sets the passed matrix "matrix" as perspective projection matrix for
   * the shadows and the passed view matrix with the fov equal to the SpotLight
   * angle and and aspect ratio of 1.0.
   */
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

  /**
   * @returns False by default.
   */
  bool needCube() const override;

  /**
   * @returns False by default.
   */
  bool needRefreshPerFrame() const override;

  /**
   * @brief Returns the SpotLight direction (Vector3) for any passed face index.
   */
  Vector3 getShadowDirection(unsigned int faceIndex = 0) override;

  /**
   * @brief Updates the SpotLight direction towards the passed target (Vector3).
   * @returns The updated direction.
   */
  Vector3& setDirectionToTarget(Vector3& target);

  /**
   * @brief Computes the SpotLight transformed position if parented.
   * @returns True if parented, else false.
   */
  bool computeTransformedPosition() override;

  /**
   * @brief Sets the passed Effect object with the SpotLight transfomed position
   * (or position if not parented) and normalized direction.
   */
  void transferToEffect(Effect* effect, const std::string& positionUniformName,
                        const std::string& directionUniformName) override;

  /**
   * @brief Returns the light World matrix.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Returns the integer 2.
   */
  unsigned int getTypeID() const override;

  /**
   * @brief Returns the SpotLight rotation (Vector3).
   */
  Vector3 getRotation();

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
  SpotLight(const std::string& name, const Vector3& position,
            const Vector3& direction, float angle, float exponent,
            Scene* scene);

public:
  std::unique_ptr<Vector3> transformedPosition;
  Vector3 position;
  Vector3 direction;
  float angle;
  float exponent;
  Nullable<float> shadowMinZ;
  Nullable<float> shadowMaxZ;

private:
  std::unique_ptr<Vector3> _transformedDirection;
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class PointLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SPOT_LIGHT_H
