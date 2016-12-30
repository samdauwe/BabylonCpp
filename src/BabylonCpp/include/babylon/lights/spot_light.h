#ifndef BABYLON_LIGHTS_SPOT_LIGHT_H
#define BABYLON_LIGHTS_SPOT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SpotLight : public Light, public IShadowLight {

public:
  template <typename... Ts>
  static SpotLight* New(Ts&&... args)
  {
    auto light = new SpotLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~SpotLight();

  Node::Type type() const override;
  Scene* getScene() override;
  Vector3 getAbsolutePosition() override;
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;
  bool needCube() const override;
  bool supportsVSM() const override;
  bool needRefreshPerFrame() const override;
  Vector3 getShadowDirection(unsigned int faceIndex = 0) override;
  Vector3& setDirectionToTarget(Vector3& target);
  bool computeTransformedPosition() override;
  void transferToEffect(Effect* effect, const std::string& positionUniformName,
                        const std::string& directionUniformName) override;
  Matrix* _getWorldMatrix() override;
  unsigned int getTypeID() const override;
  Vector3 getRotation();

protected:
  SpotLight(const std::string& name, const Vector3& position,
            const Vector3& direction, float angle, float exponent,
            Scene* scene);

public:
  std::unique_ptr<Vector3> transformedPosition;
  Vector3 position;
  Vector3 direction;
  float angle;
  float exponent;

private:
  std::unique_ptr<Vector3> _transformedDirection;
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class PointLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SPOT_LIGHT_H
