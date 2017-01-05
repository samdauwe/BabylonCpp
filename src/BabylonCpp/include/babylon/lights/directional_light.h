#ifndef BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
#define BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DirectionalLight : public Light,
                                               public IShadowLight {

public:
  template <typename... Ts>
  static DirectionalLight* New(Ts&&... args)
  {
    auto light = new DirectionalLight(std::forward<Ts>(args)...);
    light->addToScene(static_cast<std::unique_ptr<Light>>(light));

    return light;
  }
  ~DirectionalLight();

  IReflect::Type type() const override;
  Scene* getScene() override;
  Vector3 getAbsolutePosition() override;
  Vector3& setDirectionToTarget(Vector3& target);
  void setShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList) override;
  bool supportsVSM() const override;
  bool needRefreshPerFrame() const override;
  bool needCube() const override;
  Vector3 getShadowDirection(unsigned int faceIndex) override;
  bool computeTransformedPosition() override;
  void transferToEffect(Effect* effect,
                        const std::string& directionUniformName) override;
  Matrix* _getWorldMatrix() override;
  unsigned int getTypeID() const override;

protected:
  DirectionalLight(const std::string& name, const Vector3& direction,
                   Scene* scene);

public:
  Vector3 position;
  Vector3 direction;
  std::unique_ptr<Vector3> transformedPosition;
  float shadowOrthoScale;
  bool autoUpdateExtends;

private:
  std::unique_ptr<Vector3> _transformedDirection;
  std::unique_ptr<Matrix> _worldMatrix;
  // Cache
  float _orthoLeft;
  float _orthoRight;
  float _orthoTop;
  float _orthoBottom;

}; // end of class DirectionalLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
