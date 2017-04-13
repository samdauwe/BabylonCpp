#ifndef BABYLON_LIGHTS_POINT_LIGHT_H
#define BABYLON_LIGHTS_POINT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PointLight : public Light, public IShadowLight {

public:
  template <typename... Ts>
  static PointLight* New(Ts&&... args)
  {
    auto light = new PointLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~PointLight();

  /**
   * @brief Returns the string "PointLight".
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;
  Scene* getScene() override;

  /**
   * @brief Returns a Vector3, the PointLight absolute position in the World.
   */
  Vector3 getAbsolutePosition() override;

  /**
   * @brief Computes the PointLight transformed position if parented
   * @returns True if ok, false if not parented.
   */
  bool computeTransformedPosition() override;

  /**
   * @brief Sets the passed Effect "effect" with the PointLight transformed
   * position (or position, if none) and passed name (string).
   */
  void transferToEffect(Effect* effect,
                        const std::string& positionUniformName) override;

  /**
   * @returns True by default.
   */
  bool needCube() const override;

  /**
   * @returns False by default.
   */
  bool needRefreshPerFrame() const override;

  /**
   * @brief Returns a new Vector3 aligned with the PointLight cube system
   * according to the passed cube face index (integer).
   */
  Vector3 getShadowDirection(unsigned int faceIndex = 0) override;

  ShadowGenerator* getShadowGenerator() override;

  /**
   * @brief Sets the passed matrix "matrix" as a left-handed perspective
   * projection matrix with the following settings :
   * - fov = PI / 2
   * - aspect ratio : 1.0
   * - z-near and far equal to the active camera minZ and maxZ.
   */
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

  /**
   * @brief Returns the light world matrix.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Returns the integer 0.
   */
  unsigned int getTypeID() const override;

protected:
  /**
   * @brief Creates a PointLight object from the passed name and position
   * (Vector3) and adds it in the scene.
   * A PointLight emits the light in every direction.
   * It can cast shadows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  PointLight(const std::string& name, const Vector3& position, Scene* scene);

public:
  std::unique_ptr<Vector3> transformedPosition;
  Vector3 position;
  Nullable<float> shadowMinZ;
  Nullable<float> shadowMaxZ;

private:
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class PointLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_POINT_LIGHT_H
