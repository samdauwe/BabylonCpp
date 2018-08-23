#ifndef BABYLON_MATERIALS_PUSH_MATERIAL_H
#define BABYLON_MATERIALS_PUSH_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PushMaterial : public Material {

public:
  template <typename... Ts>
  static PushMaterialPtr New(Ts&&... args)
  {
    auto material = shared_ptr_t<PushMaterial>(
      new PushMaterial(::std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~PushMaterial() override;

  /**
   * @brief Returns the string "PushMaterial".
   */
  const string_t getClassName() const override;

  IReflect::Type type() const override;

  Effect* getEffect();
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

  void bind(Matrix* world, Mesh* mesh) override;

protected:
  PushMaterial(const string_t& name, Scene* scene);

  void _afterBind(Mesh* mesh, Effect* effect = nullptr);
  bool _mustRebind(Scene* scene, Effect* effect, float visibility = 1.f);

protected:
  Effect* _activeEffect;
  Matrix _normalMatrix;

}; // end of class PushMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PUSH_MATERIAL_H
