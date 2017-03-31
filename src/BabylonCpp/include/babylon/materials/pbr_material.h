#ifndef BABYLON_MATERIALS_PBR_MATERIAL_H
#define BABYLON_MATERIALS_PBR_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

/**
 * @brief The Physically based material of BJS.
 *
 * This offers the main features of a standard PBR material.
 * For more information, please refer to the documentation :
 * http://doc.babylonjs.com/extensions/Physically_Based_Rendering
 */
class BABYLON_SHARED_EXPORT PBRMaterial : public Material {

public:
  using PMD = PBRMaterialDefines;

  template <typename... Ts>
  static PBRMaterial* New(Ts&&... args)
  {
    auto material = new PBRMaterial(std::forward<Ts>(args)...);
    material->addMaterialToScene(
      static_cast<std::unique_ptr<Material>>(material));

    return material;
  }
  ~PBRMaterial();

  /**
   * @brief Returns the string "PBRMaterial".
   */
  const char* getClassName() const;

  IReflect::Type type() const override;

  bool useLogarithmicDepth() const;
  void setUseLogarithmicDepth(bool value);
  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  void convertColorToLinearSpaceToRef(Color3& color, Color3& ref);

  static void ConvertColorToLinearSpaceToRef(Color3& color, Color3& ref,
                                             bool useScalarInLinear);
  static void BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect,
                         MaterialDefines& defines, bool useScalarInLinearSpace,
                         unsigned int maxSimultaneousLights,
                         bool usePhysicalLightFalloff);

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

