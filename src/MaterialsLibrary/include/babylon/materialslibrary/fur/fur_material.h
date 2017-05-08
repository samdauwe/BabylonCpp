#ifndef BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/fur/fur_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT FurMaterial : public PushMaterial {

public:
  using FMD = FurMaterialDefines;

public:
  FurMaterial(const std::string& name, Scene* scene);
  ~FurMaterial();

  float furTime() const;
  void setFurTime(float newFurTime);
  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  void updateFur();
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                         bool useInstances) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static FurMaterial* Parse(const Json::value& source, Scene* scene,
                            const std::string& rootUrl);
  static DynamicTexture* GenerateTexture(const std::string& name, Scene* scene);

  /**
   * Creates and returns an array of meshes used as shells for the Fur Material
   * that can be disposed later in your code
   * The quality is in interval [0, 100]
   */
  static std::vector<Mesh*> FurifyMesh(Mesh* sourceMesh, unsigned int quality);

public:
  Color3 diffuseColor;
  float furLength;
  float furAngle;
  Color3 furColor;
  float furOffset;
  float furSpacing;
  Vector3 furGravity;
  float furSpeed;
  float furDensity;
  DynamicTexture* furTexture;
  bool highLevelFur;
  std::vector<AbstractMesh*> _meshes;

private:
  BaseTexture* _diffuseTexture;
  BaseTexture* _heightTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  int _renderId;
  float _furTime;
  FurMaterialDefines _defines;

}; // end of class FurMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_H
