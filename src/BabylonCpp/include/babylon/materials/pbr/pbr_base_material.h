#ifndef BABYLON_MATERIALS_PBR_PBR_BASE_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_BASE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/push_material.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class IAnimatable;
class ImageProcessingConfiguration;
class PBRAnisotropicConfiguration;
class PBRBRDFConfiguration;
class PBRClearCoatConfiguration;
struct PBRMaterialDefines;
class PBRSheenConfiguration;
class PBRSubSurfaceConfiguration;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;
using ImageProcessingConfigurationPtr
  = std::shared_ptr<ImageProcessingConfiguration>;
using PBRAnisotropicConfigurationPtr
  = std::shared_ptr<PBRAnisotropicConfiguration>;
using PBRBRDFConfigurationPtr      = std::shared_ptr<PBRBRDFConfiguration>;
using PBRClearCoatConfigurationPtr = std::shared_ptr<PBRClearCoatConfiguration>;
using PBRSheenConfigurationPtr     = std::shared_ptr<PBRSheenConfiguration>;
using PBRSubSurfaceConfigurationPtr
  = std::shared_ptr<PBRSubSurfaceConfiguration>;

/**
 * @brief The Physically based material base class of BJS.
 *
 * This offers the main features of a standard PBR material.
 * For more information, please refer to the documentation :
 * https://doc.babylonjs.com/how_to/physically_based_rendering
 */
class BABYLON_SHARED_EXPORT PBRBaseMaterial : public PushMaterial {

  friend struct PBRMaterialPropertyGridComponent;

public:
  /**
   * PBRMaterialTransparencyMode: No transparency mode, Alpha channel is not
   * use.
   */
  static constexpr unsigned int PBRMATERIAL_OPAQUE = 0;

  /**
   * PBRMaterialTransparencyMode: Alpha Test mode, pixel are discarded below a
   * certain threshold defined by the alpha cutoff value.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHATEST = 1;

  /**
   * PBRMaterialTransparencyMode: Pixels are blended (according to the alpha
   * mode) with the already drawn pixels in the current frame buffer.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHABLEND = 2;

  /**
   * PBRMaterialTransparencyMode: Pixels are blended (according to the alpha
   * mode) with the already drawn pixels in the current frame buffer. They are
   * also discarded below the alpha cutoff threshold to improve performances.
   */
  static constexpr unsigned int PBRMATERIAL_ALPHATESTANDBLEND = 3;

  /**
   * Defines the default value of how much AO map is occluding the analytical
   * lights (point spot...).
   */
  static constexpr unsigned int DEFAULT_AO_ON_ANALYTICAL_LIGHTS = 0;

  /**
   * PBRMaterialLightFalloff Physical: light is falling off following the
   * inverse squared distance law.
   */
  static constexpr unsigned int LIGHTFALLOFF_PHYSICAL = 0;

  /**
   * PBRMaterialLightFalloff gltf: light is falling off as described in the gltf
   * moving to PBR document to enhance interoperability with other engines.
   */
  static constexpr unsigned int LIGHTFALLOFF_GLTF = 1;

  /**
   * PBRMaterialLightFalloff Standard: light is falling off like in the standard
   * material to enhance interoperability with other materials.
   */
  static constexpr unsigned int LIGHTFALLOFF_STANDARD = 2;

public:
  /**
   * @brief Instantiates a new PBRMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRBaseMaterial(const std::string& name, Scene* scene);
  ~PBRBaseMaterial() override;

  /**
   * @brief Gets the name of the material class.
   */
  const std::string getClassName() const override;

  /**
   * @brief Returns true if alpha blending should be disabled.
   */
  bool _disableAlphaBlending() const;

  /**
   * @brief Specifies whether or not this material should be rendered in alpha
   * blend mode.
   */
  bool needAlphaBlending() const override;

  /**
   * @brief Specifies if the mesh will require alpha blending.
   * @param mesh - BJS mesh.
   */
  bool needAlphaBlendingForMesh(const AbstractMesh& mesh) const override;

  /**
   * @brief Specifies whether or not this material should be rendered in alpha
   * test mode.
   */
  bool needAlphaTesting() const override;

  /**
   * @brief Gets the texture used for the alpha test.
   */
  BaseTexturePtr getAlphaTestTexture() override;

  /**
   * @brief Specifies that the submesh is ready to be used.
   * @param mesh - BJS mesh.
   * @param subMesh - A submesh of the BJS mesh.  Used to check if it is ready.
   * @param useInstances - Specifies that instances should be used.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Specifies if the material uses metallic roughness workflow.
   * @returns boolean specifiying if the material uses metallic roughness
   * workflow.
   */
  bool isMetallicWorkflow() const;

