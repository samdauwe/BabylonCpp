#ifndef BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/lava/lava_material_defines.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

FWD_CLASS_SPTR(IAnimatable)

namespace MaterialsLibrary {

FWD_CLASS_SPTR(LavaMaterial)

class BABYLON_SHARED_EXPORT LavaMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static LavaMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<LavaMaterial>(new LavaMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~LavaMaterial() override; // = default

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  std::string getClassName() const override;
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;
  MaterialPtr clone(const std::string& name, bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static LavaMaterial* Parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Constructor
   * @param name The name given to the material in order to identify it afterwards.
   * @param scene The scene the material is used in.
   */
  LavaMaterial(const std::string& name, Scene* scene);

  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  bool get_disableLighting() const;
  void set_disableLighting(bool value);
  bool get_unlit() const;
  void set_unlit(bool value);
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<LavaMaterial, BaseTexturePtr> diffuseTexture;
  BaseTexturePtr noiseTexture;
  std::optional<Color3> fogColor;
  float speed;
  float movingSpeed;
  float lowFrequencySpeed;
  float fogDensity;
  Color3 diffuseColor;
  Property<LavaMaterial, bool> disableLighting;
  Property<LavaMaterial, bool> unlit;
  Property<LavaMaterial, unsigned int> maxSimultaneousLights;

private:
  BaseTexturePtr _diffuseTexture;
  float _lastTime;
  bool _disableLighting;
  bool _unlit;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  Color3 _scaledDiffuse;

}; // end of class LavaMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H
