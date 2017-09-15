#ifndef BABYLON_HEMISPHERIC_LIGHT_H
#define BABYLON_HEMISPHERIC_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT HemisphericLight : public Light {

public:
  template <typename... Ts>
  static HemisphericLight* New(Ts&&... args)
  {
    auto light = new HemisphericLight(::std::forward<Ts>(args)...);
    light->addToScene(static_cast<unique_ptr_t<Light>>(light));

    return light;
  }
  ~HemisphericLight();

  /**
   * @brief Returns the string "HemisphericLight".
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;

  /**
   * @brief Sets the HemisphericLight direction towards the passed target
   * (Vector3).
   * @returns The updated direction.
   */
  Vector3 setDirectionToTarget(const Vector3& target);

  ShadowGenerator* getShadowGenerator() override;

  /**
   * @brief Sets the passed Effect object with the HemisphericLight normalized
   * direction and color and the passed name (string).
   */
  void transferToEffect(Effect* effect, const string_t& lightIndex) override;

  /**
   * @brief Returns the light world matrix.
   */
  Matrix* _getWorldMatrix() override;

  /**
   * @brief Returns the integer 3.
   */
  unsigned int getTypeID() const override;

protected:
  /**
   * @brief Creates a HemisphericLight object in the scene according to the
   * passed direction (Vector3).
   * The HemisphericLight simulates the ambient environment light, so the passed
   * direction is the light reflection direction, not the incoming direction.
   * The HemisphericLight can't cast shadows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  HemisphericLight(const string_t& name, Scene* scene);
  HemisphericLight(const string_t& name, const Vector3& direction,
                   Scene* scene);

  void _buildUniformLayout() override;

public:
  Color3 groundColor;
  Vector3 direction;

private:
  unique_ptr_t<Matrix> _worldMatrix;

}; // end of class HemisphericLight

} // end of namespace BABYLON

#endif // end of BABYLON_HEMISPHERIC_LIGHT_H
