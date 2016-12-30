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
    auto light = new HemisphericLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~HemisphericLight();

  Node::Type type() const override;
  Vector3 setDirectionToTarget(const Vector3& target);
  ShadowGenerator* getShadowGenerator() override;
  void transferToEffect(Effect* effect, const std::string& directionUniformName,
                        const std::string& groundColorUniformName) override;
  Matrix* _getWorldMatrix() override;
  unsigned int getTypeID() const override;

protected:
  HemisphericLight(const std::string& name, const Vector3& direction,
                   Scene* scene);

public:
  Color3 groundColor;
  Vector3 direction;

private:
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class HemisphericLight

} // end of namespace BABYLON

#endif // end of BABYLON_HEMISPHERIC_LIGHT_H
