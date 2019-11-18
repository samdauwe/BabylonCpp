#ifndef ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H
#define ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/shadowonly/shadow_only_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class IShadowLight;
using IShadowLightPtr = std::shared_ptr<IShadowLight>;

namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT ShadowOnlyMaterial : public PushMaterial {

public:
  ShadowOnlyMaterial(const std::string& name, Scene* scene);
  ~ShadowOnlyMaterial() override; // = default

  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh, bool useInstances) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  [[nodiscard]] std::string getClassName() const override;
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;
  [[nodiscard]] json serialize() const;

  /** Statics **/
  static ShadowOnlyMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  IShadowLightPtr& get_activeLight();
  void set_activeLight(const IShadowLightPtr& light);

public:
  Color3 shadowColor;
  Property<ShadowOnlyMaterial, IShadowLightPtr> activeLight;

private:
  int _renderId;
  IShadowLightPtr _activeLight;

}; // end of class ShadowOnlyMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of ABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_H
