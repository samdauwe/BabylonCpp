#ifndef BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materialslibrary/water/water_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT WaterMaterial : public Material {

public:
  using WMD = WaterMaterialDefines;

public:
  WaterMaterial(const std::string& name, Scene* scene,
                const Vector2& renderTargetSize = Vector2(512.f, 512.f));
  ~WaterMaterial();

  // Get / Set
  bool useLogarithmicDepth() const;
  void setUseLogarithmicDepth(bool value);
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
  static WaterMaterial* Parse(const Json::value& source, Scene* scene,
                              const std::string& rootUrl);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);
  void _createRenderTargets(Scene* scene, const Vector2& renderTargetSize);

public:
  BaseTexture* bumpTexture;
  Color3 diffuseColor;
  Color3 specularColor;
  float specularPower;
  bool disableLighting;
  unsigned int maxSimultaneousLights;
  /**
   * @param {number}: Represents the wind force
   */
  float windForce;
  /**
   * @param {Vector2}: The direction of the wind in the plane (X, Z)
   */
  Vector2 windDirection;
  /**
   * @param {number}: Wave height, represents the height of the waves
   */
  float waveHeight;
  /**
   * @param {number}: Bump height, represents the bump height related to the
   * bump map
   */
  float bumpHeight;
  /**
  * @param {boolean}: Add a smaller moving bump to less steady waves.
  */
  bool bumpSuperimpose;
  /**
   * @param {boolean}: Color refraction and reflection differently with
   * .waterColor2 and .colorBlendFactor2. Non-linear (physically correct)
   * fresnel.
   */
  bool fresnelSeparate;
  /**
   * @param {boolean}: bump Waves modify the reflection.
   */
  bool bumpAffectsReflection;
  /**
   * @param {number}: The water color blended with the reflection and refraction
   * samplers
   */
  Color3 waterColor;
  /**
   * @param {number}: The blend factor related to the water color
   */
  float colorBlendFactor;
  /**
   * @param {number}: The water color blended with the reflection (far)
   */
  Color3 waterColor2;
  /**
   * @param {number}: The blend factor related to the water color (reflection,
   * far)
   */
  float colorBlendFactor2;
  /**
   * @param {number}: Represents the maximum length of a wave
   */
  float waveLength;
  /**
   * @param {number}: Defines the waves speed
   */
  float waveSpeed;

private:
  AbstractMesh* _mesh;
  RenderTargetTexture* _refractionRTT;
  RenderTargetTexture* _reflectionRTT;
  ShaderMaterial* _material;
  Matrix _reflectionTransform;
  microseconds_t _lastTime;
  int _renderId;
  WaterMaterialDefines _defines;
  std::unique_ptr<WaterMaterialDefines> _cachedDefines;
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
