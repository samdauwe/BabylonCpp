#ifndef BABYLON_MATERIALS_PUSH_MATERIAL_H
#define BABYLON_MATERIALS_PUSH_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PushMaterial : public Material {

public:
  template <typename... Ts>
  static PushMaterial* New(Ts&&... args)
  {
    auto material = new PushMaterial(std::forward<Ts>(args)...);
    material->addMaterialToScene(
      static_cast<std::unique_ptr<Material>>(material));

    return material;
  }
  ~PushMaterial();

  /**
   * @brief Returns the string "PushMaterial".
   */
  const char* getClassName() const;

  IReflect::Type type() const override;

  Effect* getEffect();
  bool isReady(AbstractMesh* mesh, bool useInstances) override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh) override;

protected:
  PushMaterial(const std::string& name, Scene* scene);

  void _afterBind(Mesh* mesh, Effect* effect);
  bool _mustRebind(Scene* scene, Effect* effect, float visibility = 1.f);

protected:
  Effect* _activeEffect;

}; // end of class PushMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PUSH_MATERIAL_H
