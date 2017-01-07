#ifndef BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/normal/normal_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT NormalMaterial : public Material {

public:
  using NMD = NormalMaterialDefines;

public:
  NormalMaterial(const std::string& name, Scene* scene);
  ~NormalMaterial();

  IReflect::Type type() const override;
  void setDiffuseColor(const Color3& color) override;

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
  static NormalMaterial* Parse(const Json::value& source, Scene* scene,
                               const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* diffuseTexture;
  Color3 diffuseColor;
  bool disableLighting;
  unsigned int maxSimultaneousLights;

private:
  Matrix _worldViewProjectionMatrix;
  Color3 _scaledDiffuse;
  int _renderId;
  NormalMaterialDefines _defines;
  std::unique_ptr<NormalMaterialDefines> _cachedDefines;

}; // end of class NormalMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
