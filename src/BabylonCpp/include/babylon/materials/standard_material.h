#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class ColorCurves;
class FresnelParameters;
class IAnimatable;
class ImageProcessingConfiguration;
class StandardMaterial;
struct StandardMaterialDefines;
using FresnelParametersPtr = std::shared_ptr<FresnelParameters>;
using IAnimatablePtr       = std::shared_ptr<IAnimatable>;
using StandardMaterialPtr  = std::shared_ptr<StandardMaterial>;

/**
 * @brief This is the default material used in Babylon. It is the best trade off between quality and
 * performances.
 * @see http://doc.babylonjs.com/babylon101/materials
 */
class BABYLON_SHARED_EXPORT StandardMaterial : public PushMaterial {

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
    auto material
      = std::shared_ptr<StandardMaterial>(new StandardMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~StandardMaterial() override; // = default

  /**
   * @brief Gets the current class name of the material e.g. "StandardMaterial".
   * Mainly use in serialization.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  [[nodiscard]] Type type() const override;

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  [[nodiscard]] bool needAlphaBlending() const override;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode.
   * @returns a boolean specifying if an alpha test is needed.
   */
  [[nodiscard]] bool needAlphaTesting() const override;

  /**
   * @brief Get the texture used for alpha test purpose.
   * @returns the diffuse texture in case of the standard material.
   */
  BaseTexturePtr getAlphaTestTexture() override;

  /**
   * @brief Get if the submesh is ready to be used and all its information
   * available. Child classes can use it to update shaders.
   * @param mesh defines the mesh to check
   * @param subMesh defines which submesh to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances = false) override;

  /**
   * @brief Builds the material UBO layouts.
   * Used internally during the effect preparation.
   */
  void buildUniformLayout();

  /**
   * @brief Unbinds the material from the mesh.
   */
  void unbind() override;

  /**
   * @brief Binds the submesh to this material by preparing the effect and
   * shader to draw.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh containing the submesh
   * @param subMesh defines the submesh to bind the material to
   */
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Get the list of animatables in the material.
   * @returns the list of animatables object used in the material
   */
  std::vector<IAnimatablePtr> getAnimatables() override;

  /**
   * @brief Gets the active textures from the material.
   * @returns an array of textures
   */
  [[nodiscard]] std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture defines the texture to check against the material
   * @returns a boolean specifying if the material uses the texture
   */
  [[nodiscard]] bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect specifies if effects should be forcefully
   * disposed
   * @param forceDisposeTextures specifies if textures should be forcefully
   * disposed
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;

  /**
   * @brief Makes a duplicate of the material, and gives it a new name.
   * @param name defines the new name for the duplicated material
   * @returns the cloned material
   */
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;

  /**
   * @brief Serializes this material in a JSON representation.
   * @returns the serialized material object
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Creates a standard material from parsed material data.
   * @param source defines the JSON representation of the material
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures and relative
   * dependencies
   * @returns a new standard material
   */
  static StandardMaterialPtr Parse(const json& source, Scene* scene, const std::string& rootUrl);

  /**
   * Are diffuse textures enabled in the application.
   */
  static bool DiffuseTextureEnabled();
  static void SetDiffuseTextureEnabled(bool value);

  /**
   * Are ambient textures enabled in the application.
   */
  static bool AmbientTextureEnabled();
  static void SetAmbientTextureEnabled(bool value);

  /**
   * Are opacity textures enabled in the application.
   */
  static bool OpacityTextureEnabled();
  static void SetOpacityTextureEnabled(bool value);

  /**
   * Are reflection textures enabled in the application.
   */
  static bool ReflectionTextureEnabled();
  static void SetReflectionTextureEnabled(bool value);

  /**
   * Are emissive textures enabled in the application.
   */
  static bool EmissiveTextureEnabled();
  static void SetEmissiveTextureEnabled(bool value);

  /**
   * Are specular textures enabled in the application.
   */
  static bool SpecularTextureEnabled();
  static void SetSpecularTextureEnabled(bool value);

  /**
   * Are bump textures enabled in the application.
   */
  static bool BumpTextureEnabled();
  static void SetBumpTextureEnabled(bool value);

  /**
   * Are lightmap textures enabled in the application.
   */
  static bool LightmapTextureEnabled();
  static void SetLightmapTextureEnabled(bool value);

  /**
   * Are refraction textures enabled in the application.
   */
  static bool RefractionTextureEnabled();
  static void SetRefractionTextureEnabled(bool value);

  /**
   * Are color grading textures enabled in the application.
   */
  static bool ColorGradingTextureEnabled();
  static void SetColorGradingTextureEnabled(bool value);

  /**
   * Are fresnels enabled in the application.
   */
  static bool FresnelEnabled();
  static void SetFresnelEnabled(bool value);

protected:
  /**
   * @brief Instantiates a new standard material.
   * This is the default material used in Babylon. It is the best trade off
   * between quality and performances.
   * @see http://doc.babylonjs.com/babylon101/materials
   * @param name Define the name of the material in the scene
   * @param scene Define the scene the material belong to
   */
  StandardMaterial(const std::string& name, Scene* scene);
  StandardMaterial(const StandardMaterial& other);

  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_ambientTexture();
  void set_ambientTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_opacityTexture();
  void set_opacityTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_reflectionTexture();
  void set_reflectionTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_emissiveTexture();
  void set_emissiveTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_specularTexture();
  void set_specularTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_bumpTexture();
  void set_bumpTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_lightmapTexture();
  void set_lightmapTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_refractionTexture();
  void set_refractionTexture(const BaseTexturePtr& value);

  [[nodiscard]] bool get_useAlphaFromDiffuseTexture() const;
  void set_useAlphaFromDiffuseTexture(bool value);
  [[nodiscard]] bool get_useEmissiveAsIllumination() const;
  void set_useEmissiveAsIllumination(bool value);
  [[nodiscard]] bool get_linkEmissiveWithDiffuse() const;
  void set_linkEmissiveWithDiffuse(bool value);
  [[nodiscard]] bool get_useSpecularOverAlpha() const;
  void set_useSpecularOverAlpha(bool value);
  [[nodiscard]] bool get_useReflectionOverAlpha() const;
  void set_useReflectionOverAlpha(bool value);
  [[nodiscard]] bool get_disableLighting() const;
  void set_disableLighting(bool value);
  [[nodiscard]] bool get_useObjectSpaceNormalMap() const;
  void set_useObjectSpaceNormalMap(bool value);
  [[nodiscard]] bool get_useParallax() const;
  void set_useParallax(bool value);
  [[nodiscard]] bool get_useParallaxOcclusion() const;
  void set_useParallaxOcclusion(bool value);
  [[nodiscard]] float get_roughness() const;
  void set_roughness(float value);
  [[nodiscard]] bool get_useLightmapAsShadowmap() const;
  void set_useLightmapAsShadowmap(bool value);

  // Fresnel
  FresnelParametersPtr& get_diffuseFresnelParameters();
  void set_diffuseFresnelParameters(const FresnelParametersPtr& value);
  FresnelParametersPtr& get_opacityFresnelParameters();
  void set_opacityFresnelParameters(const FresnelParametersPtr& value);
  FresnelParametersPtr& get_reflectionFresnelParameters();
  void set_reflectionFresnelParameters(const FresnelParametersPtr& value);
  FresnelParametersPtr& get_refractionFresnelParameters();
  void set_refractionFresnelParameters(const FresnelParametersPtr& value);
  FresnelParametersPtr& get_emissiveFresnelParameters();
  void set_emissiveFresnelParameters(const FresnelParametersPtr& value);
  [[nodiscard]] bool get_useReflectionFresnelFromSpecular() const;
  void set_useReflectionFresnelFromSpecular(bool value);
  [[nodiscard]] bool get_useGlossinessFromSpecularMapAlpha() const;
  void set_useGlossinessFromSpecularMapAlpha(bool value);
  [[nodiscard]] unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);
  [[nodiscard]] bool get_invertNormalMapX() const;
  void set_invertNormalMapX(bool value);
  [[nodiscard]] bool get_invertNormalMapY() const;
  void set_invertNormalMapY(bool value);
  [[nodiscard]] bool get_twoSidedLighting() const;
  void set_twoSidedLighting(bool value);

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
  void set_imageProcessingConfiguration(ImageProcessingConfiguration* const& value);

