#ifndef BABYLON_LIGHTS_POINT_LIGHT_H
#define BABYLON_LIGHTS_POINT_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PointLight : public Light, public IShadowLight {

public:
  template <typename... Ts>
  static PointLight* New(Ts&&... args)
  {
    auto light = new PointLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~PointLight();

  IReflect::Type type() const override;
  Scene* getScene() override;
  Vector3 getAbsolutePosition() override;
  bool computeTransformedPosition() override;
  void transferToEffect(Effect* effect,
                        const std::string& positionUniformName) override;
  bool needCube() const override;
  bool supportsVSM() const override;
  bool needRefreshPerFrame() const override;
  Vector3 getShadowDirection(unsigned int faceIndex = 0) override;
  ShadowGenerator* getShadowGenerator() override;
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;
  Matrix* _getWorldMatrix() override;
  unsigned int getTypeID() const override;

protected:
  PointLight(const std::string& name, const Vector3& position, Scene* scene);

public:
  std::unique_ptr<Vector3> transformedPosition;
  Vector3 position;

private:
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class PointLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_POINT_LIGHT_H
