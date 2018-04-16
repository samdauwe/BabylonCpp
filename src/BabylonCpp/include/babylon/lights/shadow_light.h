#ifndef BABYLON_LIGHTS_SHADOW_LIGHT_H
#define BABYLON_LIGHTS_SHADOW_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

/**
 * @brief Base implementation IShadowLight.
 * It groups all the common behaviour in order to reduce dupplication and better
 * follow the DRY pattern.
 */
class BABYLON_SHARED_EXPORT ShadowLight : public IShadowLight {

public:
  ShadowLight(const string_t& name, Scene* scene);
  ~ShadowLight() override;

  /**
   * @brief Gets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  Vector3& position();

  /**
   * @brief Sets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  void setPosition(const Vector3& value);

  /**
   * @brief In 2d mode (needCube being false), gets the direction used to cast
   * the shadow. Also use as the light direction on spot and directional lights.
   */
  Vector3& direction() override;

  /**
   * @brief In 2d mode (needCube being false), sets the direction used to cast
   * the shadow. Also use as the light direction on spot and directional lights.
   */
  void setDirection(const Vector3& value);

  /**
   * @brief Gets the transformed position. Position of the light in world space
   * taking parenting in account.
   */
  Vector3& transformedPosition() override;

  /**
   * @brief Gets the transformed direction. Direction of the light in world
   * space taking parenting in account.
   */
  Vector3& transformedDirection() override;

  /**
   * @brief Gets the shadow projection clipping minimum z value.
   */
  const Nullable<float>& shadowMinZ() const;

  /**
   * @brief Sets the shadow projection clipping minimum z value.
   */
  void setShadowMinZ(float value);

  /**
   * @brief Sets the shadow projection clipping maximum z value.
   */
  const Nullable<float>& shadowMaxZ() const;

  /**
   * @brief Gets the shadow projection clipping maximum z value.
   */
  void setShadowMaxZ(float value);

  /**
   * @brief Computes the transformed information (transformedPosition and
   * transformedDirection in World space) of the current light
   * @returns true if the information has been computed, false if it does not
   * need to (no parenting)
   */
  bool computeTransformedInformation() override;

  /**
   * @brief Gets the scene the light belongs to.
   * @returns The scene
   */
  Scene* getScene() override;

  /**
   * @brief Return the depth scale used for the shadow map.
   * @returns the depth scale.
   */
  float getDepthScale() const override;

  /**
   * @brief Get the direction to use to render the shadow map. In case of cube
   * texture, the face index can be passed.
   * @param faceIndex The index of the face we are computed the direction to
   * generate shadow
   * @returns The set direction in 2d mode otherwise the direction to the
   * cubemap face if needCube() is true
   */
  Vector3 getShadowDirection(unsigned int faceIndex) override;

  /**
   * @brief Returns the ShadowLight absolute position in the World.
   * @returns the position vector in world space
   */
  Vector3 getAbsolutePosition() override;

  /**
   * @brief Sets the ShadowLight direction toward the passed target.
   * @param target The point tot target in local space
   * @returns the updated ShadowLight direction
   */
  Vector3 setDirectionToTarget(const Vector3& target);

  /**
   * @brief Returns the light rotation in euler definition.
   * @returns the x y z rotation in local space.
   */
  Vector3 getRotation();

  /**
   * @brief Returns whether or not the shadow generation require a cube texture
   * or a 2d texture.
   * @returns true if a cube texture needs to be use
   */
  bool needCube() const override;

  /**
   * @brief Detects if the projection matrix requires to be recomputed this
   * frame.
   * @returns true if it requires to be recomputed otherwise, false.
   */
  bool needProjectionMatrixCompute() override;

  /**
   * @brief Forces the shadow generator to recompute the projection matrix even
   * if position and direction did not changed.
   */
  void forceProjectionMatrixCompute() override;

  /**
   * @brief Get the world matrix of the sahdow lights.
   * Internal Use Only
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Gets the minZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera The camera we are returning the min for
   * @returns the depth min z
   */
  float getDepthMinZ(Camera* activeCamera) const override;

  /**
   * @brief Gets the maxZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera The camera we are returning the max for
   * @returns the depth max z
   */
  float getDepthMaxZ(Camera* activeCamera) const override;

  /**
   * @brief Sets the shadow projection matrix in parameter to the generated
   * projection matrix.
   * @param matrix The materix to updated with the projection information
   * @param viewMatrix The transform matrix of the light
   * @param renderList The list of mesh to render in the map
   * @returns The current light
   */
  IShadowLight*
  setShadowProjectionMatrix(Matrix& matrix, Matrix& viewMatrix,
                            const vector_t<AbstractMesh*>& renderList) override;

protected:
  virtual void
  _setDefaultShadowProjectionMatrix(Matrix& matrix, const Matrix& viewMatrix,
                                    const vector_t<AbstractMesh*>& renderList)
    = 0;

  virtual void _setDirection(const Vector3& value);

  /**
   * @brief Sets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  virtual void _setPosition(const Vector3& value);

public:
  /**
   * Callback defining a custom Projection Matrix Builder.
   * This can be used to override the default projection matrix computation.
   */
  ::std::function<void(const Matrix& viewMatrix,
                       const vector_t<AbstractMesh*>& renderList,
                       Matrix& result)>
    customProjectionMatrixBuilder;

protected:
  Vector3 _position;
  unique_ptr_t<Vector3> _direction;

  /**
   * The transformed position. Position of the light in world space taking
   * parenting in account.
   */
  unique_ptr_t<Vector3> _transformedPosition;

  /**
   * The transformed direction. Direction of the light in world space taking
   * parenting in account.
   */
  unique_ptr_t<Vector3> _transformedDirection;

private:
  Nullable<float> _shadowMinZ;
  Nullable<float> _shadowMaxZ;
  unique_ptr_t<Matrix> _worldMatrix;
  bool _needProjectionMatrixCompute;

}; // end of class ShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOW_LIGHT_H
