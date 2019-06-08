#ifndef BABYLON_LIGHTS_SHADOW_LIGHT_H
#define BABYLON_LIGHTS_SHADOW_LIGHT_H

#include <babylon/babylon_api.h>
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
  ShadowLight(const std::string& name, Scene* scene);
  ~ShadowLight() override;

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
   * @brief Computes the world matrix of the node
   * @param force defines if the cache version should be invalidated forcing the
   * world matrix to be created from scratch
   * @returns the world matrix
   */
  bool computeTransformedInformation() override;

  /**
   * @brief Gets the scene the light belongs to.
   * @returns The scene
   */
  Scene* getScene() const override;

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
   * @param target The point to target in local space
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
   * @brief Hidden
   */
  void _initCache() override;

  /**
   * @brief Hidden
   */
  bool _isSynchronized() override;

  /**
   * @brief Computes the world matrix of the node
   * @param force defines if the cache version should be invalidated forcing the
   * world matrix to be created from scratch
   * @returns the world matrix
   */
  Matrix& computeWorldMatrix(bool force             = false,
                             bool useWasUpdatedFlag = false) override;

  /**
   * @brief Gets the minZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera The camera we are returning the min for
   * @returns the depth min z
   */
  float getDepthMinZ(const Camera& activeCamera) const override;

  /**
   * @brief Gets the maxZ used for shadow according to both the scene and the
   * light.
   * @param activeCamera The camera we are returning the max for
   * @returns the depth max z
   */
  float getDepthMaxZ(const Camera& activeCamera) const override;

  /**
   * @brief Sets the shadow projection matrix in parameter to the generated
   * projection matrix.
   * @param matrix The materix to updated with the projection information
   * @param viewMatrix The transform matrix of the light
   * @param renderList The list of mesh to render in the map
   * @returns The current light
   */
  IShadowLight* setShadowProjectionMatrix(
    Matrix& matrix, Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;

protected:
  virtual void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList)
    = 0;

  /**
   * @brief Sets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  virtual void _setPosition(const Vector3& value);

  /**
   * @brief Gets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  Vector3& get_position() override;

  /**
   * @brief Sets the position the shadow will be casted from. Also use as the
   * light position for both point and spot lights.
   */
  void set_position(const Vector3& value) override;

  virtual void _setDirection(const Vector3& value);

  /**
   * @brief In 2d mode (needCube being false), gets the direction used to cast
   * the shadow. Also use as the light direction on spot and directional lights.
   */
  Vector3& get_direction() override;

  /**
   * @brief In 2d mode (needCube being false), sets the direction used to cast
   * the shadow. Also use as the light direction on spot and directional lights.
   */
  void set_direction(const Vector3& value) override;

  /**
   * @brief Gets the shadow projection clipping minimum z value.
   */
  std::optional<float>& get_shadowMinZ();

  /**
   * @brief Sets the shadow projection clipping minimum z value.
   */
  void set_shadowMinZ(const std::optional<float>& value);

  /**
   * @brief Sets the shadow projection clipping maximum z value.
   */
  std::optional<float>& get_shadowMaxZ();

  /**
   * @brief Gets the shadow projection clipping maximum z value.
   */
  void set_shadowMaxZ(const std::optional<float>& value);

public:
  /**
   * Callback defining a custom Projection Matrix Builder.
   * This can be used to override the default projection matrix computation.
   */
  std::function<void(const Matrix& viewMatrix,
                     const std::vector<AbstractMesh*>& renderList,
                     Matrix& result)>
    customProjectionMatrixBuilder;

  /**
   * The shadow projection clipping minimum z value.
   */
  Property<ShadowLight, std::optional<float>> shadowMinZ;

  /**
   * The shadow projection clipping maximum z value.
   */
  Property<ShadowLight, std::optional<float>> shadowMaxZ;

protected:
  Vector3 _position;
  std::unique_ptr<Vector3> _direction;

  /**
   * The transformed position. Position of the light in world space taking
   * parenting in account.
   */
  std::unique_ptr<Vector3> _transformedPosition;

  /**
   * The transformed direction. Direction of the light in world space taking
   * parenting in account.
   */
  std::unique_ptr<Vector3> _transformedDirection;

private:
  std::optional<float> _shadowMinZ;
  std::optional<float> _shadowMaxZ;
  bool _needProjectionMatrixCompute;

}; // end of class ShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOW_LIGHT_H
