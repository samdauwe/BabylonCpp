#ifndef BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/fire/fire_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class FireMaterial;
using FireMaterialPtr = std::shared_ptr<FireMaterial>;

class BABYLON_SHARED_EXPORT FireMaterial : public PushMaterial {

public:
  static unsigned int maxSimultaneousLights;

public:
  template <typename... Ts>
  static FireMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<FireMaterial>(new FireMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~FireMaterial() override; // = default

  [[nodiscard]] bool needAlphaBlending() const override;
  [[nodiscard]] bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances) override;
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
  static FireMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * Constructor
   * @param name The name given to the material in order to identify it
   * afterwards.
   * @param scene The scene the material is used in.
   */
  FireMaterial(const std::string& name, Scene* scene);

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
  FireMaterialDefines _defines;
  float _lastTime;

}; // end of class FireMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