  /**
   * @brief Gets whether the color curves effect is enabled.
   */
  [[nodiscard]] bool get_cameraColorCurvesEnabled() const;

  /**
   * @brief Sets whether the color curves effect is enabled.
   */
  void set_cameraColorCurvesEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  [[nodiscard]] bool get_cameraColorGradingEnabled() const;

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  void set_cameraColorGradingEnabled(bool value);

  /**
   * @brief Gets wether tonemapping is enabled or not.
   */
  [[nodiscard]] bool get_cameraToneMappingEnabled() const;

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
  [[nodiscard]] float get_cameraExposure() const;

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
  [[nodiscard]] float get_cameraContrast() const;

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
  std::shared_ptr<ColorCurves>& get_cameraColorCurves();

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
  void set_cameraColorCurves(const std::shared_ptr<ColorCurves>& value);

  /**
   * @brief Gets a boolean indicating that current material needs to register
   * RTT.
   */
  [[nodiscard]] bool get_hasRenderTargetTextures() const override;

  /**
   * @brief Gets the logarithmic depth setting.
   */
  [[nodiscard]] bool get_useLogarithmicDepth() const override;

  /**
   * @brief Sets the logarithmic depth setting.
   */
  void set_useLogarithmicDepth(bool value) override;

protected:
  /**
   * Attaches a new image processing configuration to the Standard Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(ImageProcessingConfiguration* configuration);

  [[nodiscard]] bool _shouldUseAlphaFromDiffuseTexture() const;
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);

public:
  /**
   * The basic texture of the material as viewed under a light.
   */
  Property<StandardMaterial, BaseTexturePtr> diffuseTexture;

