#ifndef BABYLON_MATERIALS_MULTI_MATERIAL_H
#define BABYLON_MATERIALS_MULTI_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MultiMaterial : public Material {

public:
  template <typename... Ts>
  static MultiMaterial* New(Ts&&... args)
  {
    auto material = new MultiMaterial(std::forward<Ts>(args)...);
    material->addMultiMaterialToScene(
      static_cast<std::unique_ptr<MultiMaterial>>(material));

    return material;
  }
  ~MultiMaterial();

  Material::Type type() const override;

  /** Properties **/
  Material* getSubMaterial(unsigned int index);

  /** Methods **/
  bool isReady(AbstractMesh* mesh = nullptr,
               bool useInstances  = false) override;
  Material* clone(const std::string& _name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

protected:
  MultiMaterial(const std::string name, Scene* scene);

public:
  std::vector<Material*> subMaterials;

}; // end of class MultiMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MULTI_MATERIAL_H
