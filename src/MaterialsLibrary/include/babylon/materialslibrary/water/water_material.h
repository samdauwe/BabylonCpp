#ifndef BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/water/water_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT WaterMaterial : public PushMaterial {

public:
  using WMD = WaterMaterialDefines;

public:
  WaterMaterial(const std::string& name, Scene* scene,
                const Vector2& renderTargetSize = Vector2(512.f, 512.f));
  ~WaterMaterial();

  // Get / Set
  bool useLogarithmicDepth() const override;
  void setUseLogarithmicDepth(bool value) override;
  RenderTargetTexture* refractionTexture();
  RenderTargetTexture* reflectionTexture();

  // Methods
  void addToRenderList(AbstractMesh* node);
  void enableRenderTargets(bool enable);
  std::vector<AbstractMesh*>& getRenderList();
  bool renderTargetsEnabled() const;
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
  static WaterMaterial* Parse(const Json::value& source, Scene* scene,
                              const std::string& rootUrl);
  static Mesh* CreateDefaultMesh(const std::string& name, Scene* scene);

private:
  void _createRenderTargets(Scene* scene, const Vector2& renderTargetSize);

public:
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  /**
   * @param Represents the wind force
   */
  float windForce;
  /**
   * @param The direction of the wind in the plane (X, Z)
   */
  Vector2 windDirection;
  /**
   * @param Wave height, represents the height of the waves
   */
  float waveHeight;
  /**
   * @param Bump height, represents the bump height related to the
   * bump map
   */
  float bumpHeight;
  /**
   * @param The water color blended with the reflection and refraction samplers
   */
  Color3 waterColor;
  /**
   * @param The blend factor related to the water color
   */
  float colorBlendFactor;
  /**
   * @param The water color blended with the reflection (far)
   */
  Color3 waterColor2;
  /**
   * @param The blend factor related to the water color (reflection,
   * far)
   */
  float colorBlendFactor2;
  /**
   * @param Represents the maximum length of a wave
   */
  float waveLength;
  /**
   * @param Defines the waves speed
   */
  float waveSpeed;

private:
  BaseTexture* _bumpTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;

  /**
   * @param Add a smaller moving bump to less steady waves.
   */
  bool _bumpSuperimpose;
  /**
   * @param Color refraction and reflection differently with .waterColor2 and
   * .colorBlendFactor2. Non-linear (physically correct)
   * fresnel.
   */
  bool _fresnelSeparate;
  /**
   * @param Bump Waves modify the reflection.
   */
  bool _bumpAffectsReflection;

  AbstractMesh* _mesh;
  std::unique_ptr<RenderTargetTexture> _refractionRTT;
  std::unique_ptr<RenderTargetTexture> _reflectionRTT;
  ShaderMaterial* _material;
  Matrix _reflectionTransform;
  float _lastTime;
  int _renderId;
  // Needed for callbacks
  bool _isVisible;
  Plane* _clipPlane;
  Matrix _savedViewMatrix;
  Matrix _mirrorMatrix;
  bool _useLogarithmicDepth;

}; // end of class WaterMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H