  /**
   * AKA Occlusion Texture in other nomenclature, it helps adding baked shadows
   * into your material.
   */
  Property<StandardMaterial, BaseTexturePtr> ambientTexture;

  /**
   * Define the transparency of the material from a texture.
   * The final alpha value can be read either from the red channel (if
   * texture.getAlphaFromRGB is false) or from the luminance or the current
   * texel (if texture.getAlphaFromRGB is true)
   */
  Property<StandardMaterial, BaseTexturePtr> opacityTexture;

  /**
   * Define the texture used to display the reflection.
   * @see
   * http://doc.babylonjs.com/how_to/reflect#how-to-obtain-reflections-and-refractions
   */
  Property<StandardMaterial, BaseTexturePtr> reflectionTexture;

  /**
   * Define texture of the material as if self lit.
   * This will be mixed in the final result even in the absence of light.
   */
  Property<StandardMaterial, BaseTexturePtr> emissiveTexture;

  /**
   * Define how the color and intensity of the highlight given by the light in
   * the material.
   */
  Property<StandardMaterial, BaseTexturePtr> specularTexture;

  /**
   * Bump mapping is a technique to simulate bump and dents on a rendered
   * surface. These are made by creating a normal map from an image. The means
   * to do this can be found on the web, a search for 'normal map generator'
   * will bring up free and paid for methods of doing this.
   * @see http://doc.babylonjs.com/how_to/more_materials#bump-map
   */
  Property<StandardMaterial, BaseTexturePtr> bumpTexture;

  /**
   * Complex lighting can be computationally expensive to compute at runtime.
   * To save on computation, lightmaps may be used to store calculated lighting
   * in a texture which will be applied to a given mesh.
   * @see http://doc.babylonjs.com/babylon101/lights#lightmaps
   */
  Property<StandardMaterial, BaseTexturePtr> lightmapTexture;

  /**
   * Define the texture used to display the refraction.
   * @see
   * http://doc.babylonjs.com/how_to/reflect#how-to-obtain-reflections-and-refractions
   */
  Property<StandardMaterial, BaseTexturePtr> refractionTexture;

  /**
   * The color of the material lit by the environmental background lighting.
   * @see http://doc.babylonjs.com/babylon101/materials#ambient-color-example
   */
  Color3 ambientColor;

  /**
   * The basic color of the material as viewed under a light.
   */
  Color3 diffuseColor;

  /**
   * Define how the color and intensity of the highlight given by the light in
   * the material.
   */
  Color3 specularColor;

  /**
   * Define the color of the material as if self lit.
   * This will be mixed in the final result even in the absence of light.
   */
  Color3 emissiveColor;

  /**
   * Defines how sharp are the highlights in the material.
   * The bigger the value the sharper giving a more glossy feeling to the
   * result. Reversely, the smaller the value the blurrier giving a more rough
   * feeling to the result.
   */
  float specularPower;

  /**
   * Does the transparency come from the diffuse texture alpha channel.
   */
  Property<StandardMaterial, bool> useAlphaFromDiffuseTexture;

  /**
   * If true, the emissive value is added into the end result, otherwise it is
   * multiplied in.
   */
  Property<StandardMaterial, bool> useEmissiveAsIllumination;

  /**
   * If true, some kind of energy conservation will prevent the end result to be
   * more than 1 by reducing the emissive level when the final color is close to
   * one.
   */
  Property<StandardMaterial, bool> linkEmissiveWithDiffuse;

  /**
   * Specifies that the material will keep the specular highlights over a
   * transparent surface (only the most limunous ones). A car glass is a good
   * exemple of that. When sun reflects on it you can not see what is behind.
   */
  Property<StandardMaterial, bool> useSpecularOverAlpha;

