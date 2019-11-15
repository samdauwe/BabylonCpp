#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/sky/sky_material_defines.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace MaterialsLibrary {

class SkyMaterial;
using SkyMaterialPtr = std::shared_ptr<SkyMaterial>;

class BABYLON_SHARED_EXPORT SkyMaterial : public PushMaterial {

public:
  template <typename... Ts>
  static SkyMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<SkyMaterial>(
      new SkyMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~SkyMaterial(); // = default

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables() override;
  const std::string getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false,
                       bool notBoundToMesh       = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  json serialize() const;

  /** Statics **/
  static SkyMaterial* Parse(const json& source, Scene* scene,
                            const std::string& rootUrl);

protected:
  SkyMaterial(const std::string& name, Scene* scene);

public:
  // Public members
  float luminance;
  float turbidity;
  float rayleigh;
  float mieCoefficient;
  float mieDirectionalG;
  float distance;
  float inclination;
  float azimuth;
  Vector3 sunPosition;
  bool useSunPosition;

private:
  Vector3 _cameraPosition;
  int _renderId;

}; // end of class SkyMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H
