#ifndef BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
#define BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/push_material.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class BackgroundMaterial;
class ImageProcessingConfiguration;
class IShadowLight;
using BackgroundMaterialPtr = std::shared_ptr<BackgroundMaterial>;
using ImageProcessingConfigurationPtr
  = std::shared_ptr<ImageProcessingConfiguration>;
using IShadowLightPtr = std::shared_ptr<IShadowLight>;

/**
 * @brief Background material used to create an efficient environement around
 * your scene.
 */
class BABYLON_SHARED_EXPORT BackgroundMaterial : public PushMaterial {

public:
  /**
   * @brief Returns Standard reflectance value at parallel view angle.
   */
  static float StandardReflectance0();

  /**
   * @brief Returns Standard reflectance value at grazing angle.
   */
  static float StandardReflectance90();

public:
  template <typename... Ts>
  static BackgroundMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<BackgroundMaterial>(
      new BackgroundMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~BackgroundMaterial() override;

  /**
   * @brief The entire material has been created in order to prevent overdraw.
   * @returns false
   */
  bool needAlphaTesting() const override;

  /**
   * @brief The entire material has been created in order to prevent overdraw.
   * @returns true if blending is enable
   */
  bool needAlphaBlending() const override;

  /**
   * @brief Checks wether the material is ready to be rendered for a given mesh.
   * @param mesh The mesh to render
   * @param subMesh The submesh to check against
   * @param useInstances Specify wether or not the material is used with
   * instances
   * @returns true if all the dependencies are ready (Textures, Effects...)
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Build the uniform buffer used in the material.
   */
  void buildUniformLayout();

  /**
   * @brief Unbind the material.
   */
  void unbind() override;

  /**
   * @brief Bind only the world matrix to the material.
   * @param world The world matrix to bind.
   */
  void bindOnlyWorldMatrix(Matrix& world) override;

  /**
   * @brief Bind the material for a dedicated submeh (every used meshes will be
   * considered opaque).
   * @param world The world matrix to bind.
   * @param subMesh The submesh to bind for.
   */
  void bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Dispose the material.
   * @param forceDisposeEffect Force disposal of the associated effect.
   * @param forceDisposeTextures Force disposal of the associated textures.
   */
  void dispose(bool forceDisposeEffect   = false,
               bool forceDisposeTextures = false,
               bool notBoundToMesh       = false) override;

  /**
   * @brief Clones the material.
   * @param name The cloned name.
   * @returns The cloned material.
   */
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;

  /**
   * @brief Serializes the current material to its JSON representation.
   * @returns The JSON representation.
   */
  json serialize() const;

  /**
   * @brief Gets the class name of the material
   * @returns "BackgroundMaterial"
   */
  const std::string getClassName() const override;

  /**
   * @brief Parse a JSON input to create back a background material.
   * @param source The JSON data to parse
   * @param scene The scene to create the parsed material in
   * @param rootUrl The root url of the assets the material depends upon
   * @returns the instantiated BackgroundMaterial.
   */
  static std::unique_ptr<BackgroundMaterial>
  Parse(const json& source, Scene* scene, const std::string& url);

protected:
  /**
   * @brief Instantiates a Background Material in the given scene
   * @param name The friendly name of the material
   * @param scene The scene to add the material to
   */
  BackgroundMaterial(const std::string& name, Scene* scene);

  /**
   * @brief Attaches a new image processing configuration to the PBR Material.
   * @param configuration (if null the scene configuration will be use)
   */
  void _attachImageProcessingConfiguration(
    const ImageProcessingConfigurationPtr& configuration);

  /**
   * @brief Gets the key light Color (multiply against the environement
   * texture).
   */
  Color3& get_primaryColor();

  /**
   * @brief Sets the key light Color (multiply against the environement
   * texture).
   */
  void set_primaryColor(const Color3& value);

  /**
   * @brief Gets the key light Color in "perceptual value" meaning the color you
   * would like to see on screen. This acts as a helper to set the primary color
   * to a more "human friendly" value. Conversion to linear space as well as
   * exposure and tone mapping correction will be applied to keep the output
   * color as close as possible from the chosen value. (This does not account
   * for contrast color grading and color curves as they are considered post
   * effect and not directly part of lighting setup.)
   */
  std::optional<Color3>& get__perceptualColor();

  /**
   * @brief Sets the key light Color in "perceptual value" meaning the color you
   * would like to see on screen. This acts as a helper to set the primary color
   * to a more "human friendly" value. Conversion to linear space as well as
   * exposure and tone mapping correction will be applied to keep the output
   * color as close as possible from the chosen value. (This does not account
   * for contrast color grading and color curves as they are considered post
   * effect and not directly part of lighting setup.)
   */
  void set__perceptualColor(const std::optional<Color3>& value);

  /**
   * @brief Gets the level of the shadows (dark area of the reflection map) in
   * order to help scaling the colors. The color opposite to the primary color
   * is used at the level chosen to define what the black area would look.
   */
  float get_primaryColorShadowLevel() const;

  /**
   * @brief Sets the level of the shadows (dark area of the reflection map) in
   * order to help scaling the colors. The color opposite to the primary color
   * is used at the level chosen to define what the black area would look.
   */
  void set_primaryColorShadowLevel(float value);

  /**
   *  @brief Gets the level of the highliights (highlight area of the reflection
   * map) in order to help scaling the colors. The primary color is used at the
   * level chosen to define what the white area would look.
   */
  float get_primaryColorHighlightLevel() const;

  /**
   *  @brief Sets the level of the highliights (highlight area of the reflection
   * map) in order to help scaling the colors. The primary color is used at the
   * level chosen to define what the white area would look.
   */
  void set_primaryColorHighlightLevel(float value);

  /**
   *  @brief Gets reflection Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  BaseTexturePtr& get_reflectionTexture();

  /**
   *  @brief Sets reflection Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  void set_reflectionTexture(const BaseTexturePtr& value);

  /**
   *  @brief Gets the reflection Texture level of blur.
   *
   * Can be use to reuse an existing HDR Texture and target a specific LOD to
   * prevent authoring the texture twice.
   */
  float get_reflectionBlur() const;

  /**
   *  @brief Sets the reflection Texture level of blur.
   *
   * Can be use to reuse an existing HDR Texture and target a specific LOD to
   * prevent authoring the texture twice.
   */
  void set_reflectionBlur(float value);

  /**
   * @brief Gets the diffuse Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  BaseTexturePtr& get_diffuseTexture();

  /**
   * @brief Sets the diffuse Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  void set_diffuseTexture(const BaseTexturePtr& value);

  /**
   * @brief Gets the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  std::vector<IShadowLightPtr>& get_shadowLights();

  /**
   * @brief Sets the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  void set_shadowLights(const std::vector<IShadowLightPtr>& shadowLights);

  /**
   * @brief Helps adjusting the shadow to a softer level if required.
   * 0 means black shadows and 1 means no shadows.
   */
  float get_shadowLevel() const;

  /**
   * @brief Helps adjusting the shadow to a softer level if required.
   * 0 means black shadows and 1 means no shadows.
   */
  void set_shadowLevel(float value);

  /**
   * @brief In case of opacity Fresnel or reflection falloff, this is use as a
   * scene center. It is usually zero but might be interesting to modify
   * according to your setup.
   */
  Vector3& get_sceneCenter();

  /**
   * @brief In case of opacity Fresnel or reflection falloff, this is use as a
   * scene center. It is usually zero but might be interesting to modify
   * according to your setup.
   */
  void set_sceneCenter(const Vector3& value);

  /**
   * @brief This helps specifying that the material is falling off to the sky
   * box at grazing angle. This helps ensuring a nice transition when the camera
   * goes under the ground.
   */
  bool get_opacityFresnel() const;

  /**
   * @brief This helps specifying that the material is falling off to the sky
   * box at grazing angle. This helps ensuring a nice transition when the camera
   * goes under the ground.
   */
  void set_opacityFresnel(bool value);

  /**
   * @brief This helps specifying that the material is falling off from diffuse
   * to the reflection texture at grazing angle. This helps adding a mirror
   * texture on the ground.
   */
  bool get_reflectionFresnel() const;

  /**
   * @brief This helps specifying that the material is falling off from diffuse
   * to the reflection texture at grazing angle. This helps adding a mirror
   * texture on the ground.
   */
  void set_reflectionFresnel(bool value);

  /**
   * @brief This helps specifying the falloff radius off the reflection texture
   * from the sceneCenter. This helps adding a nice falloff effect to the
   * reflection if used as a mirror for instance.
   */
  float get_reflectionFalloffDistance() const;

  /**
   * @brief This helps specifying the falloff radius off the reflection texture
   * from the sceneCenter. This helps adding a nice falloff effect to the
   * reflection if used as a mirror for instance.
   */
  void set_reflectionFalloffDistance(float value);

  /**
   * @brief Gets the weight of the reflection against the background in case of
   * reflection Fresnel.
   */
  float get_reflectionAmount() const;

  /**
   * @brief Sets the weight of the reflection against the background in case of
   * reflection Fresnel.
   */
  void set_reflectionAmount(float value);

  /**
   * @brief Gets the weight of the reflection at grazing angle.
   */
  float get_reflectionReflectance0() const;

  /**
   * @brief Sets the weight of the reflection at grazing angle.
   */
  void set_reflectionReflectance0(float value);

  /**
   * @brief Gets the weight of the reflection at a perpendicular point of view.
   */
  float get_reflectionReflectance90() const;

  /**
   * @brief Sets the weight of the reflection at a perpendicular point of view.
   */
  void set_reflectionReflectance90(float value);

  /**
   * @brief Gets the reflection reflectance fresnel values according to the
   * default standard empirically know to work well :-)
   */
  void set_reflectionStandardFresnelWeight(float value);

  /**
   * @brief Helps to directly use the maps channels instead of their level.
   */
  bool get_useRGBColor() const;

  /**
   * @brief Helps to directly use the maps channels instead of their level.
   */
  void set_useRGBColor(bool value);

  /**
   * @brief This helps reducing the banding effect that could occur on the
   * background.
   */
  bool get_enableNoise() const;

  /**
   * @brief This helps reducing the banding effect that could occur on the
   * background.
   */
  void set_enableNoise(bool value);

  /**
   * @brief Gets the current fov(field of view) multiplier.
   * @return The current fov(field of view) multiplier.
   */
  float get_fovMultiplier() const;

  /**
   * @brief Sets the reflection reflectance fresnel values according to the
   * default standard empirically know to work well :-)
   */
  void set_fovMultiplier(float value);

  /**
   * @brief Gets the number of Simultaneous lights allowed on the material.
   */
  unsigned int get_maxSimultaneousLights() const;

  /**
   * @brief Sets the number of Simultaneous lights allowed on the material.
   */
  void set_maxSimultaneousLights(unsigned int value);

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
   * @brief Sets wether tonemapping is enabled or not
   */
  void set_cameraToneMappingEnabled(bool value);

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process. This corresponds to a photographic
   * exposure.
   */
  float get_cameraExposure() const;

  /**
   * @brief The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process. This corresponds to a photographic
   * exposure.
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
   * is applied after any color grading transform (3D LUT). They allow basic
   * adjustment of saturation and small exposure adjustments, along with color
   * filter tinting to provide white balance adjustment or more stylistic
   * effects. These are similar to controls found in many professional imaging
   * or colorist software. The global controls are applied to the entire image.
   * For advanced tuning, extra controls are provided to adjust the shadow,
   * midtone and highlight areas of the image; corresponding to low luminance,
   * medium luminance, and high luminance areas respectively.
   */
  ColorCurvesPtr& get_cameraColorCurves();

  /**
   * @brief The color grading curves provide additional color adjustmnent that
   * is applied after any color grading transform (3D LUT). They allow basic
   * adjustment of saturation and small exposure adjustments, along with color
   * filter tinting to provide white balance adjustment or more stylistic
   * effects. These are similar to controls found in many professional imaging
   * or colorist software. The global controls are applied to the entire image.
   * For advanced tuning, extra controls are provided to adjust the shadow,
   * midtone and highlight areas of the image; corresponding to low luminance,
   * medium luminance, and high luminance areas respectively.
   */
  void set_cameraColorCurves(const ColorCurvesPtr& value);

  /**
   * @brief Gets a boolean indicating that current material needs to register
   * RTT.
   */
  bool get_hasRenderTargetTextures() const override;

private:
  /**
   * @brief Compute the primary color according to the chosen perceptual color.
   */
  void _computePrimaryColorFromPerceptualColor();

  /**
   * @brief Compute the highlights and shadow colors according to their chosen
   * levels.
   */
  void _computePrimaryColors();

public:
  /**
   * Key light Color (multiply against the environement texture)
   */
  Property<BackgroundMaterial, Color3> primaryColor;

  /**
   * Experimental Internal Use Only.
   *
   * Key light Color in "perceptual value" meaning the color you would like to
   * see on screen. This acts as a helper to set the primary color to a more
   * "human friendly" value. Conversion to linear space as well as exposure and
   * tone mapping correction will be applied to keep the output color as close
   * as possible from the chosen value. (This does not account for contrast
   * color grading and color curves as they are considered post effect and not
   * directly part of lighting setup.)
   */
  Property<BackgroundMaterial, std::optional<Color3>> _perceptualColor;

  /**
   * Defines the level of the shadows (dark area of the reflection map) in order
   * to help scaling the colors. The color opposite to the primary color is used
   * at the level chosen to define what the black area would look.
   */
  Property<BackgroundMaterial, float> primaryColorShadowLevel;

  /**
   * Defines the level of the highliights (highlight area of the reflection map)
   * in order to help scaling the colors. The primary color is used at the level
   * chosen to define what the white area would look.
   */
  Property<BackgroundMaterial, float> primaryColorHighlightLevel;

  /**
   * Reflection Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  Property<BackgroundMaterial, BaseTexturePtr> reflectionTexture;

  /**
   * Reflection Texture level of blur.
   *
   * Can be use to reuse an existing HDR Texture and target a specific LOD to
   * prevent authoring the texture twice.
   */
  Property<BackgroundMaterial, float> reflectionBlur;

  /**
   * Diffuse Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  Property<BackgroundMaterial, BaseTexturePtr> diffuseTexture;

  /**
   * Specify the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  Property<BackgroundMaterial, std::vector<IShadowLightPtr>> shadowLights;

  /**
   * Helps adjusting the shadow to a softer level if required.
   * 0 means black shadows and 1 means no shadows.
   */
  Property<BackgroundMaterial, float> shadowLevel;

  /**
   * In case of opacity Fresnel or reflection falloff, this is use as a scene
   * center. It is usually zero but might be interesting to modify according to
   * your setup.
   */
  Property<BackgroundMaterial, Vector3> sceneCenter;
  /**
   * This helps specifying that the material is falling off to the sky box at
   * grazing angle. This helps ensuring a nice transition when the camera goes
   * under the ground.
   */
  Property<BackgroundMaterial, bool> opacityFresnel;

  /**
   * This helps specifying that the material is falling off from diffuse to the
   * reflection texture at grazing angle. This helps adding a mirror texture on
   * the ground.
   */
  Property<BackgroundMaterial, bool> reflectionFresnel;

  /**
   * This helps specifying the falloff radius off the reflection texture from
   * the sceneCenter. This helps adding a nice falloff effect to the reflection
   * if used as a mirror for instance.
   */
  Property<BackgroundMaterial, float> reflectionFalloffDistance;

  /**
   * This specifies the weight of the reflection against the background in case
   * of reflection Fresnel.
   */
  Property<BackgroundMaterial, float> reflectionAmount;

  /**
   * This specifies the weight of the reflection at grazing angle.
   */
  Property<BackgroundMaterial, float> reflectionReflectance0;

  /**
   * This specifies the weight of the reflection at a perpendicular point of
   * view.
   */
  Property<BackgroundMaterial, float> reflectionReflectance90;

  /**
   * Sets the reflection reflectance fresnel values according to the default
   * standard empirically know to work well :-)
   */
  WriteOnlyProperty<BackgroundMaterial, float> reflectionStandardFresnelWeight;

  /**
   * Helps to directly use the maps channels instead of their level.
   */
  Property<BackgroundMaterial, bool> useRGBColor;

  /**
   * This helps reducing the banding effect that could occur on the background.
   */
  Property<BackgroundMaterial, bool> enableNoise;

  /**
   * The current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0.
   * Lower values "zoom in" and higher values "zoom out". Best used when trying
   * to implement visual zoom effects like fish-eye or binoculars while not
   * adjusting camera fov. Recommended to be keep at 1.0 except for special
   * cases.
   */
  Property<BackgroundMaterial, float> fovMultiplier;

  /**
   * Enable the FOV adjustment feature controlled by fovMultiplier.
   */
  bool useEquirectangularFOV;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  Property<BackgroundMaterial, unsigned int> maxSimultaneousLights;

  /**
   * Gets the image processing configuration used either in this material.
   */
  Property<BackgroundMaterial, ImageProcessingConfigurationPtr>
    imageProcessingConfiguration;

  /**
   * Gets wether the color curves effect is enabled.
   */
  Property<BackgroundMaterial, bool> cameraColorCurvesEnabled;

  /**
   * Gets wether the color grading effect is enabled.
   */
  Property<BackgroundMaterial, bool> cameraColorGradingEnabled;

  /**
   * Gets wether tonemapping is enabled or not.
   */
  Property<BackgroundMaterial, bool> cameraToneMappingEnabled;

  /**
   * The camera exposure used on this material.
   * This property is here and not in the camera to allow controlling exposure
   * without full screen post process. This corresponds to a photographic
   * exposure.
   */
  Property<BackgroundMaterial, float> cameraExposure;

  /**
   * Gets The camera contrast used on this material.
   */
  Property<BackgroundMaterial, float> cameraContrast;

  /**
   * Gets the Color Grading 2D Lookup Texture.
   */
  Property<BackgroundMaterial, BaseTexturePtr> cameraColorGradingTexture;

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
  Property<BackgroundMaterial, ColorCurvesPtr> cameraColorCurves;

protected:
  /**
   * Key light Color (multiply against the environement texture)
   */
  Color3 _primaryColor;

  /**
   * Experimental Internal Use Only.
   *
   * Key light Color in "perceptual value" meaning the color you would like to
   * see on screen. This acts as a helper to set the primary color to a more
   * "human friendly" value. Conversion to linear space as well as exposure and
   * tone mapping correction will be applied to keep the output color as close
   * as possible from the chosen value. (This does not account for contrast
   * color grading and color curves as they are considered post effect and not
   * directly part of lighting setup.)
   */
  std::optional<Color3> __perceptualColor;

  /**
   * Defines the level of the shadows (dark area of the reflection map) in order
   * to help scaling the colors. The color opposite to the primary color is used
   * at the level chosen to define what the black area would look.
   */
  float _primaryColorShadowLevel;

  /**
   * Defines the level of the highliights (highlight area of the reflection map)
   * in order to help scaling the colors. The primary color is used at the level
   * chosen to define what the white area would look.
   */
  float _primaryColorHighlightLevel;

  /**
   * Reflection Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  BaseTexturePtr _reflectionTexture;

  /**
   * Reflection Texture level of blur.
   *
   * Can be use to reuse an existing HDR Texture and target a specific LOD to
   * prevent authoring the
   * texture twice.
   */
  float _reflectionBlur;

  /**
   * Diffuse Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  BaseTexturePtr _diffuseTexture;

  /**
   * Specify the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  std::vector<IShadowLightPtr> _shadowLights;

  /**
   * Helps adjusting the shadow to a softer level if required.
   * 0 means black shadows and 1 means no shadows.
   */
  float _shadowLevel;

  /**
   * In case of opacity Fresnel or reflection falloff, this is use as a scene
   * center.
   * It is usually zero but might be interesting to modify according to your
   * setup.
   */
  Vector3 _sceneCenter;

  /**
   * This helps specifying that the material is falling off to the sky box at
   * grazing angle.
   * This helps ensuring a nice transition when the camera goes under the
   * ground.
   */
  bool _opacityFresnel;

  /**
   * This helps specifying that the material is falling off from diffuse to the
   * reflection texture at grazing angle.
   * This helps adding a mirror texture on the ground.
   */
  bool _reflectionFresnel;

  /**
   * This helps specifying the falloff radius off the reflection texture from
   * the sceneCenter.
   * This helps adding a nice falloff effect to the reflection if used as a
   * mirror for instance.
   */
  float _reflectionFalloffDistance;

  /**
   * This specifies the weight of the reflection against the background in case
   * of reflection Fresnel.
   */
  float _reflectionAmount;

  /**
   * This specifies the weight of the reflection at grazing angle.
   */
  float _reflectionReflectance0;

  /**
   * This specifies the weight of the reflection at a perpendicular point of
   * view.
   */
  float _reflectionReflectance90;

  /**
   * Helps to directly use the maps channels instead of their level.
   */
  bool _useRGBColor;

  /**
   * This helps reducing the banding effect that could occur on the background.
   */
  bool _enableNoise;

  /**
   * Default configuration related to image processing available in the
   * Background Material.
   */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

private:
  /**
   * Standard reflectance value at parallel view angle.
   */
  static float _standardReflectance0;

  /**
   * Standard reflectance value at grazing angle.
   */
  static float _standardReflectance90;

private:
  /**
   * The current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0.
   * Lower values "zoom in" and higher values "zoom out". Best used when trying
   * to implement visual zoom effects like fish-eye or binoculars while not
   * adjusting camera fov. Recommended to be keep at 1.0 except for special
   * cases.
   */
  float _fovMultiplier;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int _maxSimultaneousLights;

  /**
   * Due to a bug in iOS10, video tags (which are using the background material)
   * are in BGR and not RGB. Setting this flag to true (not done automatically!)
   * will convert it back to RGB.
   */
  bool switchToBGR;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

  // Temp values kept as cache in the material.
  std::vector<RenderTargetTexturePtr> _renderTargets;
  Vector4 _reflectionControls;
  Color3 _white;
  Color3 _primaryShadowColor;
  Color3 _primaryHighlightColor;

}; // end of class BackgroundMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
