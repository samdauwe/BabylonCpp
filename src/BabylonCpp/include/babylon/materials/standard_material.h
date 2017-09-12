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
  static StandardMaterial* New(Ts&&... args)
  {
    auto material = new StandardMaterial(std::forward<Ts>(args)...);
    material->addMaterialToScene(
      static_cast<std::unique_ptr<Material>>(material));

    return material;
  }
  ~StandardMaterial();

  /**
   * @brief Returns the string "StandardMaterial".
   */
  const char* getClassName() const;

  IReflect::Type type() const override;

  void setAmbientColor(const Color3& color) override;
  void setDiffuseColor(const Color3& color) override;
  void setSpecularColor(const Color3& color) override;
  void setEmissiveColor(const Color3& color) override;

  bool useLogarithmicDepth() const;
  void setUseLogarithmicDepth(bool value);
  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;

  /**
   * Child classes can use it to update shaders
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void buildUniformLayout();
  void unbind() override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  std::vector<BaseTexture*> getActiveTextures() const override;
  bool hasTexture(BaseTexture* texture) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  // Getters / Setters
  BaseTexture* emissiveTexture() const;
  bool useAlphaFromDiffuseTexture() const;
  void setUseAlphaFromDiffuseTexture(bool value);
  bool useEmissiveAsIllumination() const;
  void setUseEmissiveAsIllumination(bool value);
  bool linkEmissiveWithDiffuse() const;
  void setLinkEmissiveWithDiffuse(bool value);
  bool useReflectionFresnelFromSpecular() const;
  void setUseReflectionFresnelFromSpecular(bool value);
  bool useSpecularOverAlpha() const;
  void setUseSpecularOverAlpha(bool value);
  bool useReflectionOverAlpha() const;
  void setUseReflectionOverAlpha(bool value);
  bool disableLighting() const;
  void setDisableLighting(bool value);
  bool useParallax() const;
  void setUseParallax(bool value);
  bool useParallaxOcclusion() const;
  void setUseParallaxOcclusion(bool value);
  float roughness() const;
  void setRoughness(float value);
  bool useLightmapAsShadowmap() const;
  void setUseLightmapAsShadowmap(bool value);
  bool useGlossinessFromSpecularMapAlpha() const;
  void setUseGlossinessFromSpecularMapAlpha(bool value);
  unsigned int maxSimultaneousLights() const;
  void setMaxSimultaneousLights(unsigned int value);
  bool invertNormalMapX() const;
  void setInvertNormalMapX(bool value);
  bool invertNormalMapY() const;
  void setInvertNormalMapY(bool value);

  /**
   * @brief Gets the image processing configuration used either in this
   * material.
   */
  ImageProcessingConfiguration* imageProcessingConfiguration() const;

  /**
   * @brief Sets the Default image processing configuration used either in the
   * this material.
   *
   * If sets to null, the scene one is in use.
   */
  void setImageProcessingConfiguration(ImageProcessingConfiguration* value);

  /**
   * @brief Gets whether the color curves effect is enabled.
   */
  bool cameraColorCurvesEnabled() const;

  /**
   * @brief Sets whether the color curves effect is enabled.
   */
  void setCameraColorCurvesEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  bool cameraColorGradingEnabled() const;

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  void setCameraColorGradingEnabled(bool value);

  /**
   * @brief Gets wether tonemapping is enabled or not.
   */
  bool cameraToneMappingEnabled() const;

  /**
   * @brief Sets wether tonemapping is enabled or not
   */
  void setCameraToneMappingEnabled(bool value);

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  float cameraExposure() const;

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  void setCameraExposure(float value);

  /**
   * @brief Gets The camera contrast used on this material.
   */
  float cameraContrast() const;

  /**
   * @brief Sets The camera contrast used on this material.
   */
  void setCameraContrast(float value);

  /**
   * @brief Gets the Color Grading 2D Lookup Texture.
   */
  BaseTexture* cameraColorGradingTexture() const;

  /**
   * @brief Sets the Color Grading 2D Lookup Texture.
   */
  void setCameraColorGradingTexture(BaseTexture* value);

  // Statics
  static StandardMaterial* Parse(const Json::value& source, Scene* scene,
                                 const std::string& rootUrl);
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
  StandardMaterial(const std::string& name, Scene* scene);
  StandardMaterial(const StandardMaterial& other);

protected:
  /**
   * Attaches a new image processing configuration to the Standard Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(
    ImageProcessingConfiguration* configuration);

  bool _shouldUseAlphaFromDiffuseTexture();
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
  ::std::function<std::string(
    const std::string& shaderName, std::vector<std::string>& uniforms,
    std::vector<std::string>& uniformBuffers,
    std::vector<std::string>& samplers, StandardMaterialDefines& defines)>
    customShaderNameResolve;

protected:
  std::vector<RenderTargetTexture*> _renderTargets;
  Matrix _worldViewProjectionMatrix;
  Color3 _globalAmbientColor;
  bool _useLogarithmicDepth;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  ImageProcessingConfiguration* _imageProcessingConfiguration;

private:
  BaseTexture* _diffuseTexture;
  BaseTexture* _ambientTexture;
  BaseTexture* _opacityTexture;
  RenderTargetTexture* _reflectionTexture;
  BaseTexture* _emissiveTexture;
  BaseTexture* _specularTexture;
  BaseTexture* _bumpTexture;
  BaseTexture* _lightmapTexture;
  RenderTargetTexture* _refractionTexture;
  bool _useAlphaFromDiffuseTexture;
  bool _useEmissiveAsIllumination;
  bool _linkEmissiveWithDiffuse;
  bool _useReflectionFresnelFromSpecular;
  bool _useSpecularOverAlpha;
  bool _useReflectionOverAlpha;
  bool _disableLighting;
  bool _useParallax;
  bool _useParallaxOcclusion;
  float _roughness;
  bool _useLightmapAsShadowmap;
  std::unique_ptr<FresnelParameters> _diffuseFresnelParameters;
  std::unique_ptr<FresnelParameters> _opacityFresnelParameters;
  std::unique_ptr<FresnelParameters> _reflectionFresnelParameters;
  std::unique_ptr<FresnelParameters> _refractionFresnelParameters;
  std::unique_ptr<FresnelParameters> _emissiveFresnelParameters;
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
