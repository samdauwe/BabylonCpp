#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/sky/sky_material_defines.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT SkyMaterial : public PushMaterial {

public:
  using SMD = SkyMaterialDefines;

public:
  SkyMaterial(const std::string& name, Scene* scene);
  ~SkyMaterial();

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static SkyMaterial* Parse(const Json::value& source, Scene* scene,
                            const std::string& rootUrl);

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
