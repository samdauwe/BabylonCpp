#ifndef BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/terrain/terrain_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT TerrainMaterial : public Material {

public:
  using TMD = TerrainMaterialDefines;

public:
  TerrainMaterial(const std::string& name, Scene* scene);
  ~TerrainMaterial();

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
  static TerrainMaterial* Parse(const Json::value& source, Scene* scene,
                                const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* mixTexture;
  Texture* diffuseTexture1;
  Texture* diffuseTexture2;
  Texture* diffuseTexture3;
  Texture* bumpTexture1;
  Texture* bumpTexture2;
  Texture* bumpTexture3;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  bool disableLighting;
  unsigned int maxSimultaneousLights;

private:
  Matrix _worldViewProjectionMatrix;
  int _renderId;
  TerrainMaterialDefines _defines;
  std::unique_ptr<TerrainMaterialDefines> _cachedDefines;

}; // end of class TerrainMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
