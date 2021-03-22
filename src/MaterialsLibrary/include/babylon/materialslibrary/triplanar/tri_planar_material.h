#ifndef BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/triplanar/tri_planar_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(IAnimatable)
FWD_CLASS_SPTR(Mesh)
FWD_CLASS_SPTR(Texture)

namespace MaterialsLibrary {

FWD_CLASS_SPTR(TriPlanarMaterial)

class BABYLON_SHARED_EXPORT TriPlanarMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static TriPlanarMaterialPtr New(Ts&&... args)
  {
    auto material
      = std::shared_ptr<TriPlanarMaterial>(new TriPlanarMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~TriPlanarMaterial() override; // = default

  [[nodiscard]] bool needAlphaBlending() const override;
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
  static TriPlanarMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  TriPlanarMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_mixTexture();
  void set_mixTexture(const BaseTexturePtr& value);
  TexturePtr& get_diffuseTextureX();
  void set_diffuseTextureX(const TexturePtr& value);
  TexturePtr& get_diffuseTextureY();
  void set_diffuseTextureY(const TexturePtr& value);
  TexturePtr& get_diffuseTextureZ();
  void set_diffuseTextureZ(const TexturePtr& value);
  TexturePtr& get_normalTextureX();
  void set_normalTextureX(const TexturePtr& value);
  TexturePtr& get_normalTextureY();
  void set_normalTextureY(const TexturePtr& value);
  TexturePtr& get_normalTextureZ();
  void set_normalTextureZ(const TexturePtr& value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<TriPlanarMaterial, BaseTexturePtr> mixTexture;
  Property<TriPlanarMaterial, TexturePtr> diffuseTextureX;
  Property<TriPlanarMaterial, TexturePtr> diffuseTextureY;
  Property<TriPlanarMaterial, TexturePtr> diffuseTextureZ;
  Property<TriPlanarMaterial, TexturePtr> normalTextureX;
  Property<TriPlanarMaterial, TexturePtr> normalTextureY;
  Property<TriPlanarMaterial, TexturePtr> normalTextureZ;
  float tileSize;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  Property<TriPlanarMaterial, bool> disableLighting;
  Property<TriPlanarMaterial, unsigned int> maxSimultaneousLights;

private:
  BaseTexturePtr _mixTexture;
  TexturePtr _diffuseTextureX;
  TexturePtr _diffuseTextureY;
  TexturePtr _diffuseTextureZ;
  TexturePtr _normalTextureX;
  TexturePtr _normalTextureY;
  TexturePtr _normalTextureZ;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;

}; // end of class TriPlanarMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
