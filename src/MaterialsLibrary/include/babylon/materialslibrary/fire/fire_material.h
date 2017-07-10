#ifndef BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/fire/fire_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT FireMaterial : public PushMaterial {

public:
  using FMD = FireMaterialDefines;

public:
  static unsigned int maxSimultaneousLights;

public:
  FireMaterial(const std::string& name, Scene* scene);
  virtual ~FireMaterial();

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static FireMaterial* Parse(const Json::value& source, Scene* scene,
                             const std::string& rootUrl);

public:
  Color3 diffuseColor;
  float speed;

private:
  BaseTexture* _diffuseTexture;
  BaseTexture* _distortionTexture;
  BaseTexture* _opacityTexture;
  Color3 _scaledDiffuse;
  int _renderId;
  FireMaterialDefines _defines;
  microseconds_t _lastTime;

}; // end of class FireMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
