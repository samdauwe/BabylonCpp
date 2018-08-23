#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/math/color3.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StandardMaterial : public PushMaterial {

public:
  using SMD = StandardMaterialDefines;

private:
  // Flags used to enable or disable a type of texture for all Standard
  // Materials
  static bool _DiffuseTextureEnabled;
  static bool _AmbientTextureEnabled;
  static bool _OpacityTextureEnabled;
  static bool _ReflectionTextureEnabled;
  static bool _EmissiveTextureEnabled;
  static bool _SpecularTextureEnabled;
  static bool _BumpTextureEnabled;
  static bool _FresnelEnabled;
  static bool _LightmapTextureEnabled;
  static bool _RefractionTextureEnabled;
  static bool _ColorGradingTextureEnabled;

public:
  template <typename... Ts>
  static StandardMaterialPtr New(Ts&&... args)
  {
    auto material = shared_ptr_t<StandardMaterial>(
      new StandardMaterial(::std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~StandardMaterial() override;

  /**
   * @brief Returns the string "StandardMaterial".
   */
  const string_t getClassName() const override;

  IReflect::Type type() const override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;

  /**
   * Child classes can use it to update shaders
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void buildUniformLayout();
  void unbind() override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  vector_t<IAnimatablePtr> getAnimatables();
  vector_t<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  MaterialPtr clone(const string_t& name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;

  // Getters / Setters
  BaseTexturePtr& diffuseTexture();
  void setDiffuseTexture(const BaseTexturePtr& value);
  BaseTexturePtr& ambientTexture();
  void setAmbientTexture(const BaseTexturePtr& value);
  BaseTexturePtr& opacityTexture();
  void setOpacityTexture(const BaseTexturePtr& value);
  BaseTexturePtr& reflectionTexture();
  void setReflectionTexture(const BaseTexturePtr& value);
  BaseTexturePtr& emissiveTexture();
  void setEmissiveTexture(const BaseTexturePtr& value);
  BaseTexturePtr& specularTexture();
  void setSpecularTexture(const BaseTexturePtr& value);
  BaseTexturePtr& bumpTexture();
  void setBumpTexture(const BaseTexturePtr& value);
  BaseTexturePtr& lightmapTexture();
  void setLightmapTexture(const BaseTexturePtr& value);
  BaseTexturePtr& refractionTexture();
  void setRefractionTexture(const BaseTexturePtr& value);
  bool useAlphaFromDiffuseTexture() const;
  void setUseAlphaFromDiffuseTexture(bool value);
  bool useEmissiveAsIllumination() const;
  void setUseEmissiveAsIllumination(bool value);
  bool linkEmissiveWithDiffuse() const;
  void setLinkEmissiveWithDiffuse(bool value);
  bool useSpecularOverAlpha() const;
  void setUseSpecularOverAlpha(bool value);
  bool useReflectionOverAlpha() const;
  void setUseReflectionOverAlpha(bool value);
  bool disableLighting() const;
  void setDisableLighting(bool value);
  bool useObjectSpaceNormalMap() const;
  void setUseObjectSpaceNormalMap(bool value);
  bool useParallax() const;
  void setUseParallax(bool value);
  bool useParallaxOcclusion() const;
  void setUseParallaxOcclusion(bool value);
  float roughness() const;
  void setRoughness(float value);
  bool useLightmapAsShadowmap() const;
  void setUseLightmapAsShadowmap(bool value);

  // Fresnel
  FresnelParameters* diffuseFresnelParameters();
  FresnelParameters* opacityFresnelParameters();
  FresnelParameters* reflectionFresnelParameters();
  FresnelParameters* refractionFresnelParameters();
  FresnelParameters* emissiveFresnelParameters();
  bool useReflectionFresnelFromSpecular() const;
  void setUseReflectionFresnelFromSpecular(bool value);
  bool useGlossinessFromSpecularMapAlpha() const;
  void setUseGlossinessFromSpecularMapAlpha(bool value);
  unsigned int maxSimultaneousLights() const;
  void setMaxSimultaneousLights(unsigned int value);
  bool invertNormalMapX() const;
  void setInvertNormalMapX(bool value);
  bool invertNormalMapY() const;
  void setInvertNormalMapY(bool value);
  bool twoSidedLighting() const;
  void setTwoSidedLighting(bool value);

  // Statics
  static StandardMaterialPtr Parse(const Json::value& source, Scene* scene,
                                   const string_t& rootUrl);
  static bool DiffuseTextureEnabled();
  static void SetDiffuseTextureEnabled(bool value);
  static bool AmbientTextureEnabled();
  static void SetAmbientTextureEnabled(bool value);
  static bool OpacityTextureEnabled();
  static void SetOpacityTextureEnabled(bool value);
  static bool ReflectionTextureEnabled();
  static void SetReflectionTextureEnabled(bool value);
  static bool EmissiveTextureEnabled();
  static void SetEmissiveTextureEnabled(bool value);
  static bool SpecularTextureEnabled();
  static void SetSpecularTextureEnabled(bool value);
  static bool BumpTextureEnabled();
  static void SetBumpTextureEnabled(bool value);
  static bool LightmapTextureEnabled();
  static void SetLightmapTextureEnabled(bool value);
  static bool RefractionTextureEnabled();
  static void SetRefractionTextureEnabled(bool value);
  static bool ColorGradingTextureEnabled();
  static void SetColorGradingTextureEnabled(bool value);
  static bool FresnelEnabled();
  static void SetFresnelEnabled(bool value);

protected:
  StandardMaterial(const string_t& name, Scene* scene);
  StandardMaterial(const StandardMaterial& other);

  /**
   * @brief Gets the image processing configuration used either in this
   * material.
   */
  ImageProcessingConfiguration*& get_imageProcessingConfiguration();

  /**
   * @brief Sets the Default image processing configuration used either in the
   * this material.
   *
   * If sets to null, the scene one is in use.
   */
  void
  set_imageProcessingConfiguration(ImageProcessingConfiguration* const& value);

  /**
   * @brief Gets whether the color curves effect is enabled.
   */
  bool get_cameraColorCurvesEnabled() const;

  /**
   * @brief Sets whether the color curves effect is enabled.
   */
  void set_cameraColorCurvesEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  bool get_cameraColorGradingEnabled() const;

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  void set_cameraColorGradingEnabled(bool value);

  /**
   * @brief Gets wether tonemapping is enabled or not.
   */
  bool get_cameraToneMappingEnabled() const;

  /**
   * @brief Sets wether tonemapping is enabled or not
   */
  void set_cameraToneMappingEnabled(bool value);

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  float get_cameraExposure() const;

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  void set_cameraExposure(float value);

  /**
   * @brief Gets The camera contrast used on this material.
   */
  float get_cameraContrast() const;

  /**
   * @brief Sets The camera contrast used on this material.
   */
  void set_cameraContrast(float value);

  /**
   * @brief Gets the Color Grading 2D Lookup Texture.
   */
  BaseTexturePtr& get_cameraColorGradingTexture();

  /**
   * @brief Sets the Color Grading 2D Lookup Texture.
   */
  void set_cameraColorGradingTexture(const BaseTexturePtr& value);

  /**
   * @brief The color grading curves provide additional color adjustmnent that
   * is applied after any color grading transform (3D LUT).
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
  shared_ptr_t<ColorCurves>& get_cameraColorCurves();

  /**
   * @brief The color grading curves provide additional color adjustmnent that
   * is applied after any color grading transform (3D LUT).
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
  void set_cameraColorCurves(const shared_ptr_t<ColorCurves>& value);

  /**
   * @brief Gets the logarithmic depth setting.
   */
  bool get_useLogarithmicDepth() const override;

  /**
   * @brief Sets the logarithmic depth setting.
   */
  void set_useLogarithmicDepth(bool value) override;

protected:
  /**
   * Attaches a new image processing configuration to the Standard Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(
    ImageProcessingConfiguration* configuration);

  bool _shouldUseAlphaFromDiffuseTexture() const;
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  Color3 ambientColor;
  Color3 diffuseColor;
  Color3 specularColor;
  Color3 emissiveColor;
  float specularPower;
  float parallaxScaleBias;
  float indexOfRefraction;
  bool invertRefractionY;

  /**
   * Defines the alpha limits in alpha test mode
   */
  float alphaCutOff;

  ::std::function<string_t(
    const string_t& shaderName, vector_t<string_t>& uniforms,
    vector_t<string_t>& uniformBuffers, vector_t<string_t>& samplers,
    StandardMaterialDefines& defines)>
    customShaderNameResolve;

  /**
   * The image processing configuration used either in this material
   */
  Property<StandardMaterial, ImageProcessingConfiguration*>
    imageProcessingConfiguration;

  /**
   * Whether the color curves effect is enabled
   */
  Property<StandardMaterial, bool> cameraColorCurvesEnabled;

  /**
   * Wether the color grading effect is enabled
   */
  Property<StandardMaterial, bool> cameraColorGradingEnabled;

  /**
   * Wether tonemapping is enabled or not
   */
  Property<StandardMaterial, bool> cameraToneMappingEnabled;

  /**
   * The camera exposure used on this material
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  Property<StandardMaterial, float> cameraExposure;

  /**
   * The camera contrast used on this material.
   */
  Property<StandardMaterial, float> cameraContrast;

  /**
   * The Color Grading 2D Lookup Texture.
   */
  Property<StandardMaterial, BaseTexturePtr> cameraColorGradingTexture;

  /**
   * The color grading curves provide additional color adjustmnent that
   * is applied after any color grading transform (3D LUT).
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
  Property<StandardMaterial, shared_ptr_t<ColorCurves>> cameraColorCurves;

protected:
  vector_t<RenderTargetTexturePtr> _renderTargets;
  Matrix _worldViewProjectionMatrix;
  Color3 _globalAmbientColor;
  bool _useLogarithmicDepth;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  ImageProcessingConfiguration* _imageProcessingConfiguration;

private:
  BaseTexturePtr _diffuseTexture;
  BaseTexturePtr _ambientTexture;
  BaseTexturePtr _opacityTexture;
  BaseTexturePtr _reflectionTexture;
  BaseTexturePtr _emissiveTexture;
  BaseTexturePtr _specularTexture;
  BaseTexturePtr _bumpTexture;
  BaseTexturePtr _lightmapTexture;
  BaseTexturePtr _refractionTexture;
  bool _useAlphaFromDiffuseTexture;
  bool _useEmissiveAsIllumination;
  bool _linkEmissiveWithDiffuse;
  bool _useReflectionFresnelFromSpecular;
  bool _useSpecularOverAlpha;
  bool _useReflectionOverAlpha;
  bool _disableLighting;
  bool _useObjectSpaceNormalMap;
  bool _useParallax;
  bool _useParallaxOcclusion;
  float _roughness;
  bool _useLightmapAsShadowmap;
  unique_ptr_t<FresnelParameters> _diffuseFresnelParameters;
  unique_ptr_t<FresnelParameters> _opacityFresnelParameters;
  unique_ptr_t<FresnelParameters> _reflectionFresnelParameters;
  unique_ptr_t<FresnelParameters> _refractionFresnelParameters;
  unique_ptr_t<FresnelParameters> _emissiveFresnelParameters;
  bool _useGlossinessFromSpecularMapAlpha;
  unsigned int _maxSimultaneousLights;

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  bool _invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  bool _invertNormalMapY;

  /**
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  bool _twoSidedLighting;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

}; // end of class StandardMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_H
