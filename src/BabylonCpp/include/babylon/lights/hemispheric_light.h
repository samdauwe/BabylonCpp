#ifndef BABYLON_HEMISPHERIC_LIGHT_H
#define BABYLON_HEMISPHERIC_LIGHT_H

#include <babylon/babylon_api.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class HemisphericLight;
using HemisphericLightPtr = std::shared_ptr<HemisphericLight>;

/**
 * @brief The HemisphericLight simulates the ambient environment light,
 * so the passed direction is the light reflection direction, not the incoming direction.
 */
class BABYLON_SHARED_EXPORT HemisphericLight : public Light {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static HemisphericLightPtr New(Ts&&... args)
  {
    auto light = std::shared_ptr<HemisphericLight>(new HemisphericLight(std::forward<Ts>(args)...));
    light->addToScene(light);

    return light;
  }
  ~HemisphericLight() override; // = default

  /**
   * @brief Returns the string "HemisphericLight".
   * @return The class name
   */
  std::string getClassName() const override;

  Type type() const override;

  /**
   * @brief Sets the HemisphericLight direction towards the passed target (Vector3).
   * Returns the updated direction.
   * @param target The target the direction should point to
   * @return The computed direction
   */
  Vector3 setDirectionToTarget(const Vector3& target);

  /**
   * @brief Returns the shadow generator associated to the light.
   * @returns Always null for hemispheric lights because it does not support shadows.
   */
  IShadowGeneratorPtr getShadowGenerator() override;

  /**
   * @brief Sets the passed Effect object with the HemisphericLight normalized direction and color
   * and the passed name (string).
   * @param effect The effect to update
   * @param lightIndex The index of the light in the effect to update
   * @returns The hemispheric light
   */
  void transferToEffect(const EffectPtr& effect, const std::string& lightIndex) override;

  /**
   * @brief Sets the passed Effect "effect" with the Light information.
   * @param effect The effect to update
   * @param lightDataUniformName The uniform used to store light data (position or direction)
   * @returns The light
   */
  HemisphericLight& transferToNodeMaterialEffect(const EffectPtr& effect,
                                                 const std::string& lightDataUniformName) override;

  /**
   * @brief Computes the world matrix of the node
   * @param force defines if the cache version should be invalidated forcing the world matrix to be
   * created from scratch
   * @param useWasUpdatedFlag defines a reserved property
   * @returns the world matrix
   */
  Matrix& computeWorldMatrix(bool force = false, bool useWasUpdatedFlag = false) override;

  /**
   * @brief Returns the integer 3.
   * @return The light Type id as a constant defines in Light.LIGHTTYPEID_x
   */
  unsigned int getTypeID() const override;

  /**
   * @brief Prepares the list of defines specific to the light type.
   * @param defines the list of defines
   * @param lightIndex defines the index of the light for the effect
   */
  void prepareLightSpecificDefines(MaterialDefines& defines, unsigned int lightIndex) override;

protected:
  /**
   * @brief Creates a HemisphericLight object in the scene according to the passed direction
   * (Vector3). The HemisphericLight simulates the ambient environment light, so the passed
   * direction is the light reflection direction, not the incoming direction. The HemisphericLight
   * can't cast shadows. Documentation : https://doc.babylonjs.com/babylon101/lights
   * @param name The friendly name of the light
   * @param direction The direction of the light reflection
   * @param scene The scene the light belongs to
   */
  HemisphericLight(const std::string& name, Scene* scene);
  HemisphericLight(const std::string& name, const Vector3& direction, Scene* scene);

  void _buildUniformLayout() override;

public:
  /**
   * The groundColor is the light in the opposite direction to the one specified during creation.
   * You can think of the diffuse and specular light as coming from the centre of the object in the
   * given direction and the groundColor light in the opposite direction.
   */
  Color3 groundColor;

  /**
   * The light reflection direction, not the incoming direction.
   */
  Vector3 direction;

private:
  static bool NodeConstructorAdded;

}; // end of class HemisphericLight

} // end of namespace BABYLON

#endif // end of BABYLON_HEMISPHERIC_LIGHT_H
