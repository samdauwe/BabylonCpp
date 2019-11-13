#ifndef BABYLON_MATERIALS_PUSH_MATERIAL_H
#define BABYLON_MATERIALS_PUSH_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class Effect;
class PushMaterial;
using EffectPtr       = std::shared_ptr<Effect>;
using PushMaterialPtr = std::shared_ptr<PushMaterial>;

/**
 * @brief Base class of materials working in push mode in babylon JS.
 * Hidden
 */
class BABYLON_SHARED_EXPORT PushMaterial : public Material {

public:
  template <typename... Ts>
  static PushMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<PushMaterial>(
      new PushMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~PushMaterial(); // = default

  /**
   * @brief Returns the string "PushMaterial".
   */
  const std::string getClassName() const override;

  Type type() const override;

  EffectPtr& getEffect();
  bool isReady(AbstractMesh* mesh, bool useInstances) override;

  /**
   * @brief Binds the given world matrix to the active effect.
   * @param world the matrix to bind
   */
  void bindOnlyWorldMatrix(Matrix& world) override;

  /**
   * @brief Binds the given normal matrix to the active effect.
   * @param normalMatrix the matrix to bind
   */
  void bindOnlyNormalMatrix(Matrix& normalMatrix);

  void bind(Matrix& world, Mesh* mesh) override;

protected:
  PushMaterial(const std::string& name, Scene* scene);

  void _afterBind(Mesh* mesh, const EffectPtr& effect = nullptr);
  bool _mustRebind(Scene* scene, const EffectPtr& effect,
                   float visibility = 1.f);

public:
  /**
   * Gets or sets a boolean indicating that the material is allowed to do shader
   * hot swapping. This means that the material can keep using a previous shader
   * while a new one is being compiled. This is mostly used when shader parallel
   * compilation is supported (true by default)
   */
  bool allowShaderHotSwapping;

protected:
  EffectPtr _activeEffect;
  Matrix _normalMatrix;

}; // end of class PushMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PUSH_MATERIAL_H
