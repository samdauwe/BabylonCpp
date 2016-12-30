#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StandardMaterial : public Material {

public:
  // Flags used to enable or disable a type of texture for all Standard
  // Materials
  static bool DiffuseTextureEnabled;
  static bool AmbientTextureEnabled;
  static bool OpacityTextureEnabled;
  static bool ReflectionTextureEnabled;
  static bool EmissiveTextureEnabled;
  static bool SpecularTextureEnabled;
  static bool BumpTextureEnabled;
  static bool FresnelEnabled;
  static bool LightmapTextureEnabled;
  static bool RefractionTextureEnabled;
  static bool ColorGradingTextureEnabled;

  static const Color3 _scaledDiffuse;
  static const Color3 _scaledSpecular;

  using SMD = StandardMaterialDefines;

public:
  template <typename... Ts>
  static StandardMaterial* New(Ts&&... args)
  {
    auto material = new StandardMaterial(std::forward<Ts>(args)...);
    material->addMaterialToScene(
      static_cast<std::unique_ptr<Material>>(material));

    return material;
  }
  ~StandardMaterial();

  Material::Type type() const override;

  void setAmbientColor(const Color3& color) override;
  void setDiffuseColor(const Color3& color) override;
  void setSpecularColor(const Color3& color) override;
  void setEmissiveColor(const Color3& color) override;

  bool useLogarithmicDepth() const;
  void setUseLogarithmicDepth(bool value);
  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;

  /** Overriden Methods **/
  bool isReady(AbstractMesh* mesh, bool useInstances) override;
  void unbind() override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  // Statics
  static std::unique_ptr<StandardMaterial>
  Parse(const Json::value& source, Scene* scene, const std::string& url);

protected:
  StandardMaterial(const std::string& name, Scene* scene);
  StandardMaterial(const StandardMaterial& other);

private:
  bool _shouldUseAlphaFromDiffuseTexture();
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  BaseTexture* diffuseTexture;
  BaseTexture* ambientTexture;
  BaseTexture* opacityTexture;
  RenderTargetTexture* reflectionTexture;
  BaseTexture* emissiveTexture;
  BaseTexture* specularTexture;
  BaseTexture* bumpTexture;
  BaseTexture* lightmapTexture;
  RenderTargetTexture* refractionTexture;
  Color3 ambientColor;
  Color3 diffuseColor;
  Color3 specularColor;
  Color3 emissiveColor;
  float specularPower;
  bool useAlphaFromDiffuseTexture;
  bool useEmissiveAsIllumination;
  bool linkEmissiveWithDiffuse;
  bool useReflectionFresnelFromSpecular;
  bool useSpecularOverAlpha;
  bool useReflectionOverAlpha;
  bool disableLighting;
  bool useParallax;
  bool useParallaxOcclusion;
  float parallaxScaleBias;
  float roughness;
  float indexOfRefraction;
  bool invertRefractionY;
  bool useLightmapAsShadowmap;
  std::unique_ptr<FresnelParameters> diffuseFresnelParameters;
  std::unique_ptr<FresnelParameters> opacityFresnelParameters;
  std::unique_ptr<FresnelParameters> reflectionFresnelParameters;
  std::unique_ptr<FresnelParameters> refractionFresnelParameters;
  std::unique_ptr<FresnelParameters> emissiveFresnelParameters;
  bool useGlossinessFromSpecularMapAlpha;
  unsigned int maxSimultaneousLights;
  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  bool invertNormalMapX;
  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  bool invertNormalMapY;

  /**
   * Color Grading 2D Lookup Texture.
   * This allows special effects like sepia, black and white to sixties
   * rendering style.
   */
  BaseTexture* cameraColorGradingTexture;

  /**
   * The color grading curves provide additional color adjustmnent that is
   * applied after any color grading transform (3D LUT).
   * They allow basic adjustment of saturation and small exposure adjustments,
   * along with color filter tinting to provide white balance adjustment or more
   * stylistic effects.
   * These are similar to controls found in many professional imaging or
   * colorist software. The global controls are applied to the entire image. For
   * advanced tuning, extra controls are provided to adjust the shadow, midtone
   * and highlight areas of the image;
   * corresponding to low luminance, medium luminance, and high luminance areas
   * respectively.
   */
  ColorCurves* cameraColorCurves;

private:
  std::vector<RenderTargetTexture*> _renderTargets;
  Matrix _worldViewProjectionMatrix;
  Color3 _globalAmbientColor;
  int _renderId;
  StandardMaterialDefines _defines;
  std::unique_ptr<StandardMaterialDefines> _cachedDefines;
  bool _useLogarithmicDepth;

}; // end of class StandardMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_H
