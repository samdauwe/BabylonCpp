#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/pbr/pbr_base_material.h>

namespace BABYLON {

class ColorCurves;
class PBRMaterial;
using ColorCurvesPtr = std::shared_ptr<ColorCurves>;
using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;

/**
 * @brief The Physically based material of BJS.
 *
 * This offers the main features of a standard PBR material.
 * For more information, please refer to the documentation :
 * https://doc.babylonjs.com/how_to/physically_based_rendering
 */
class BABYLON_SHARED_EXPORT PBRMaterial : public PBRBaseMaterial {

public:
  /**
   * PBRMaterialTransparencyMode: No transparency mode, Alpha channel is not
   * use.
   */
  static constexpr unsigned int PBRMATERIAL_OPAQUE
    = PBRBaseMaterial::PBRMATERIAL_OPAQUE;

  /**
   * PBRMaterialTransparencyMode: Alpha Test mode, pixel are discarded below a
   * certain threshold defined by the alpha cutoff value.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHATEST
    = PBRBaseMaterial::PBRMATERIAL_ALPHATEST;

  /**
   * PBRMaterialTransparencyMode: Pixels are blended (according to the alpha
   * mode) with the already drawn pixels in the current frame buffer.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHABLEND
    = PBRBaseMaterial::PBRMATERIAL_ALPHABLEND;

  /**
   * PBRMaterialTransparencyMode: Pixels are blended (according to the alpha
   * mode) with the already drawn pixels in the current frame buffer. They are
   * also discarded below the alpha cutoff threshold to improve performances.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHATESTANDBLEND
    = PBRBaseMaterial::PBRMATERIAL_ALPHATESTANDBLEND;

  /**
   * Defines the default value of how much AO map is occluding the analytical
   * lights (point spot...).
   */
  static constexpr unsigned int DEFAULT_AO_ON_ANALYTICAL_LIGHTS
    = PBRBaseMaterial::DEFAULT_AO_ON_ANALYTICAL_LIGHTS;

public:
  template <typename... Ts>
  static PBRMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<PBRMaterial>(
      new PBRMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~PBRMaterial() override;

  /**
   * @brief Returns the name of this material class.
   */
  const std::string getClassName() const override;

  /**
   * @brief Makes a duplicate of the current material.
   * @param name - name to use for the new material.
   */
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;

  /**
   * @brief Serializes this PBR Material.
   * @returns - An object with the serialized material.
   */
  json serialize() const;

  // Statics
  /**
   * @brief Parses a PBR Material from a serialized object.
   * @param source - Serialized object.
   * @param scene - BJS scene instance.
   * @param rootUrl - url for the scene object
   * @returns - PBRMaterial
   */
  static PBRMaterial* Parse(const json& source, Scene* scene,
                            const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new PBRMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRMaterial(const std::string& name, Scene* scene);

  /**
   * Intensity of the direct lights e.g. the four lights available in your
   * scene.
   * This impacts both the direct diffuse and specular highlights.
   */
  float get_directIntensity() const;
  void set_directIntensity(float value);

  /**
   * Intensity of the emissive part of the material.
   * This helps controlling the emissive effect without modifying the emissive
   * color.
   */
  float get_emissiveIntensity() const;
  void set_emissiveIntensity(float value);

  /**
   * Intensity of the environment e.g. how much the environment will light the
   * object
   * either through harmonics for rough material or through the refelction for
   * shiny ones.
   */
  float get_environmentIntensity() const;
  void set_environmentIntensity(float value);

  /**
   * This is a special control allowing the reduction of the specular highlights
   * coming from the
   * four lights of the scene. Those highlights may not be needed in full
   * environment lighting.
   */
  float get_specularIntensity() const;
  void set_specularIntensity(float value);

  /**
   * Debug Control allowing disabling the bump map on this material.
   */
  bool get_disableBumpMap() const;
  void set_disableBumpMap(bool value);

  /**
   * AKA Diffuse Texture in standard nomenclature.
   */
  BaseTexturePtr& get_albedoTexture();
  void set_albedoTexture(const BaseTexturePtr& value);

  /**
   * AKA Occlusion Texture in other nomenclature.
   */
  BaseTexturePtr& get_ambientTexture();
  void set_ambientTexture(const BaseTexturePtr& value);

  /**
   * AKA Occlusion Texture Intensity in other nomenclature.
   */
  float get_ambientTextureStrength() const;
  void set_ambientTextureStrength(float value);

  /**
   * Defines how much the AO map is occluding the analytical lights (point
   * spot...). 1 means it completely occludes it 0 mean it has no impact
   */
  unsigned int get_ambientTextureImpactOnAnalyticalLights() const;
  void set_ambientTextureImpactOnAnalyticalLights(unsigned int value);

  /**
   * Stores the alpha values in a texture.
   */
  BaseTexturePtr& get_opacityTexture();
  void set_opacityTexture(const BaseTexturePtr& value);

  /**
   * Stores the reflection values in a texture.
   */
  BaseTexturePtr& get_reflectionTexture();
  void set_reflectionTexture(const BaseTexturePtr& value);

  /**
   * Stores the emissive values in a texture.
   */
  BaseTexturePtr& get_emissiveTexture();
  void set_emissiveTexture(const BaseTexturePtr& value);

  /**
   * AKA Specular texture in other nomenclature.
   */
  BaseTexturePtr& get_reflectivityTexture();
  void set_reflectivityTexture(const BaseTexturePtr& value);

  /**
   * Used to switch from specular/glossiness to metallic/roughness workflow.
   */
  BaseTexturePtr& get_metallicTexture();
  void set_metallicTexture(const BaseTexturePtr& value);

  /**
   * Specifies the metallic scalar of the metallic/roughness workflow.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  std::optional<float>& get_metallic();
  void set_metallic(const std::optional<float>& value);

  /**
   * Specifies the roughness scalar of the metallic/roughness workflow.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  std::optional<float>& get_roughness();
  void set_roughness(const std::optional<float>& value);

  /**
   * Used to enable roughness/glossiness fetch from a separate channel depending
   * on the current mode. Gray Scale represents roughness in metallic mode and
   * glossiness in specular mode.
   */
  BaseTexturePtr& get_microSurfaceTexture();
  void set_microSurfaceTexture(const BaseTexturePtr& value);

  /**
   * Stores surface normal data used to displace a mesh in a texture.
   */
  BaseTexturePtr& get_bumpTexture();
  void set_bumpTexture(const BaseTexturePtr& value);

  /**
   * Stores the pre-calculated light information of a mesh in a texture.
   */
  BaseTexturePtr& get_lightmapTexture();
  void set_lightmapTexture(const BaseTexturePtr& value);

  /**
   * Stores the refracted light information in a texture.
   */
  BaseTexturePtr& get_refractionTexture();
  void set_refractionTexture(const BaseTexturePtr& value);

  /**
   * The color of a material in ambient lighting.
   */
  Color3& get_ambientColor();
  void set_ambientColor(const Color3& value);

  /**
   * AKA Diffuse Color in other nomenclature.
   */
  Color3& get_albedoColor();
  void set_albedoColor(const Color3& value);

  /**
   * AKA Specular Color in other nomenclature.
   */
  Color3& get_reflectivityColor();
  void set_reflectivityColor(const Color3& value);

  /**
   * The color reflected from the material.
   */
  Color3& get_reflectionColor();
  void set_reflectionColor(const Color3& value);

  /**
   * The color emitted from the material.
   */
  Color3& get_emissiveColor();
  void set_emissiveColor(const Color3& value);

  /**
   * AKA Glossiness in other nomenclature.
   */
  float get_microSurface() const;
  void set_microSurface(float value);

  /**
   * source material index of refraction (IOR)' / 'destination material IOR.
   */
  float get_indexOfRefraction() const;
  void set_indexOfRefraction(float value);

  /**
   * Controls if refraction needs to be inverted on Y. This could be useful for
   * procedural texture.
   */
  bool get_invertRefractionY() const;
  void set_invertRefractionY(bool value);

  /**
   * This parameters will make the material used its opacity to control how much
   * it is refracting aginst not.
   * Materials half opaque for instance using refraction could benefit from this
   * control.
   */
  bool get_linkRefractionWithTransparency() const;
  void set_linkRefractionWithTransparency(bool value);

  /**
   * If true, the light map contains occlusion information instead of lighting
   * info.
   */
  bool get_useLightmapAsShadowmap() const;
  void set_useLightmapAsShadowmap(bool value);

  /**
   * Specifies that the alpha is coming form the albedo channel alpha channel
   * for alpha blending.
   */
  bool get_useAlphaFromAlbedoTexture() const;
  void set_useAlphaFromAlbedoTexture(bool value);

  /**
   * Enforces alpha test in opaque or blend mode in order to improve the
   * performances of some situations.
   */
  bool get_forceAlphaTest() const;
  void set_forceAlphaTest(bool value);

  /**
   * Defines the alpha limits in alpha test mode.
   */
  float get_alphaCutOff() const;
  void set_alphaCutOff(float value);

  /**
   * Specifies that the material will keep the specular highlights over a
   * transparent surface (only the most limunous ones). A car glass is a good
   * exemple of that. When sun reflects on it you can not see what is behind.
   */
  bool get_useSpecularOverAlpha() const;
  void set_useSpecularOverAlpha(bool value);

  /**
   * Specifies if the reflectivity texture contains the glossiness information
   * in its alpha channel.
   */
  bool get_useMicroSurfaceFromReflectivityMapAlpha() const;
  void set_useMicroSurfaceFromReflectivityMapAlpha(bool value);

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * alpha channel.
   */
  bool get_useRoughnessFromMetallicTextureAlpha() const;
  void set_useRoughnessFromMetallicTextureAlpha(bool value);

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * green channel.
   */
  bool get_useRoughnessFromMetallicTextureGreen() const;
  void set_useRoughnessFromMetallicTextureGreen(bool value);

  /**
   * Specifies if the metallic texture contains the metallness information in
   * its blue channel.
   */
  bool get_useMetallnessFromMetallicTextureBlue() const;
  void set_useMetallnessFromMetallicTextureBlue(bool value);

  /**
   * Specifies if the metallic texture contains the ambient occlusion
   * information in its red channel.
   */
  bool get_useAmbientOcclusionFromMetallicTextureRed() const;
  void set_useAmbientOcclusionFromMetallicTextureRed(bool value);

  /**
   * Specifies if the ambient texture contains the ambient occlusion information
   * in its red channel only.
   */
  bool get_useAmbientInGrayScale() const;
  void set_useAmbientInGrayScale(bool value);

  /**
   * In case the reflectivity map does not contain the microsurface information
   * in its alpha channel,
   * The material will try to infer what glossiness each pixel should be.
   */
  bool get_useAutoMicroSurfaceFromReflectivityMap() const;
  void set_useAutoMicroSurfaceFromReflectivityMap(bool value);

  /**
   * @brief BJS is using an harcoded light falloff based on a manually sets up
   * range. In PBR, one way to represents the fallof is to use the inverse
   * squared root algorythm. This parameter can help you switch back to the BJS
   * mode in order to create scenes using both materials.
   */
  bool get_usePhysicalLightFalloff() const;

  /**
   * @brief BJS is using an harcoded light falloff based on a manually sets up
   * range. In PBR, one way to represents the fallof is to use the inverse
   * squared root algorythm. This parameter can help you switch back to the BJS
   * mode in order to create scenes using both materials.
   */
  void set_usePhysicalLightFalloff(bool value);

  /**
   * @brief In order to support the falloff compatibility with gltf, a special
   * mode has been added to reproduce the gltf light falloff.
   */
  bool get_useGLTFLightFalloff() const;

  /**
   * @brief In order to support the falloff compatibility with gltf, a special
   * mode has been added to reproduce the gltf light falloff.
   */
  void set_useGLTFLightFalloff(bool value);

  /**
   * Specifies that the material will keeps the reflection highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When the street lights reflects on
   * it you can not see what is behind.
   */
  bool get_useRadianceOverAlpha() const;
  void set_useRadianceOverAlpha(bool value);

  /**
   * Allows using an object space normal map (instead of tangent space).
   */
  bool get_useObjectSpaceNormalMap() const;
  void set_useObjectSpaceNormalMap(bool value);

  /**
   * Allows using the bump map in parallax mode.
   */
  bool get_useParallax() const;
  void set_useParallax(bool value);

  /**
   * Allows using the bump map in parallax occlusion mode.
   */
  bool get_useParallaxOcclusion() const;
  void set_useParallaxOcclusion(bool value);

  /**
   * Controls the scale bias of the parallax mode.
   */
  float get_parallaxScaleBias() const;
  void set_parallaxScaleBias(float value);

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  bool get_disableLighting() const;
  void set_disableLighting(bool value);

  /**
   * Force the shader to compute irradiance in the fragment shader in order to
   * take bump in account.
   */
  bool get_forceIrradianceInFragment() const;
  void set_forceIrradianceInFragment(bool value);

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  bool get_invertNormalMapX() const;
  void set_invertNormalMapX(bool value);

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  bool get_invertNormalMapY() const;
  void set_invertNormalMapY(bool value);

  /**
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  bool get_twoSidedLighting() const;
  void set_twoSidedLighting(bool value);

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested.
   * And/Or occlude the blended part. (alpha is converted to gamma to compute
   * the fresnel)
   */
  bool get_useAlphaFresnel() const;
  void set_useAlphaFresnel(bool value);

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested.
   * And/Or occlude the blended part. (alpha stays linear to compute the
   * fresnel)
   */
  bool get_useLinearAlphaFresnel() const;
  void set_useLinearAlphaFresnel(bool value);

  /**
   * Let user defines the brdf lookup texture used for IBL.
   * A default 8bit version is embedded but you could point at :
   * * Default texture:
   * https://assets.babylonjs.com/environments/correlatedMSBRDF_RGBD.png
   * * Default 16bit pixel depth texture:
   * https://assets.babylonjs.com/environments/correlatedMSBRDF.dds
   * * LEGACY Default None correlated
   * https://assets.babylonjs.com/environments/uncorrelatedBRDF_RGBD.png
   * * LEGACY Default None correlated 16bit pixel depth
   * https://assets.babylonjs.com/environments/uncorrelatedBRDF.dds
   */
  BaseTexturePtr& get_environmentBRDFTexture();
  void set_environmentBRDFTexture(const BaseTexturePtr& value);

  /**
   * Force normal to face away from face.
   */
  bool get_forceNormalForward() const;
  void set_forceNormalForward(bool value);

  /**
   * Enables specular anti aliasing in the PBR shader.
   * It will both interacts on the Geometry for analytical and IBL lighting.
   * It also prefilter the roughness map based on the bump values.
   */
  bool get_enableSpecularAntiAliasing() const;
  void set_enableSpecularAntiAliasing(bool value);

  /**
   * This parameters will enable/disable Horizon occlusion to prevent normal
   * maps to look shiny when the normal
   * makes the reflect vector face the model (under horizon).
   */
  bool get_useHorizonOcclusion() const;
  void set_useHorizonOcclusion(bool value);

  /**
   * This parameters will enable/disable radiance occlusion by preventing the
   * radiance to lit
   * too much the area relying on ambient texture to define their ambient
   * occlusion.
   */
  bool get_useRadianceOcclusion() const;
  void set_useRadianceOcclusion(bool value);

  /**
   * If set to true, no lighting calculations will be applied.
   */
  bool get_unlit() const;
  void set_unlit(bool value);

  /**
   * @brief Gets the image processing configuration used either in this
   * material.
   */
  ImageProcessingConfigurationPtr& get_imageProcessingConfiguration();

  /**
   * @brief Sets the Default image processing configuration used either in the
   * this material.
   *
   * If sets to null, the scene one is in use.
   */
  void set_imageProcessingConfiguration(
    const ImageProcessingConfigurationPtr& value);

  /**
   * @brief Gets wether the color curves effect is enabled.
   */
  bool get_cameraColorCurvesEnabled() const;

  /**
   * @brief Sets wether the color curves effect is enabled.
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
   * Sets wether tonemapping is enabled or not
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
  ColorCurvesPtr& get_cameraColorCurves();

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
  void set_cameraColorCurves(const ColorCurvesPtr& value);

public:
  /**
   * Intensity of the direct lights e.g. the four lights available in your
   * scene.
   * This impacts both the direct diffuse and specular highlights.
   */
  Property<PBRMaterial, float> directIntensity;

  /**
   * Intensity of the emissive part of the material.
   * This helps controlling the emissive effect without modifying the emissive
   * color.
   */
  Property<PBRMaterial, float> emissiveIntensity;

  /**
   * Intensity of the environment e.g. how much the environment will light the
   * object
   * either through harmonics for rough material or through the refelction for
   * shiny ones.
   */
  Property<PBRMaterial, float> environmentIntensity;

  /**
   * This is a special control allowing the reduction of the specular highlights
   * coming from the
   * four lights of the scene. Those highlights may not be needed in full
   * environment lighting.
   */
  Property<PBRMaterial, float> specularIntensity;

  /**
   * Debug Control allowing disabling the bump map on this material.
   */
  Property<PBRMaterial, bool> disableBumpMap;

  /**
   * AKA Diffuse Texture in standard nomenclature.
   */
  Property<PBRMaterial, BaseTexturePtr> albedoTexture;

  /**
   * AKA Occlusion Texture in other nomenclature.
   */
  Property<PBRMaterial, BaseTexturePtr> ambientTexture;

  /**
   * AKA Occlusion Texture Intensity in other nomenclature.
   */
  Property<PBRMaterial, float> ambientTextureStrength;

  /**
   * Defines how much the AO map is occluding the analytical lights (point
   * spot...). 1 means it completely occludes it 0 mean it has no impact
   */
  Property<PBRMaterial, unsigned int> ambientTextureImpactOnAnalyticalLights;

  /**
   * Stores the alpha values in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> opacityTexture;

  /**
   * Stores the reflection values in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> reflectionTexture;

  /**
   * Stores the emissive values in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> emissiveTexture;

  /**
   * AKA Specular texture in other nomenclature.
   */
  Property<PBRMaterial, BaseTexturePtr> reflectivityTexture;

  /**
   * Used to switch from specular/glossiness to metallic/roughness workflow.
   */
  Property<PBRMaterial, BaseTexturePtr> metallicTexture;

  /**
   * Specifies the metallic scalar of the metallic/roughness workflow.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  Property<PBRMaterial, std::optional<float>> metallic;

  /**
   * Specifies the roughness scalar of the metallic/roughness workflow.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  Property<PBRMaterial, std::optional<float>> roughness;

  /**
   * Used to enable roughness/glossiness fetch from a separate channel depending
   * on the current mode. Gray Scale represents roughness in metallic mode and
   * glossiness in specular mode.
   */
  Property<PBRMaterial, BaseTexturePtr> microSurfaceTexture;

  /**
   * Stores surface normal data used to displace a mesh in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> bumpTexture;

  /**
   * Stores the pre-calculated light information of a mesh in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> lightmapTexture;

  /**
   * Stores the refracted light information in a texture.
   */
  Property<PBRMaterial, BaseTexturePtr> refractionTexture;

  /**
   * The color of a material in ambient lighting.
   */
  Property<PBRMaterial, Color3> ambientColor;

  /**
   * AKA Diffuse Color in other nomenclature.
   */
  Property<PBRMaterial, Color3> albedoColor;

  /**
   * AKA Specular Color in other nomenclature.
   */
  Property<PBRMaterial, Color3> reflectivityColor;

  /**
   * The color reflected from the material.
   */
  Property<PBRMaterial, Color3> reflectionColor;

  /**
   * The color emitted from the material.
   */
  Property<PBRMaterial, Color3> emissiveColor;

  /**
   * AKA Glossiness in other nomenclature.
   */
  Property<PBRMaterial, float> microSurface;

  /**
   * source material index of refraction (IOR)' / 'destination material IOR.
   */
  Property<PBRMaterial, float> indexOfRefraction;

  /**
   * Controls if refraction needs to be inverted on Y. This could be useful for
   * procedural texture.
   */
  Property<PBRMaterial, bool> invertRefractionY;

  /**
   * This parameters will make the material used its opacity to control how much
   * it is refracting aginst not.
   * Materials half opaque for instance using refraction could benefit from this
   * control.
   */
  Property<PBRMaterial, bool> linkRefractionWithTransparency;

  /**
   * If true, the light map contains occlusion information instead of lighting
   * info.
   */
  Property<PBRMaterial, bool> useLightmapAsShadowmap;

  /**
   * Specifies that the alpha is coming form the albedo channel alpha channel
   * for alpha blending.
   */
  Property<PBRMaterial, bool> useAlphaFromAlbedoTexture;

  /**
   * Enforces alpha test in opaque or blend mode in order to improve the
   * performances of some situations.
   */
  Property<PBRMaterial, bool> forceAlphaTest;

  /**
   * Defines the alpha limits in alpha test mode.
   */
  Property<PBRMaterial, float> alphaCutOff;

  /**
   * Specifies that the material will keep the specular highlights over a
   * transparent surface (only the most limunous ones). A car glass is a good
   * exemple of that. When sun reflects on it you can not see what is behind.
   */
  Property<PBRMaterial, bool> useSpecularOverAlpha;

  /**
   * Specifies if the reflectivity texture contains the glossiness information
   * in its alpha channel.
   */
  Property<PBRMaterial, bool> useMicroSurfaceFromReflectivityMapAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * alpha channel.
   */
  Property<PBRMaterial, bool> useRoughnessFromMetallicTextureAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * green channel.
   */
  Property<PBRMaterial, bool> useRoughnessFromMetallicTextureGreen;

  /**
   * Specifies if the metallic texture contains the metallness information in
   * its blue channel.
   */
  Property<PBRMaterial, bool> useMetallnessFromMetallicTextureBlue;

  /**
   * Specifies if the metallic texture contains the ambient occlusion
   * information in its red channel.
   */
  Property<PBRMaterial, bool> useAmbientOcclusionFromMetallicTextureRed;

  /**
   * Specifies if the ambient texture contains the ambient occlusion information
   * in its red channel only.
   */
  Property<PBRMaterial, bool> useAmbientInGrayScale;

  /**
   * In case the reflectivity map does not contain the microsurface information
   * in its alpha channel,
   * The material will try to infer what glossiness each pixel should be.
   */
  Property<PBRMaterial, bool> useAutoMicroSurfaceFromReflectivityMap;

  /**
   * BJS is using an harcoded light falloff based on a manually sets up range.
   * In PBR, one way to represents the fallof is to use the inverse squared root
   * algorythm. This parameter can help you switch back to the BJS mode in order
   * to create scenes using both materials.
   */
  Property<PBRMaterial, bool> usePhysicalLightFalloff;

  /**
   * In order to support the falloff compatibility with gltf, a special mode has
   * been added to reproduce the gltf light falloff.
   */
  Property<PBRMaterial, bool> useGLTFLightFalloff;

  /**
   * Specifies that the material will keeps the reflection highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When the street lights reflects on
   * it you can not see what is behind.
   */
  Property<PBRMaterial, bool> useRadianceOverAlpha;

  /**
   * Allows using an object space normal map (instead of tangent space).
   */
  Property<PBRMaterial, bool> useObjectSpaceNormalMap;

  /**
   * Allows using the bump map in parallax mode.
   */
  Property<PBRMaterial, bool> useParallax;

  /**
   * Allows using the bump map in parallax occlusion mode.
   */
  Property<PBRMaterial, bool> useParallaxOcclusion;

  /**
   * Controls the scale bias of the parallax mode.
   */
  Property<PBRMaterial, float> parallaxScaleBias;

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  Property<PBRMaterial, bool> disableLighting;

  /**
   * Force the shader to compute irradiance in the fragment shader in order to
   * take bump in account.
   */
  Property<PBRMaterial, bool> forceIrradianceInFragment;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  Property<PBRMaterial, unsigned int> maxSimultaneousLights;

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  Property<PBRMaterial, bool> invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  Property<PBRMaterial, bool> invertNormalMapY;

  /**
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  Property<PBRMaterial, bool> twoSidedLighting;

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested.
   * And/Or occlude the blended part. (alpha is converted to gamma to compute
   * the fresnel)
   */
  Property<PBRMaterial, bool> useAlphaFresnel;

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested.
   * And/Or occlude the blended part. (alpha stays linear to compute the
   * fresnel)
   */
  Property<PBRMaterial, bool> useLinearAlphaFresnel;

  /**
   * Let user defines the brdf lookup texture used for IBL.
   * A default 8bit version is embedded but you could point at :
   * * Default texture:
   * https://assets.babylonjs.com/environments/correlatedMSBRDF_RGBD.png
   * * Default 16bit pixel depth texture:
   * https://assets.babylonjs.com/environments/correlatedMSBRDF.dds
   * * LEGACY Default None correlated
   * https://assets.babylonjs.com/environments/uncorrelatedBRDF_RGBD.png
   * * LEGACY Default None correlated 16bit pixel depth
   * https://assets.babylonjs.com/environments/uncorrelatedBRDF.dds
   */
  Property<PBRMaterial, BaseTexturePtr> environmentBRDFTexture;

  /**
   * Force normal to face away from face.
   */
  Property<PBRMaterial, bool> forceNormalForward;

  /**
   * Enables specular anti aliasing in the PBR shader.
   * It will both interacts on the Geometry for analytical and IBL lighting.
   * It also prefilter the roughness map based on the bump values.
   */
  Property<PBRMaterial, bool> enableSpecularAntiAliasing;

  /**
   * This parameters will enable/disable Horizon occlusion to prevent normal
   * maps to look shiny when the normal
   * makes the reflect vector face the model (under horizon).
   */
  Property<PBRMaterial, bool> useHorizonOcclusion;

  /**
   * This parameters will enable/disable radiance occlusion by preventing the
   * radiance to lit
   * too much the area relying on ambient texture to define their ambient
   * occlusion.
   */
  Property<PBRMaterial, bool> useRadianceOcclusion;

  /**
   * If set to true, no lighting calculations will be applied.
   */
  Property<PBRMaterial, bool> unlit;

  /**
   * The image processing configuration used either in this
   * material.
   */
  Property<PBRMaterial, ImageProcessingConfigurationPtr>
    imageProcessingConfiguration;

  /**
   * Wether the color curves effect is enabled.
   */
  Property<PBRMaterial, bool> cameraColorCurvesEnabled;

  /**
   * Wether the color grading effect is enabled.
   */
  Property<PBRMaterial, bool> cameraColorGradingEnabled;

  /**
   * Wether tonemapping is enabled or not.
   */
  Property<PBRMaterial, bool> cameraToneMappingEnabled;

  /**
   * The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  Property<PBRMaterial, float> cameraExposure;

  /**
   * The camera contrast used on this material.
   */
  Property<PBRMaterial, float> cameraContrast;

  /**
   * The Color Grading 2D Lookup Texture.
   */
  Property<PBRMaterial, BaseTexturePtr> cameraColorGradingTexture;

  /**
   * The color grading curves provide additional color adjustmnent that is
   * applied after any color grading transform (3D LUT). They allow basic
   * adjustment of saturation and small exposure adjustments, along with color
   * filter tinting to provide white balance adjustment or more stylistic
   * effects. These are similar to controls found in many professional imaging
   * or colorist software. The global controls are applied to the entire image.
   * For advanced tuning, extra controls are provided to adjust the shadow,
   * midtone and highlight areas of the image; corresponding to low luminance,
   * medium luminance, and high luminance areas respectively.
   */
  Property<PBRMaterial, ColorCurvesPtr> cameraColorCurves;

}; // end of class PBRMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_H
