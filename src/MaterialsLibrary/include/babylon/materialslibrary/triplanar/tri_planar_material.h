#ifndef BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/triplanar/tri_planar_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT TriPlanarMaterial : public PushMaterial {

public:
  using TPMD = TriPlanarMaterialDefines;

public:
  TriPlanarMaterial(const std::string& name, Scene* scene);
  ~TriPlanarMaterial();

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static TriPlanarMaterial* Parse(const Json::value& source, Scene* scene,
                                  const std::string& rootUrl);

public:
  float tileSize;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;

private:
  BaseTexture* _mixTexture;
  Texture* _diffuseTextureX;
  Texture* _diffuseTextureY;
  Texture* _diffuseTextureZ;
  Texture* _normalTextureX;
  Texture* _normalTextureY;
  Texture* _normalTextureZ;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  int _renderId;

}; // end of class TriPlanarMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_H
