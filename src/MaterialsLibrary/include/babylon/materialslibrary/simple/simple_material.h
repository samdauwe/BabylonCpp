#ifndef BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/simple/simple_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class IAnimatable;
class Texture;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;
using TexturePtr     = std::shared_ptr<Texture>;

namespace MaterialsLibrary {

class SimpleMaterial;
using SimpleMaterialPtr = std::shared_ptr<SimpleMaterial>;

class BABYLON_SHARED_EXPORT SimpleMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static SimpleMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<SimpleMaterial>(new SimpleMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~SimpleMaterial() override; // = default

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
  static SimpleMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  SimpleMaterial(const std::string& name, Scene* scene);

  TexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const TexturePtr& value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<SimpleMaterial, TexturePtr> diffuseTexture;
  Color3 diffuseColor;
  Property<SimpleMaterial, bool> disableLighting;
  Property<SimpleMaterial, unsigned int> maxSimultaneousLights;

private:
  TexturePtr _diffuseTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  int _renderId;

}; // end of class SimpleMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H
