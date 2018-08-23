#ifndef BABYLON_MATERIALS_MULTI_MATERIAL_H
#define BABYLON_MATERIALS_MULTI_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MultiMaterial : public Material {

public:
  template <typename... Ts>
  static MultiMaterialPtr New(Ts&&... args)
  {
    auto material = shared_ptr_t<MultiMaterial>(
      new MultiMaterial(::std::forward<Ts>(args)...));
    material->addMultiMaterialToScene(material);

    return material;
  }
  virtual ~MultiMaterial() override;

  /**
   * @brief Returns the string "MultiMaterial".
   */
  const string_t getClassName() const override;

  IReflect::Type type() const override;

  /** Properties **/
  vector_t<MaterialPtr>& subMaterials();
  void setSubMaterials(const vector_t<MaterialPtr>& value);
  MaterialPtr& getSubMaterial(unsigned int index);
  vector_t<BaseTexturePtr> getActiveTextures() const override;

  /** Methods **/
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  MaterialPtr clone(const string_t& _name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;
  void dispose(bool forceDisposeEffect   = false,
               bool forceDisposeTextures = false) override;

protected:
  MultiMaterial(const string_t name, Scene* scene);

private:
  void _hookArray(const vector_t<MaterialPtr>& array);

private:
  vector_t<MaterialPtr> _subMaterials;

}; // end of class MultiMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MULTI_MATERIAL_H
