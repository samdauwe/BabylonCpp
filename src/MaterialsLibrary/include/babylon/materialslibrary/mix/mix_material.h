#ifndef BABYLON_MATERIALS_LIBRARY_MIX_MIX_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_MIX_MIX_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/lava/lava_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class IAnimatable;
class Texture;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;
using TexturePtr     = std::shared_ptr<Texture>;

namespace MaterialsLibrary {

class MixMaterial;
using MixMaterialPtr = std::shared_ptr<MixMaterial>;

class BABYLON_SHARED_EXPORT MixMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static MixMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<MixMaterial>(
      new MixMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~MixMaterial() override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables();
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  const std::string getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false,
                       bool notBoundToMesh       = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static MixMaterial* Parse(const json& source, Scene* scene,
                            const std::string& rootUrl);

protected:
  MixMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_mixTexture1();
  void set_mixTexture1(const BaseTexturePtr& value);
  BaseTexturePtr& get_mixTexture2();
  void set_mixTexture2(const BaseTexturePtr& value);
  TexturePtr& get_diffuseTexture1();
  void set_diffuseTexture1(const TexturePtr& value);
  TexturePtr& get_diffuseTexture2();
  void set_diffuseTexture2(const TexturePtr& value);
  TexturePtr& get_diffuseTexture3();
  void set_diffuseTexture3(const TexturePtr& value);
  TexturePtr& get_diffuseTexture4();
  void set_diffuseTexture4(const TexturePtr& value);
  TexturePtr& get_diffuseTexture5();
  void set_diffuseTexture5(const TexturePtr& value);
  TexturePtr& get_diffuseTexture6();
  void set_diffuseTexture6(const TexturePtr& value);
  TexturePtr& get_diffuseTexture7();
  void set_diffuseTexture7(const TexturePtr& value);
  TexturePtr& get_diffuseTexture8();
  void set_diffuseTexture8(const TexturePtr& value);
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  /**
   * Mix textures
   */
  Property<MixMaterial, BaseTexturePtr> mixTexture1;
  Property<MixMaterial, BaseTexturePtr> mixTexture2;

  /**
   * Diffuse textures
   */
  Property<MixMaterial, TexturePtr> diffuseTexture1;
  Property<MixMaterial, TexturePtr> diffuseTexture2;
  Property<MixMaterial, TexturePtr> diffuseTexture3;
  Property<MixMaterial, TexturePtr> diffuseTexture4;
  Property<MixMaterial, TexturePtr> diffuseTexture5;
  Property<MixMaterial, TexturePtr> diffuseTexture6;
  Property<MixMaterial, TexturePtr> diffuseTexture7;
  Property<MixMaterial, TexturePtr> diffuseTexture8;

  /**
   * Uniforms
   */
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;

  Property<MixMaterial, bool> disableLighting;
  Property<MixMaterial, unsigned int> maxSimultaneousLights;

private:
  /**
   * Mix textures
   */
  BaseTexturePtr _mixTexture1;
  BaseTexturePtr _mixTexture2;

  /**
   * Diffuse textures
   */
  TexturePtr _diffuseTexture1;
  TexturePtr _diffuseTexture2;
  TexturePtr _diffuseTexture3;
  TexturePtr _diffuseTexture4;
  TexturePtr _diffuseTexture5;
  TexturePtr _diffuseTexture6;
  TexturePtr _diffuseTexture7;
  TexturePtr _diffuseTexture8;

  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  int _renderId;

}; // end of class MixMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_MIX_MIX_MATERIAL_H
