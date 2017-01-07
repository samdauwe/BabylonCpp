#ifndef BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/triplanar/tri_planar_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT TriPlanarMaterial : public Material {

public:
  using TPMD = TriPlanarMaterialDefines;

public:
  TriPlanarMaterial(const std::string& name, Scene* scene);
  ~TriPlanarMaterial();

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
  static TriPlanarMaterial* Parse(const Json::value& source, Scene* scene,
                                  const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* mixTexture;
  Texture* diffuseTextureX;
  Texture* diffuseTextureY;
  Texture* diffuseTextureZ;
  Texture* normalTextureX;
  Texture* normalTextureY;
  Texture* normalTextureZ;
  float tileSize;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  bool disableLighting;
  unsigned int maxSimultaneousLights;

private:
  Matrix _worldViewProjectionMatrix;
  int _renderId;
  TriPlanarMaterialDefines _defines;
  std::unique_ptr<TriPlanarMaterialDefines> _cachedDefines;

}; // end of class TriPlanarMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
