#ifndef BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/terrain/terrain_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class AbstractMesh;
class IAnimatable;
class Mesh;
class Texture;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using IAnimatablePtr  = std::shared_ptr<IAnimatable>;
using MeshPtr         = std::shared_ptr<Mesh>;
using TexturePtr      = std::shared_ptr<Texture>;

namespace MaterialsLibrary {

class TerrainMaterial;
using TerrainMaterialPtr = std::shared_ptr<TerrainMaterial>;

class BABYLON_SHARED_EXPORT TerrainMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static TerrainMaterialPtr New(Ts&&... args)
  {
    auto material
      = std::shared_ptr<TerrainMaterial>(new TerrainMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~TerrainMaterial() override; // = default

  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
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
  static TerrainMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  TerrainMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_mixTexture();
  void set_mixTexture(const BaseTexturePtr& value);
  TexturePtr& get_diffuseTexture1();
  void set_diffuseTexture1(const TexturePtr& value);
  TexturePtr& get_diffuseTexture2();
  void set_diffuseTexture2(const TexturePtr& value);
  TexturePtr& get_diffuseTexture3();
  void set_diffuseTexture3(const TexturePtr& value);
  TexturePtr& get_bumpTexture1();
  void set_bumpTexture1(const TexturePtr& value);
  TexturePtr& get_bumpTexture2();
  void set_bumpTexture2(const TexturePtr& value);
  TexturePtr& get_bumpTexture3();
  void set_bumpTexture3(const TexturePtr& value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<TerrainMaterial, BaseTexturePtr> mixTexture;
  Property<TerrainMaterial, TexturePtr> diffuseTexture1;
  Property<TerrainMaterial, TexturePtr> diffuseTexture2;
  Property<TerrainMaterial, TexturePtr> diffuseTexture3;
  Property<TerrainMaterial, TexturePtr> bumpTexture1;
  Property<TerrainMaterial, TexturePtr> bumpTexture2;
  Property<TerrainMaterial, TexturePtr> bumpTexture3;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  Property<TerrainMaterial, bool> disableLighting;
  Property<TerrainMaterial, unsigned int> maxSimultaneousLights;

private:
  BaseTexturePtr _mixTexture;
  TexturePtr _diffuseTexture1;
  TexturePtr _diffuseTexture2;
  TexturePtr _diffuseTexture3;
  TexturePtr _bumpTexture1;
  TexturePtr _bumpTexture2;
  TexturePtr _bumpTexture3;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  int _renderId;

}; // end of class TerrainMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
