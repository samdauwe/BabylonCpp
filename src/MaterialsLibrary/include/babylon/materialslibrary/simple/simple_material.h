#ifndef BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/simple/simple_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT SimpleMaterial : public PushMaterial {

public:
  SimpleMaterial(const std::string& name, Scene* scene);
  ~SimpleMaterial() override;

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
  static SimpleMaterial* Parse(const Json::value& source, Scene* scene,
                               const std::string& rootUrl);

protected:
  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<SimpleMaterial, BaseTexturePtr> diffuseTexture;
  Color3 diffuseColor;
  Property<SimpleMaterial, bool> disableLighting;
  Property<SimpleMaterial, unsigned int> maxSimultaneousLights;

private:
  BaseTexturePtr _diffuseTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  int _renderId;

}; // end of class SimpleMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_H
