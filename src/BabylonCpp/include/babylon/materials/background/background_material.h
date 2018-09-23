#ifndef BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
#define BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/push_material.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BackgroundMaterial;
class IShadowLight;
using BackgroundMaterialPtr = std::shared_ptr<BackgroundMaterial>;

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
      new BackgroundMaterial(::std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~BackgroundMaterial() override;

  /**
   * @brief Sets the reflection reflectance fresnel values according to the
   * default standard empirically know to work well :-)
   */
  void setReflectionStandardFresnelWeight(float value);

  /**
   * @brief Gets the current fov(field of view) multiplier.
   * @return The current fov(field of view) multiplier.
   */
  float fovMultiplier() const;

  /**
   * @brief Sets the current fov(field of view) multiplier, 0.0 - 2.0. Defaults
   * to 1.0. Lower values "zoom in" and higher values "zoom out". Best used when
   * trying to implement visual zoom effects like fish-eye or binoculars while
   * not adjusting camera fov. Recommended to be keep at 1.0 except for special
   * cases.
   * @param value The fov(field of view) multiplier value.
   */
  void setFovMultiplier(float value);

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
   * @brief Gets wether the color curves effect is enabled.
   */
  bool cameraColorCurvesEnabled() const;

  /**
   * @brief Sets wether the color curves effect is enabled.
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
  float cameraExposure();

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
  BaseTexturePtr& cameraColorGradingTexture();

  /**
   * @brief Sets the Color Grading 2D Lookup Texture.
   */
  void setCameraColorGradingTexture(const BaseTexturePtr& value);

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
  ColorCurves* cameraColorCurves();

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
  void setCameraColorCurves(ColorCurves* value);

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
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;

  /**
   * @brief Dispose the material.
   * @param forceDisposeEffect Force disposal of the associated effect.
   * @param forceDisposeTextures Force disposal of the associated textures.
   */
  void dispose(bool forceDisposeEffect   = false,
               bool forceDisposeTextures = false) override;

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
  Json::object serialize() const;

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
  Parse(const Json::value& source, Scene* scene, const std::string& url);

  /** Getters / Setters **/
  const Color3& primaryColor() const;
  void setPrimaryColor(const Color3& value);
  std::optional<Color3>& _perceptualColor();
  void setPerceptualColor(const std::optional<Color3>& value);
  float primaryColorShadowLevel() const;
  void setPrimaryColorShadowLevel(float value);
  float primaryColorHighlightLevel() const;
  void setPrimaryColorHighlightLevel(float value);
  BaseTexturePtr reflectionTexture();
  void setReflectionTexture(const RenderTargetTexturePtr& value);
  BaseTexturePtr diffuseTexture();
  void setDiffuseTexture(const RenderTargetTexturePtr& value);
  float shadowLevel() const;
  void setShadowLevel(float value);
  const Vector3& sceneCenter() const;
  void setSceneCenter(const Vector3& value);
  bool opacityFresnel() const;
  void setOpacityFresnel(bool value);
  bool reflectionFresnel() const;
  void setReflectionFresnel(bool value);
  float reflectionFalloffDistance() const;
  void setReflectionFalloffDistance(float value);
  float reflectionAmount() const;
  void setReflectionAmount(float value);
  float reflectionReflectance0() const;
  void setReflectionReflectance0(float value);
  float reflectionReflectance90() const;
  void setReflectionReflectance90(float value);
  bool useRGBColor() const;
  void setUseRGBColor(bool value);
  bool enableNoise() const;
  void setEnableNoise(bool value);

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
    ImageProcessingConfiguration* configuration);

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
   * Enable the FOV adjustment feature controlled by fovMultiplier.
   */
  bool useEquirectangularFOV;

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
  RenderTargetTexturePtr _reflectionTexture;

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
  RenderTargetTexturePtr _diffuseTexture;

  /**
   * Specify the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  std::vector<IShadowLight*> _shadowLights;

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
  ImageProcessingConfiguration* _imageProcessingConfiguration;

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
  std::vector<RenderTargetTexture*> _renderTargets;
  Vector4 _reflectionControls;
  Color3 _white;
  Color3 _primaryShadowColor;
  Color3 _primaryHighlightColor;

}; // end of class BackgroundMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
