#ifndef BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/lava/lava_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT LavaMaterial : public Material {

public:
  using LMD = LavaMaterialDefines;

public:
  LavaMaterial(const std::string& name, Scene* scene);
  ~LavaMaterial();

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
  static LavaMaterial* Parse(const Json::value& source, Scene* scene,
                             const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* diffuseTexture;
  BaseTexture* noiseTexture;
  std::unique_ptr<Color3> fogColor;
  float speed;
  float movingSpeed;
  float lowFrequencySpeed;
  float fogDensity;
  microseconds_t _lastTime;
  Color3 diffuseColor;
  bool disableLighting;
  unsigned int maxSimultaneousLights;

private:
  Matrix _worldViewProjectionMatrix;
  Color3 _scaledDiffuse;
  int _renderId;
  LavaMaterialDefines _defines;
  std::unique_ptr<LavaMaterialDefines> _cachedDefines;

}; // end of class LavaMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_H