  /**
   * @brief Force shader compilation
   */
  void forceCompilation(AbstractMesh* mesh,
                        std::function<void(Material* material)>& onCompiled,
                        bool clipPlane = false);

  /**
   * @brief Initializes the uniform buffer layout for the shader.
   */
  void buildUniformLayout();

  /**
   * @brief Unbinds the material from the mesh.
   */
  void unbind() override;

  /**
   * @brief Binds the submesh data.
   * @param world - The world matrix.
   * @param mesh - The BJS mesh.
   * @param subMesh - A submesh of the BJS mesh.
   */
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Returns the animatable textures.
   * @returns - Array of animatable textures.
   */
  std::vector<IAnimatablePtr> getAnimatables() override;

  /**
   * @brief Returns an array of the actively used textures.
   * @returns - Array of BaseTextures
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Disposes the resources of the material.
   * @param forceDisposeEffect - Forces the disposal of effects.
   * @param forceDisposeTextures - Forces the disposal of all textures.
   */
  void dispose(bool forceDisposeEffect   = false,
               bool forceDisposeTextures = false,
               bool notBoundToMesh       = false) override;

protected:
  /**
   * @brief Attaches a new image processing configuration to the PBR Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(
    const ImageProcessingConfigurationPtr& configuration);

  bool _shouldUseAlphaFromAlbedoTexture() const;

  /**
   * @brief Gets a boolean indicating that current material needs to register
   * RTT.
   */
  bool get_hasRenderTargetTextures() const override;

  /**
   * @brief Enabled the use of logarithmic depth buffers, which is good for wide
   * depth buffers.
   */
  bool get_useLogarithmicDepth() const override;

  /**
   * @brief Enabled the use of logarithmic depth buffers, which is good for wide
   * depth buffers.
   */
  void set_useLogarithmicDepth(bool value) override;

  /**
   * @brief Gets the current transparency mode.
   */
  std::optional<unsigned int>& get_transparencyMode();

  /**
   * @brief Sets the transparency mode of the material.
   *
   * | Value | Type                                | Description |
   * | ----- | ----------------------------------- | ----------- |
   * | 0     | OPAQUE                              |             |
   * | 1     | ALPHATEST                           |             |
   * | 2     | ALPHABLEND                          |             |
   * | 3     | ALPHATESTANDBLEND                   |             |
   *
   */
  void set_transparencyMode(const std::optional<unsigned int>& value);

  /**
   * @brief hidden
   * This is reserved for the inspector.
   * Defines the material debug mode.
   * It helps seeing only some components of the material while troubleshooting.
   */
  unsigned int get_debugMode() const;
  void set_debugMode(unsigned int value);

private:
  EffectPtr _prepareEffect(
    AbstractMesh* mesh, PBRMaterialDefines& defines,
    const std::function<void(Effect* effect)>& onCompiled = nullptr,
    std::function<void(Effect* effect, const std::string& errors)> onError
    = nullptr,
    const std::optional<bool>& useInstances = std::nullopt,
    const std::optional<bool>& useClipPlane = std::nullopt);
  void _prepareDefines(AbstractMesh* mesh, PBRMaterialDefines& defines,
                       const std::optional<bool>& useInstances = std::nullopt,
                       const std::optional<bool>& useClipPlane = std::nullopt);

  /**
   * @brief Returns the texture used for reflections.
   * @returns - Reflection texture if present.  Otherwise, returns the
   * environment texture.
   */
  BaseTexturePtr _getReflectionTexture() const;

public:
  /**
   * The transparency mode of the material.
   */
  Property<PBRBaseMaterial, std::optional<unsigned int>> transparencyMode;

  /**
   * This is reserved for the inspector.
   * Defines the material debug mode.
   * It helps seeing only some components of the material while
   * troubleshooting.
   */
  Property<PBRBaseMaterial, unsigned int> debugMode;

  /**
   * Defines the clear coat layer parameters for the material.
   */
  PBRClearCoatConfigurationPtr clearCoat;

  /**
   * Defines the anisotropic parameters for the material.
   */
  PBRAnisotropicConfigurationPtr anisotropy;

  /**
   * Defines the BRDF parameters for the material.
   */
  PBRBRDFConfigurationPtr brdf;

  /**
   * Defines the Sheen parameters for the material.
   */
  PBRSheenConfigurationPtr sheen;

  /**
   * Defines the SubSurface parameters for the material.
   */
  PBRSubSurfaceConfigurationPtr subSurface;

  /**
   * Custom callback helping to override the default shader used in the
   * material.
   */
  std::function<std::string(const std::string& shaderName,
                            const std::vector<std::string>& uniforms,
                            const std::vector<std::string>& uniformBuffers,
                            const std::vector<std::string>& samplers,
                            const PBRMaterialDefines& defines)>
    customShaderNameResolve;

protected:
  /**
   * Intensity of the direct lights e.g. the four lights available in your
   * scene.
   * This impacts both the direct diffuse and specular highlights.
   */
  float _directIntensity;

