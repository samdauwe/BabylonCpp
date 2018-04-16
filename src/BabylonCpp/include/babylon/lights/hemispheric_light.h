#ifndef BABYLON_HEMISPHERIC_LIGHT_H
#define BABYLON_HEMISPHERIC_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/light.h>

namespace BABYLON {

/**
 * @brief The HemisphericLight simulates the ambient environment light,
 * so the passed direction is the light reflection direction, not the incoming
 * direction.
 */
class BABYLON_SHARED_EXPORT HemisphericLight : public Light {

public:
  template <typename... Ts>
  static HemisphericLight* New(Ts&&... args)
  {
    auto light = new HemisphericLight(::std::forward<Ts>(args)...);
    light->addToScene(static_cast<unique_ptr_t<Light>>(light));

    return light;
  }
  ~HemisphericLight() override;

  /**
   * @brief Returns the string "HemisphericLight".
   * @return The class name
   */
  const string_t getClassName() const override;

  IReflect::Type type() const override;

  /**
   * @brief Sets the HemisphericLight direction towards the passed target
   * (Vector3). Returns the updated direction.
   * @param target The target the direction should point to
   * @return The computed direction
   */
  Vector3 setDirectionToTarget(const Vector3& target);

  /**
   * @brief Returns the shadow generator associated to the light.
   * @returns Always null for hemispheric lights because it does not support
   * shadows.
   */
  ShadowGenerator* getShadowGenerator() override;

  /**
   * @brief Sets the passed Effect object with the HemisphericLight normalized
   * direction and color and the passed name (string).
   * @param effect The effect to update
   * @param lightIndex The index of the light in the effect to update
   * @returns The hemispheric light
   */
  void transferToEffect(Effect* effect, const string_t& lightIndex) override;

  /**
   * @brief Internal use only.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Returns the integer 3.
   * @return The light Type id as a constant defines in Light.LIGHTTYPEID_x
   */
  unsigned int getTypeID() const override;

protected:
  /**
   * @brief Creates a HemisphericLight object in the scene according to the
   * passed direction (Vector3). The HemisphericLight simulates the ambient
   * environment light, so the passed direction is the light reflection
   * direction, not the incoming direction. The HemisphericLight can't cast
   * shadows. Documentation : http://doc.babylonjs.com/tutorials/lights
   * @param name The friendly name of the light
   * @param direction The direction of the light reflection
   * @param scene The scene the light belongs to
   */
  HemisphericLight(const string_t& name, Scene* scene);
  HemisphericLight(const string_t& name, const Vector3& direction,
                   Scene* scene);

  void _buildUniformLayout() override;

public:
  /**
   * The groundColor is the light in the opposite direction to the one specified
   * during creation. You can think of the diffuse and specular light as coming
   * from the centre of the object in the given direction and the groundColor
   * light in the opposite direction.
   */
  Color3 groundColor;

  /**
   * The light reflection direction, not the incoming direction.
   */
  Vector3 direction;

private:
  unique_ptr_t<Matrix> _worldMatrix;

}; // end of class HemisphericLight

} // end of namespace BABYLON

#endif // end of BABYLON_HEMISPHERIC_LIGHT_H
