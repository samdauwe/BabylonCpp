#ifndef BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/terrain/terrain_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT TerrainMaterial : public PushMaterial {

public:
  using TMD = TerrainMaterialDefines;

public:
  TerrainMaterial(const std::string& name, Scene* scene);
  ~TerrainMaterial();

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  std::vector<BaseTexture*> getActiveTextures() const override;
  bool hasTexture(BaseTexture* texture) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static TerrainMaterial* Parse(const Json::value& source, Scene* scene,
                                const std::string& rootUrl);

public:
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;

private:
  BaseTexture* _mixTexture;
  Texture* _diffuseTexture1;
  Texture* _diffuseTexture2;
  Texture* _diffuseTexture3;
  Texture* _bumpTexture1;
  Texture* _bumpTexture2;
  Texture* _bumpTexture3;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  int _renderId;

}; // end of class TerrainMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_H