protected:
  /**
   * Instantiates a new PBRMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRMaterial(const std::string& name, Scene* scene);

private:
  bool _shouldUseAlphaFromAlbedoTexture() const;
  bool _checkCache(Scene* scene, AbstractMesh* mesh = nullptr,
                   bool useInstances = false);

public:
  /**
   * Intensity of the direct lights e.g. the four lights available in your
   * scene.
   * This impacts both the direct diffuse and specular highlights.
   */
  float directIntensity;

  /**
   * Intensity of the emissive part of the material.
   * This helps controlling the emissive effect without modifying the emissive
   * color.
   */
  float emissiveIntensity;

  /**
   * Intensity of the environment e.g. how much the environment will light the
   * object
   * either through harmonics for rough material or through the refelction for
   * shiny ones.
   */
  float environmentIntensity;

  /**
   * This is a special control allowing the reduction of the specular highlights
   * coming from the
   * four lights of the scene. Those highlights may not be needed in full
   * environment lighting.
   */
  float specularIntensity;

  /**
   * Debug Control allowing disabling the bump map on this material.
   */
  bool disableBumpMap;

  /**
   * Debug Control helping enforcing or dropping the darkness of shadows.
   * 1.0 means the shadows have their normal darkness, 0.0 means the shadows are
   * not visible.
   */
  float overloadedShadowIntensity;

  /**
   * Debug Control helping dropping the shading effect coming from the diffuse
   * lighting.
   * 1.0 means the shade have their normal impact, 0.0 means no shading at all.
   */
  float overloadedShadeIntensity;

  /**
   * The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process.
   * This corresponds to a photographic exposure.
   */
  float cameraExposure;

  /**
   * The camera contrast used on this material.
   * This property is here and not in the camera to allow controlling contrast
   * without full screen post process.
   */
  float cameraContrast;

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

  /**
   * Debug Control allowing to overload the ambient color.
   * This as to be use with the overloadedAmbientIntensity parameter.
   */
  Color3 overloadedAmbient;

  /**
   * Debug Control indicating how much the overloaded ambient color is used
   * against the default one.
   */
  float overloadedAmbientIntensity;

  /**
   * Debug Control allowing to overload the albedo color.
   * This as to be use with the overloadedAlbedoIntensity parameter.
   */
  Color3 overloadedAlbedo;

  /**
   * Debug Control indicating how much the overloaded albedo color is used
   * against the default one.
   */
  float overloadedAlbedoIntensity;

  /**
   * Debug Control allowing to overload the reflectivity color.
   * This as to be use with the overloadedReflectivityIntensity parameter.
   */
  Color3 overloadedReflectivity;

  /**
   * Debug Control indicating how much the overloaded reflectivity color is used
   * against the default one.
   */
  float overloadedReflectivityIntensity;

  /**
   * Debug Control allowing to overload the emissive color.
   * This as to be use with the overloadedEmissiveIntensity parameter.
   */
  Color3 overloadedEmissive;

  /**
   * Debug Control indicating how much the overloaded emissive color is used
   * against the default one.
   */
  float overloadedEmissiveIntensity;

  /**
   * Debug Control allowing to overload the reflection color.
   * This as to be use with the overloadedReflectionIntensity parameter.
   */
  Color3 overloadedReflection;

  /**
   * Debug Control indicating how much the overloaded reflection color is used
   * against the default one.
   */
  float overloadedReflectionIntensity;

  /**
   * Debug Control allowing to overload the microsurface.
   * This as to be use with the overloadedMicroSurfaceIntensity parameter.
   */
  float overloadedMicroSurface;

  /**
   * Debug Control indicating how much the overloaded microsurface is used
   * against the default one.
   */
  float overloadedMicroSurfaceIntensity;

  /**
   * AKA Diffuse Texture in standard nomenclature.
   */
  BaseTexture* albedoTexture;

  /**
   * AKA Occlusion Texture in other nomenclature.
   */
  BaseTexture* ambientTexture;

  /**
   * AKA Occlusion Texture Intensity in other nomenclature.
   */
  float ambientTextureStrength;

  BaseTexture* opacityTexture;
  RenderTargetTexture* reflectionTexture;
  BaseTexture* emissiveTexture;

  /**
   * AKA Specular texture in other nomenclature.
   */
  BaseTexture* reflectivityTexture;

  /**
   * Used to switch from specular/glossiness to metallic/roughness workflow.
   */
  BaseTexture* metallicTexture;

  /**
   * Specifies the metallic scalar of the metallic/roughness workflow.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  float metallic;

  /**
   * Specifies the roughness scalar of the metallic/roughness workflow.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  float roughness;

  BaseTexture* bumpTexture;
  BaseTexture* lightmapTexture;
  RenderTargetTexture* refractionTexture;
  Color3 ambientColor;

  /**
   * AKA Diffuse Color in other nomenclature.
   */
  Color3 albedoColor;

  /**
   * AKA Specular Color in other nomenclature.
   */
  Color3 reflectivityColor;

  Color3 reflectionColor;
  Color3 emissiveColor;

  /**
   * AKA Glossiness in other nomenclature.
   */
  float microSurface;

  /**
   * source material index of refraction (IOR)' / 'destination material IOR.
   */
  float indexOfRefraction;

  /**
   * Controls if refraction needs to be inverted on Y. This could be usefull for
   * procedural texture.
   */
  bool invertRefractionY;

  FresnelParameters* opacityFresnelParameters;
  FresnelParameters* emissiveFresnelParameters;

  /**
   * This parameters will make the material used its opacity to control how much
   * it is refracting aginst not.
   * Materials half opaque for instance using refraction could benefit from this
   * control.
   */
  bool linkRefractionWithTransparency;

  /**
   * The emissive and albedo are linked to never be more than one (Energy
   * conservation).
   */
  bool linkEmissiveWithAlbedo;

  bool useLightmapAsShadowmap;

  /**
   * In this mode, the emissive informtaion will always be added to the lighting
   * once.
   * A light for instance can be thought as emissive.
   */
  bool useEmissiveAsIllumination;

  /**
   * Secifies that the alpha is coming form the albedo channel alpha channel.
   */
  bool useAlphaFromAlbedoTexture;

  /**
   * Specifies that the material will keeps the specular highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When sun reflects on it you can not
   * see what is behind.
   */
  bool useSpecularOverAlpha;

  /**
   * Specifies if the reflectivity texture contains the glossiness information
   * in its alpha channel.
   */
  bool useMicroSurfaceFromReflectivityMapAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * alpha channel.
   */
  bool useRoughnessFromMetallicTextureAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * green channel.
   */
  bool useRoughnessFromMetallicTextureGreen;

  /**
   * In case the reflectivity map does not contain the microsurface information
   * in its alpha channel,
   * The material will try to infer what glossiness each pixel should be.
   */
  bool useAutoMicroSurfaceFromReflectivityMap;

  /**
   * Allows to work with scalar in linear mode. This is definitely a matter of
   * preferences and tools used during
   * the creation of the material.
   */
  bool useScalarInLinearSpace;

  /**
   * BJS is using an harcoded light falloff based on a manually sets up range.
   * In PBR, one way to represents the fallof is to use the inverse squared root
   * algorythm.
   * This parameter can help you switch back to the BJS mode in order to create
   * scenes using both materials.
   */
  bool usePhysicalLightFalloff;

  /**
   * Specifies that the material will keeps the reflection highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When the street lights reflects on
   * it you can not see what is behind.
   */
  bool useRadianceOverAlpha;

  /**
   * Allows using the bump map in parallax mode.
   */
  bool useParallax;

  /**
   * Allows using the bump map in parallax occlusion mode.
   */
  bool useParallaxOcclusion;

  /**
   * Controls the scale bias of the parallax mode.
   */
  float parallaxScaleBias;

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  bool disableLighting;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
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
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  bool twoSidedLighting;

private:
  Vector4 _lightingInfos;
  Vector4 _overloadedShadowInfos;
  Vector4 _cameraInfos;
  Vector2 _microsurfaceTextureLods;
  Vector4 _overloadedIntensity;
  Vector3 _overloadedMicroSurface;
  std::vector<RenderTargetTexture*> _renderTargets;
  Matrix _worldViewProjectionMatrix;
  Color3 _globalAmbientColor;
  Color3 _tempColor;
  int _renderId;
  PBRMaterialDefines _defines;
  std::unique_ptr<PBRMaterialDefines> _cachedDefines;
  bool _useLogarithmicDepth;
  static Color3 _scaledAlbedo;
  static Color3 _scaledReflectivity;
  static Color3 _scaledEmissive;
  static Color3 _scaledReflection;
  Scene* _myScene;
  ShadowGenerator* _myShadowGenerator;

}; // end of class PBRMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_MATERIAL_H
