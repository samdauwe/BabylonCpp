#ifndef BABYLON_ISHADOW_LIGHT_H
#define BABYLON_ISHADOW_LIGHT_H

#include <babylon/babylon_api.h>
#include <babylon/lights/light.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class Camera;

/**
 * @brief Interface describing all the common properties and methods a shadow light needs to
 * implement. This helps both the shadow generator and materials to generate the corresponding
 * shadow maps as well as binding the different shadow properties to the effects.
 */
class BABYLON_SHARED_EXPORT IShadowLight : public Light {

public:
  IShadowLight(const std::string& name, Scene* scene);
  ~IShadowLight() override; // = default

  /**
   * The position the shadow will be casted from.
   */
  Property<IShadowLight, Vector3> position;

  /**
   * In 2d mode (needCube being false), the direction used to cast the shadow.
   */
  Property<IShadowLight, Vector3> direction;

  /**
   * @brief The transformed position. Position of the light in world space taking parenting in
   * account.
   */
  virtual Vector3& transformedPosition() = 0;

  /**
   * @brief The transformed direction. Direction of the light in world space taking parenting in
   * account.
   */
  virtual Vector3& transformedDirection() = 0;

  /**
   * @brief Computes the transformed information (transformedPosition and transformedDirection in
   * World space) of the current light
   * @returns true if the information has been computed, false if it does not need to (no parenting)
   */
  virtual bool computeTransformedInformation() = 0;

  /**
   * Gets the scene the light belongs to.
   * @returns The scene
   */
  Scene* getScene() const override = 0;

  /**
   * @brief Sets the shadow projection matrix in parameter to the generated projection matrix.
   * @param matrix The matrix to updated with the projection information
   * @param viewMatrix The transform matrix of the light
   * @param renderList The list of mesh to render in the map
   * @returns The current light
   */
  virtual IShadowLight* setShadowProjectionMatrix(Matrix& matrix, Matrix& viewMatrix,
                                                  const std::vector<AbstractMesh*>& renderList)
    = 0;

  /**
   * @brief Gets the current depth scale used in ESM.
   * @returns The scale
   */
  virtual float getDepthScale() const = 0;

  /**
   * @brief Returns whether or not the shadow generation require a cube texture or a 2d texture.
   * @returns true if a cube texture needs to be use
   */
  virtual bool needCube() const = 0;

  /**
   * @brief Detects if the projection matrix requires to be recomputed this frame.
   * @returns true if it requires to be recomputed otherwise, false.
   */
  virtual bool needProjectionMatrixCompute() = 0;

  /**
   * @brief Forces the shadow generator to recompute the projection matrix even if position and
   * direction did not changed.
   */
  virtual void forceProjectionMatrixCompute() = 0;

  /**
   * @brief Get the direction to use to render the shadow map. In case of cube texture, the face
   * index can be passed.
   * @param faceIndex The index of the face we are computed the direction to generate shadow
   * @returns The set direction in 2d mode otherwise the direction to the cubemap face if needCube()
   * is true
   */
  virtual Vector3 getShadowDirection(unsigned int faceIndex = 0) = 0;

  /**
   * @brief Gets the minZ used for shadow according to both the scene and the light.
   * @param activeCamera The camera we are returning the min for
   * @returns the depth min z
   */
  virtual float getDepthMinZ(const Camera& activeCamera) const = 0;

  /**
   * @brief Gets the maxZ used for shadow according to both the scene and the light.
   * @param activeCamera The camera we are returning the max for
   * @returns the depth max z
   */
  virtual float getDepthMaxZ(const Camera& activeCamera) const = 0;

protected:
  /**
   * @brief Gets the position the shadow will be casted from.
   */
  virtual Vector3& get_position() = 0;

  /**
   * @brief Sets the position the shadow will be casted from.
   */
  virtual void set_position(const Vector3& value) = 0;

  /**
   * @brief In 2d mode (needCube being false), gets the direction used to cast the shadow. Also use
   * as the light direction on spot and directional lights.
   */
  virtual Vector3& get_direction() = 0;

  /**
   * @brief In 2d mode (needCube being false), sets the direction used to cast the shadow. Also use
   * as the light direction on spot and directional lights.
   */
  virtual void set_direction(const Vector3& value) = 0;

}; // end of class IShadowLight

} // end of namespace BABYLON

#endif // end of BABYLON_ISHADOW_LIGHT_H
