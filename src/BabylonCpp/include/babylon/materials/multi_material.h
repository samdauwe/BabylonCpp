#ifndef BABYLON_MATERIALS_MULTI_MATERIAL_H
#define BABYLON_MATERIALS_MULTI_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class MultiMaterial;
using MultiMaterialPtr = std::shared_ptr<MultiMaterial>;

class BABYLON_SHARED_EXPORT MultiMaterial : public Material {

public:
  template <typename... Ts>
  static MultiMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<MultiMaterial>(
      new MultiMaterial(std::forward<Ts>(args)...));
    material->addMultiMaterialToScene(material);

    return material;
  }
  virtual ~MultiMaterial() override;

  /**
   * @brief Returns the string "MultiMaterial".
   */
  const std::string getClassName() const override;

  IReflect::Type type() const override;

  /** Properties **/
  std::vector<MaterialPtr>& subMaterials();
  void setSubMaterials(const std::vector<MaterialPtr>& value);
  MaterialPtr& getSubMaterial(unsigned int index);
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /** Methods **/
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  MaterialPtr clone(const std::string& _name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;
  void dispose(bool forceDisposeEffect   = false,
               bool forceDisposeTextures = false) override;

protected:
  MultiMaterial(const std::string name, Scene* scene);

private:
  void _hookArray(const std::vector<MaterialPtr>& array);

private:
  std::vector<MaterialPtr> _subMaterials;

}; // end of class MultiMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MULTI_MATERIAL_H
