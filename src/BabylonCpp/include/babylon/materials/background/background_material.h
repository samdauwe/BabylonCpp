#ifndef BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
#define BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/push_material.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Background material used to create an efficient environement around
 * your scene.
 */
class BABYLON_SHARED_EXPORT BackgroundMaterial : public PushMaterial {

public:
  using BMD = BackgroundMaterialDefines;

public:
  /**
   * @brief Returns Standard reflectance value at parallel view angle.
   */
  static float standardReflectance0();

  /**
   * @brief Returns Standard reflectance value at grazing angle.
   */
  static float standardReflectance90();

public:
  /**
   * @brief Constructor
   * @param name The name of the material
   * @param scene The scene to add the material to
   */
  BackgroundMaterial(const string_t& name, Scene* scene);
  ~BackgroundMaterial();

  /**
   * @brief Sets the reflection reflectance fresnel values according to the
   * default standard empirically know to work well :-)
   */
  void setReflectionStandardFresnelWeight(float value);

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
  BaseTexture* cameraColorGradingTexture() const;

  /**
   * @brief Sets the Color Grading 2D Lookup Texture.
   */
  void setCameraColorGradingTexture(BaseTexture* value);

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
  bool needAlphaTesting() override;

  /**
   * @brief The entire material has been created in order to prevent overdraw.
   * @returns true if blending is enable
   */
  bool needAlphaBlending() override;

  /**
   * @brief Checks wether the material is ready to be rendered for a given mesh.
   * @param mesh The mesh to render
   * @param subMesh The submesh to check against
   * @param useInstances Specify wether or not the material is used with
   * instances
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
   * @name The cloned name.
   * @returns The cloned material.
   */
  BackgroundMaterial* clone(const string_t& name,
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
  const char* getClassName() const;

  /**
   * @brief Parse a JSON input to create back a background material.
   * @param source
   * @param scene
   * @param rootUrl
   * @returns the instantiated BackgroundMaterial.
   */
  static unique_ptr_t<BackgroundMaterial>
  Parse(const Json::value& source, Scene* scene, const string_t& url);

  /** Getters / Setters **/
  BaseTexture* reflectionTexture() const;
  void setReflectionTexture(RenderTargetTexture* value);
  BaseTexture* diffuseTexture() const;
  void setDiffuseTexture(RenderTargetTexture* value);
  float shadowLevel() const;
  void setShadowLevel(float value);
  const Vector3& sceneCenter() const;
  void setSceneCenter(const Vector3& value);
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
   * @brief Attaches a new image processing configuration to the PBR Material.
   * @param configuration (if null the scene configuration will be use)
   */
  void _attachImageProcessingConfiguration(
    ImageProcessingConfiguration* configuration);

protected:
  /**
   * Key light Color (multiply against the R channel of the environement
   * texture)
   */
  Color3 _primaryColor;

  /**
   * Key light Level (allowing HDR output of the background)
   */
  float _primaryLevel;

  /**
   * Secondary light Color (multiply against the G channel of the environement
   * texture)
   */
  Color3 _secondaryColor;

  /**
   * Secondary light Level (allowing HDR output of the background)
   */
  float _secondaryLevel;

  /**
   * Tertiary light Color (multiply against the B channel of the environement
   * texture)
   */
  Color3 _tertiaryColor;

  /**
   * Tertiary light Level (allowing HDR output of the background)
   */
  float _tertiaryLevel;

  /**
   * Reflection Texture used in the material.
   * Should be author in a specific way for the best result (refer to the
   * documentation).
   */
  RenderTargetTexture* _reflectionTexture;

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
  RenderTargetTexture* _diffuseTexture;

  /**
   * Specify the list of lights casting shadow on the material.
   * All scene shadow lights will be included if null.
   */
  vector_t<IShadowLight*> _shadowLights;

  /**
   * For the lights having a blurred shadow generator, this can add a second
   * blur pass in order to reach
   * soft lighting on the background.
   */
  int _shadowBlurScale;

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
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int _maxSimultaneousLights;

  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;

  // Temp values kept as cache in the material.
  vector_t<RenderTargetTexture*> _renderTargets;
  Vector4 _reflectionControls;

}; // end of class BackgroundMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_H
