#ifndef BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/triplanar/tri_planar_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT TriPlanarMaterial : public PushMaterial {

public:
  TriPlanarMaterial(const std::string& name, Scene* scene);
  ~TriPlanarMaterial() override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables();
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  const string_t getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static TriPlanarMaterial* Parse(const Json::value& source, Scene* scene,
                                  const std::string& rootUrl);

protected:
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
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  unsigned int get_maxSimultaneousLights() const;
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
  int _renderId;

}; // end of class TriPlanarMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
