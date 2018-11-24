#ifndef BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/fire/fire_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT FireMaterial : public PushMaterial {

public:
  static unsigned int maxSimultaneousLights;

public:
  FireMaterial(const std::string& name, Scene* scene);
  ~FireMaterial() override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables();
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  const std::string getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static FireMaterial* Parse(const json& source, Scene* scene,
                             const std::string& rootUrl);

protected:
  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_distortionTexture();
  void set_distortionTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_opacityTexture();
  void set_opacityTexture(const BaseTexturePtr& value);

public:
  Property<FireMaterial, BaseTexturePtr> diffuseTexture;
  Property<FireMaterial, BaseTexturePtr> distortionTexture;
  Property<FireMaterial, BaseTexturePtr> opacityTexture;
  Color3 diffuseColor;
  float speed;

private:
  BaseTexturePtr _diffuseTexture;
  BaseTexturePtr _distortionTexture;
  BaseTexturePtr _opacityTexture;
  Color3 _scaledDiffuse;
  int _renderId;
  FireMaterialDefines _defines;
  float _lastTime;

}; // end of class FireMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
