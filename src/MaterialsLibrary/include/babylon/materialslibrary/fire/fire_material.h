#ifndef BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/fire/fire_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT FireMaterial : public Material {

public:
  using FMD = FireMaterialDefines;

public:
  static unsigned int maxSimultaneousLights;

public:
  FireMaterial(const std::string& name, Scene* scene);
  ~FireMaterial();

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReady(AbstractMesh* mesh, bool useInstances) override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static FireMaterial* Parse(const Json::value& source, Scene* scene,
                             const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* diffuseTexture;
  BaseTexture* distortionTexture;
  BaseTexture* opacityTexture;
  Color3 diffuseColor;
  float speed;

private:
  Color3 _scaledDiffuse;
  int _renderId;
  FireMaterialDefines _defines;
  std::unique_ptr<FireMaterialDefines> _cachedDefines;
  microseconds_t _lastTime;

}; // end of class FireMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_H
