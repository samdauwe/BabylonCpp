#ifndef BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/normal/normal_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class NormalMaterial;
using NormalMaterialPtr = std::shared_ptr<NormalMaterial>;

class BABYLON_SHARED_EXPORT NormalMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static NormalMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<NormalMaterial>(new NormalMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~NormalMaterial() override; // = default

  [[nodiscard]] Type type() const override;

  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaBlendingForMesh(const AbstractMesh& mesh) const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  [[nodiscard]] std::vector<BaseTexturePtr> getActiveTextures() const override;
  [[nodiscard]] bool hasTexture(const BaseTexturePtr& texture) const override;
  [[nodiscard]] std::string getClassName() const override;
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;
  [[nodiscard]] json serialize() const;

  /** Statics **/
  static NormalMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  NormalMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<NormalMaterial, BaseTexturePtr> diffuseTexture;
  Color3 diffuseColor;
  Property<NormalMaterial, bool> disableLighting;
  Property<NormalMaterial, unsigned int> maxSimultaneousLights;

private:
  BaseTexturePtr _diffuseTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;

}; // end of class NormalMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
