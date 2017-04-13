#ifndef BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
#define BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DirectionalLight : public Light,
                                               public IShadowLight {

public:
  template <typename... Ts>
  static DirectionalLight* New(Ts&&... args)
  {
    auto light = new DirectionalLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~DirectionalLight();

  /**
   * @brief Returns the string "DirectionalLight".
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;
  Scene* getScene() override;

  /**
   * @brief Returns the DirectionalLight absolute position in the World.
   */
  Vector3 getAbsolutePosition() override;

  /**
   * @brief Sets the DirectionalLight direction toward the passed target
   * (Vector3).
   * @returns The updated DirectionalLight direction (Vector3).
   */
  Vector3& setDirectionToTarget(Vector3& target);

  /**
   * @brief Sets the passed matrix "matrix" as projection matrix for the shadows
   * cast by the light according to the passed view matrix.
   * @returns The DirectionalLight.
   */
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

  /**
   * @brief Returns true by default.
   */
  bool needRefreshPerFrame() const override;

  /**
   * @brief Returns false by default.
   */
  bool needCube() const override;

  /**
   * @brief Returns the light direction (Vector3) for any passed face index.
   */
  Vector3 getShadowDirection(unsigned int faceIndex) override;

  /**
   * @brief Computes the light transformed position in case the light is
   * parented. Returns true if parented, else false.
   */
  bool computeTransformedPosition() override;

  /**
   * @brief Sets the passed Effect object with the DirectionalLight transformed
   * position (or position if not parented) and the passed name.
   */
  void transferToEffect(Effect* effect,
                        const std::string& directionUniformName) override;

  /**
   * @brief Returns the light world matrix.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Returns the integer 1.
   */
  unsigned int getTypeID() const override;

protected:
  /**
   * @brief Creates a DirectionalLight object in the scene, oriented towards the
   * passed direction (Vector3).
   * The directional light is emitted from everywhere in the given direction.
   * It can cast shawdows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  DirectionalLight(const std::string& name, const Vector3& direction,
                   Scene* scene);

public:
  Vector3 position;
  Vector3 direction;
  std::unique_ptr<Vector3> transformedPosition;
  float shadowOrthoScale;
  bool autoUpdateExtends;
  Nullable<float> shadowMinZ;
  Nullable<float> shadowMaxZ;

private:
  std::unique_ptr<Vector3> _transformedDirection;
  std::unique_ptr<Matrix> _worldMatrix;
  // Cache
  float _orthoLeft;
  float _orthoRight;
  float _orthoTop;
  float _orthoBottom;

}; // end of class DirectionalLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