  /**
   * Intensity of the emissive part of the material.
   * This helps controlling the emissive effect without modifying the emissive
   * color.
   */
  float _emissiveIntensity;

  /**
   * Intensity of the environment e.g. how much the environment will light the
   * object either through harmonics for rough material or through the
   * refelction for shiny ones.
   */
  float _environmentIntensity;

  /**
   * This is a special control allowing the reduction of the specular highlights
   * coming from the four lights of the scene. Those highlights may not be
   * needed in full environment lighting.
   */
  float _specularIntensity;

  /**
   * Debug Control allowing disabling the bump map on this material.
   */
  bool _disableBumpMap;

  /**
   * AKA Diffuse Texture in standard nomenclature.
   */
  BaseTexturePtr _albedoTexture;

  /**
   * AKA Occlusion Texture in other nomenclature.
   */
  BaseTexturePtr _ambientTexture;

  /**
   * AKA Occlusion Texture Intensity in other nomenclature.
   */
  float _ambientTextureStrength;

  /**
   * Defines how much the AO map is occluding the analytical lights (point
   * spot...).
   * 1 means it completely occludes it
   * 0 mean it has no impact
   */
  unsigned int _ambientTextureImpactOnAnalyticalLights;

  /**
   * Stores the alpha values in a texture.
   */
  BaseTexturePtr _opacityTexture;

  /**
   * Stores the reflection values in a texture.
   */
  BaseTexturePtr _reflectionTexture;

  /**
   * Stores the emissive values in a texture.
   */
  BaseTexturePtr _emissiveTexture;

  /**
   * AKA Specular texture in other nomenclature.
   */
  BaseTexturePtr _reflectivityTexture;

  /**
   * Used to switch from specular/glossiness to metallic/roughness workflow.
   */
  BaseTexturePtr _metallicTexture;

  /**
   * Specifies the metallic scalar of the metallic/roughness workflow.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  std::optional<float> _metallic;

  /**
   * Specifies the roughness scalar of the metallic/roughness workflow.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  std::optional<float> _roughness;

  /**
   * Used to enable roughness/glossiness fetch from a separate channel depending
   * on the current mode. Gray Scale represents roughness in metallic mode and
   * glossiness in specular mode.
   */
  BaseTexturePtr _microSurfaceTexture;

  /**
   * Stores surface normal data used to displace a mesh in a texture.
   */
  BaseTexturePtr _bumpTexture;

  /**
   * Stores the pre-calculated light information of a mesh in a texture.
   */
  BaseTexturePtr _lightmapTexture;

  /**
   * The color of a material in ambient lighting.
   */
  Color3 _ambientColor;

  /**
   * AKA Diffuse Color in other nomenclature.
   */
  Color3 _albedoColor;

  /**
   * AKA Specular Color in other nomenclature.
   */
  Color3 _reflectivityColor;

  /**
   * The color applied when light is reflected from a material.
   */
  Color3 _reflectionColor;

  /**
   * The color applied when light is emitted from a material.
   */
  Color3 _emissiveColor;

  /**
   * AKA Glossiness in other nomenclature.
   */
  float _microSurface;

  /**
   * Specifies that the material will use the light map as a show map.
   */
  bool _useLightmapAsShadowmap;

  /**
   * This parameters will enable/disable Horizon occlusion to prevent normal
   * maps to look shiny when the normal makes the reflect vector face the model
   * (under horizon).
   */
  bool _useHorizonOcclusion;

  /**
   * This parameters will enable/disable radiance occlusion by preventing the
   * radiance to lit too much the area relying on ambient texture to define
   * their ambient occlusion.
   */
  bool _useRadianceOcclusion;

  /**
   * Specifies that the alpha is coming form the albedo channel alpha channel
   * for alpha blending.
   */
  bool _useAlphaFromAlbedoTexture;

  /**
   * Specifies that the material will keeps the specular highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When sun reflects on it you can not
   * see what is behind.
   */
  bool _useSpecularOverAlpha;

  /**
   * Specifies if the reflectivity texture contains the glossiness information
   * in its alpha channel.
   */
  bool _useMicroSurfaceFromReflectivityMapAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * alpha channel.
   */
  bool _useRoughnessFromMetallicTextureAlpha;

  /**
   * Specifies if the metallic texture contains the roughness information in its
   * green channel.
   */
  bool _useRoughnessFromMetallicTextureGreen;

  /**
   * Specifies if the metallic texture contains the metallness information in
   * its blue channel.
   */
  bool _useMetallnessFromMetallicTextureBlue;