  /**
   * Specifies that the material will keeps the reflection highlights over a
   * transparent surface (only the most limunous ones). A car glass is a good
   * exemple of that. When the street lights reflects on it you can not see what
   * is behind.
   */
  Property<StandardMaterial, bool> useReflectionOverAlpha;

  /**
   * Does lights from the scene impacts this material.
   * It can be a nice trick for performance to disable lighting on a fully
   * emissive material.
   */
  Property<StandardMaterial, bool> disableLighting;

  /**
   * Allows using an object space normal map (instead of tangent space).
   */
  Property<StandardMaterial, bool> useObjectSpaceNormalMap;

  /**
   * Is parallax enabled or not.
   * @see http://doc.babylonjs.com/how_to/using_parallax_mapping
   */
  Property<StandardMaterial, bool> useParallax;

  /**
   * Is parallax occlusion enabled or not.
   * If true, the outcome is way more realistic than traditional Parallax but
   * you can expect a performance hit that worthes consideration.
   * @see http://doc.babylonjs.com/how_to/using_parallax_mapping
   */
  Property<StandardMaterial, bool> useParallaxOcclusion;

  /**
   * Apply a scaling factor that determine which "depth" the height map should
   * reprensent. A value between 0.05 and 0.1 is reasonnable in Parallax, you
   * can reach 0.2 using Parallax Occlusion.
   */
  float parallaxScaleBias;

  /**
   * Helps to define how blurry the reflections should appears in the material.
   */
  Property<StandardMaterial, float> roughness;

  /**
   * In case of refraction, define the value of the index of refraction.
   * @see
   * http://doc.babylonjs.com/how_to/reflect#how-to-obtain-reflections-and-refractions
   */
  float indexOfRefraction;

  /**
   * Invert the refraction texture alongside the y axis.
   * It can be useful with procedural textures or probe for instance.
   * @see
   * http://doc.babylonjs.com/how_to/reflect#how-to-obtain-reflections-and-refractions
   */
  bool invertRefractionY;

  /**
   * Defines the alpha limits in alpha test mode
   */
  float alphaCutOff;

  /**
   * In case of light mapping, define whether the map contains light or shadow
   * informations.
   */
  Property<StandardMaterial, bool> useLightmapAsShadowmap;

  // Fresnel

  /**
   * Define the diffuse fresnel parameters of the material.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, FresnelParametersPtr> diffuseFresnelParameters;

  /**
   * Define the opacity fresnel parameters of the material.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, FresnelParametersPtr> opacityFresnelParameters;

  /**
   * Define the reflection fresnel parameters of the material.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, FresnelParametersPtr> reflectionFresnelParameters;

  /**
   * Define the refraction fresnel parameters of the material.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, FresnelParametersPtr> refractionFresnelParameters;

  /**
   * Define the emissive fresnel parameters of the material.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, FresnelParametersPtr> emissiveFresnelParameters;

  /**
   * If true automatically deducts the fresnels values from the material
   * specularity.
   * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
   */
  Property<StandardMaterial, bool> useReflectionFresnelFromSpecular;

  /**
   * Defines if the glossiness/roughness of the material should be read from the
   * specular map alpha channel
   */
  Property<StandardMaterial, bool> useGlossinessFromSpecularMapAlpha;

  /**
   * Defines the maximum number of lights that can be used in the material
   */
  Property<StandardMaterial, unsigned int> maxSimultaneousLights;

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  Property<StandardMaterial, bool> invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  Property<StandardMaterial, bool> invertNormalMapY;

  /**
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  Property<StandardMaterial, bool> twoSidedLighting;

  /**
   * The image processing configuration used either in this material
   */
  Property<StandardMaterial, ImageProcessingConfiguration*> imageProcessingConfiguration;

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
  Property<StandardMaterial, std::shared_ptr<ColorCurves>> cameraColorCurves;

protected:
  std::vector<RenderTargetTexturePtr> _renderTargets;
  Matrix _worldViewProjectionMatrix;
  Color3 _globalAmbientColor;
  bool _useLogarithmicDepth;
  bool _rebuildInParallel;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  ImageProcessingConfiguration* _imageProcessingConfiguration;

private:
  OnCreatedEffectParameters onCreatedEffectParameters;
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
  FresnelParametersPtr _diffuseFresnelParameters;
  FresnelParametersPtr _opacityFresnelParameters;
  FresnelParametersPtr _reflectionFresnelParameters;
  FresnelParametersPtr _refractionFresnelParameters;
  FresnelParametersPtr _emissiveFresnelParameters;
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
