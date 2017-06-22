#ifndef BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
#define BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadow_light.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DirectionalLight : public ShadowLight {

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

  /**
   * @brief Returns the string "DirectionalLight".
   */
  const char* getClassName() const override;

  /**
   * @brief Returns the integer 1.
   */
  unsigned int getTypeID() const override;

  /**
   * @brief Fix frustum size for the shadow generation. This is disabled if the
   * value is 0.
   */
  float shadowFrustumSize() const;

  /**
   * @brief Specifies a fix frustum size for the shadow generation.
   */
  void setShadowFrustumSize(float value);

  float shadowOrthoScale() const;

  void setShadowOrthoScale(float value);

  /**
   * @brief Sets the passed Effect object with the DirectionalLight transformed
   * position (or position if not parented) and the passed name.
   */
  void transferToEffect(Effect* effect, const std::string& lightIndex) override;

protected:
  /**
   * @brief Creates a DirectionalLight object in the scene, oriented towards the
   * passed direction (Vector3).
   * The directional light is emitted from everywhere in the given direction.
   * It can cast shawdows.
   * Documentation : http://doc.babylonjs.com/tutorials/lights
   */
  DirectionalLight(const std::string& name, const Vector3& direction,
                   Scene* scene);

  /**
   * @brief Sets the passed matrix "matrix" as projection matrix for the shadows
   * cast by the light according to the passed view matrix.
   * @returns The DirectionalLight Shadow projection matrix.
   */
  void _setDefaultShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList);

  /**
   * @brief Sets the passed matrix "matrix" as fixed frustum projection matrix
   * for the shadows cast by the light according to the passed view matrix.
   * @returns The DirectionalLight Shadow projection matrix.
   */
  void _setDefaultFixedFrustumShadowProjectionMatrix(Matrix& matrix,
                                                     const Matrix& viewMatrix);

  /**
   * @brief Sets the passed matrix "matrix" as auto extend projection matrix for
   * the shadows cast by the light according to the passed view matrix.
   * @returns The DirectionalLight Shadow projection matrix.
   */
  void _setDefaultAutoExtendShadowProjectionMatrix(
    Matrix& matrix, const Matrix& viewMatrix,
    const std::vector<AbstractMesh*>& renderList);

  void _buildUniformLayout() override;

public:
  bool autoUpdateExtends;

private:
  float _shadowFrustumSize;
  float _shadowOrthoScale;
  // Cache
  float _orthoLeft;
  float _orthoRight;
  float _orthoTop;
  float _orthoBottom;

}; // end of class DirectionalLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_DIRECTIONAL_LIGHT_H