  /**
   * Specifies if the metallic texture contains the ambient occlusion
   * information in its red channel.
   */
  bool _useAmbientOcclusionFromMetallicTextureRed;

  /**
   * Specifies if the ambient texture contains the ambient occlusion information
   * in its red channel only.
   */
  bool _useAmbientInGrayScale;

  /**
   * In case the reflectivity map does not contain the microsurface information
   * in its alpha channel,
   * The material will try to infer what glossiness each pixel should be.
   */
  bool _useAutoMicroSurfaceFromReflectivityMap;

  /**
   * Defines the  falloff type used in this material.
   * It by default is Physical.
   */
  unsigned int _lightFalloff;

  /**
   * Specifies that the material will keeps the reflection highlights over a
   * transparent surface (only the most limunous ones).
   * A car glass is a good exemple of that. When the street lights reflects on
   * it you can not see what is behind.
   */
  bool _useRadianceOverAlpha;

  /**
   * Allows using an object space normal map (instead of tangent space).
   */
  bool _useObjectSpaceNormalMap;

  /**
   * Allows using the bump map in parallax mode.
   */
  bool _useParallax;

  /**
   * Allows using the bump map in parallax occlusion mode.
   */
  bool _useParallaxOcclusion;

  /**
   * Controls the scale bias of the parallax mode.
   */
  float _parallaxScaleBias;

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  bool _disableLighting;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int _maxSimultaneousLights;

  /**
   * If sets to true, x component of normal map value will be inverted
   * (x = 1.0 - x).
   */
  bool _invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will be inverted
   * (y = 1.0 - y).
   */
  bool _invertNormalMapY;

  /**
   * If sets to true and backfaceCulling is false, normals will be flipped on
   * the backside.
   */
  bool _twoSidedLighting;

  /**
   * Defines the alpha limits in alpha test mode.
   */
  float _alphaCutOff;

  /**
   * Enforces alpha test in opaque or blend mode in order to improve the
   * performances of some situations.
   */
  bool _forceAlphaTest;

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested.
   * And/Or occlude the blended part. (alpha is converted to gamma to compute
   * the fresnel)
   */
  bool _useAlphaFresnel;

  /**
   * A fresnel is applied to the alpha of the model to ensure grazing angles
   * edges are not alpha tested. And/Or occlude the blended part. (alpha stays
   * linear to compute the fresnel)
   */
  bool _useLinearAlphaFresnel;

  /**
   * The transparency mode of the material.
   */
  std::optional<unsigned int> _transparencyMode;

  /**
   * Specifies the environment BRDF texture used to comput the scale and offset
   * roughness values
   * from cos thetav and roughness:
   * http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
   */
  BaseTexturePtr _environmentBRDFTexture;

  /**
   * Force the shader to compute irradiance in the fragment shader in order to
   * take bump in account.
   */
  bool _forceIrradianceInFragment;

  /**
   * Force normal to face away from face.
   */
  bool _forceNormalForward;

  /**
   * Enables specular anti aliasing in the PBR shader.
   * It will both interacts on the Geometry for analytical and IBL lighting.
   * It also prefilter the roughness map based on the bump values.
   */
  bool _enableSpecularAntiAliasing;

  /**
   * Default configuration related to image processing available in the PBR
   * Material.
   */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

  /**
   * If set to true, no lighting calculations will be applied.
   */
  bool _unlit;

  /**
   * Hidden
   */
  bool _rebuildInParallel;

private:
  /**
   * This stores the direct, emissive, environment, and specular light
   * intensities into a Vector4.
   */
  Vector4 _lightingInfos;
  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

  /**
   * Stores the available render targets.
   */
  std::vector<RenderTargetTexturePtr> _renderTargets;

  /**
   * Sets the global ambient color for the material used in lighting
   * calculations.
   */
  Color3 _globalAmbientColor;

  /**
   * Enables the use of logarithmic depth buffers, which is good for wide depth
   * buffers.
   */
  bool _useLogarithmicDepth;

  /**
   * This is reserved for the inspector.
   * Defines the material debug mode.
   * It helps seeing only some components of the material while troubleshooting.
   */
  unsigned int _debugMode;

  /**
   * This is reserved for the inspector.
   * Specify from where on screen the debug mode should start.
   * The value goes from -1 (full screen) to 1 (not visible)
   * It helps with side by side comparison against the final render
   * This defaults to -1
   */
  float debugLimit;

  /**
   * This is reserved for the inspector.
   * As the default viewing range might not be enough (if the ambient is really
   * small for instance) You can use the factor to better multiply the final
   * value.
   */
  float debugFactor;

}; // end of class PBRBaseMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_BASE_MATERIAL_H
