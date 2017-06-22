#ifndef BABYLON_LIGHTS_SHADOW_LIGHT_H
#define BABYLON_LIGHTS_SHADOW_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShadowLight : public IShadowLight {

public:
  ShadowLight(const std::string& name, Scene* scene);
  ~ShadowLight();

  Vector3& direction();
  void setDirection(const Vector3& value);
  const Nullable<float>& shadowMinZ() const;
  void setShadowMinZ(float value);
  const Nullable<float>& shadowMaxZ() const;
  void setShadowMaxZ(float value);

  /**
   * @brief Computes the light transformed position/direction in case the light
   * is parented. Returns true if parented, else false.
   */
  bool computeTransformedInformation() override;

  Scene* getScene() override;

  /**
   * Return the depth scale used for the shadow map.
   */
  float getDepthScale() const override;

  /**
   * @brief Returns the light direction (Vector3) for any passed face index.
   */
  Vector3 getShadowDirection(unsigned int faceIndex) override;

  /**
   * @brief Returns the DirectionalLight absolute position in the World.
   */
  Vector3 getAbsolutePosition() override;

  /**
   * @brief Sets the DirectionalLight direction toward the passed target
   * (Vector3).
   * @returns The updated DirectionalLight direction (Vector3).
   */
  Vector3 setDirectionToTarget(const Vector3& target);

  /**
   * @brief Returns the light rotation (Vector3).
   */
  Vector3 getRotation();

  /**
   * @brief Returns a boolean : false by default.
   */
  bool needCube() const override;

  /**
   * @brief Specifies wether or not the projection matrix should be recomputed
   * this frame.
   */
  bool needProjectionMatrixCompute() override;

  /**
   * @brief Forces the shadow generator to recompute the projection matrix even
   * if position and direction did not changed.
   */
  void forceProjectionMatrixCompute() override;

  /**
   * @brief Get the world matrix of the sahdow lights.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Sets the projection matrix according to the type of light and custom
   * projection matrix definition.
   * Returns the light.
   */
  IShadowLight* setShadowProjectionMatrix(
    Matrix& matrix, Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

protected:
  virtual void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList)
    = 0;

public:
  std::function<void(const Matrix& viewMatrix,
                     const std::vector<AbstractMesh*>& renderList,
                     Matrix& result)>
    customProjectionMatrixBuilder;
  std::unique_ptr<Vector3> transformedPosition;
  std::unique_ptr<Vector3> transformedDirection;

protected:
  std::unique_ptr<Vector3> _direction;

private:
  Nullable<float> _shadowMinZ;
  Nullable<float> _shadowMaxZ;
  std::unique_ptr<Matrix> _worldMatrix;
  bool _needProjectionMatrixCompute;

}; // end of class ShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOW_LIGHT_H
