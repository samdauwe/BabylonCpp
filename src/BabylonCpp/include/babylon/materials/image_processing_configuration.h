#ifndef BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H
#define BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/math/color4.h>
#include <babylon/misc/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class BaseTexture;
class ColorCurves;
class Effect;
struct IImageProcessingConfigurationDefines;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using ColorCurvesPtr = std::shared_ptr<ColorCurves>;
using EffectPtr      = std::shared_ptr<Effect>;

/**
 * @brief This groups together the common properties used for image processing
 * either in direct forward pass or through post processing effect depending on
 * the use of the image processing pipeline in your scene or not.
 */
class BABYLON_SHARED_EXPORT ImageProcessingConfiguration {

private:
  // Static constants associated to the image processing.
  static constexpr unsigned int _VIGNETTEMODE_MULTIPLY = 0;
  static constexpr unsigned int _VIGNETTEMODE_OPAQUE   = 1;

public:
  /**
   * Default tone mapping applied in BabylonJS.
   */
  static constexpr unsigned int TONEMAPPING_STANDARD = 0;

  /**
   * ACES Tone mapping (used by default in unreal and unity). This can help
   * getting closer to other engines rendering to increase portability.
   */
  static constexpr unsigned int TONEMAPPING_ACES = 1;

  /**
   * @brief Used to apply the vignette as a mix with the pixel color.
   */
  static constexpr unsigned int VIGNETTEMODE_MULTIPLY()
  {
    return ImageProcessingConfiguration::_VIGNETTEMODE_MULTIPLY;
  }

  /**
   * @brief Used to apply the vignette as a replacement of the pixel color.
   */
  static constexpr unsigned int VIGNETTEMODE_OPAQUE()
  {
    return ImageProcessingConfiguration::_VIGNETTEMODE_OPAQUE;
  }

public:
  ImageProcessingConfiguration();
  ~ImageProcessingConfiguration(); // = default

  /**
   * @brief Gets the current class name.
   * @return "ImageProcessingConfiguration"
   */
  std::string getClassName() const;

  /**
   * @brief Prepare the list of uniforms associated with the Image Processing
   * effects.
   * @param uniforms The list of uniforms used in the effect
   * @param defines the list of defines currently in use
   */
  static void
  PrepareUniforms(std::vector<std::string>& uniforms,
                  const IImageProcessingConfigurationDefines& defines);

  /**
   * @brief Prepare the list of samplers associated with the Image Processing
   * effects.
   * @param samplersList The list of uniforms used in the effect
   * @param defines the list of defines currently in use
   */
  static void
  PrepareSamplers(std::vector<std::string>& samplersList,
                  const IImageProcessingConfigurationDefines& defines);

  /**
   * @brief Prepare the list of defines associated to the shader.
   * @param defines the list of defines to complete
   * @param forPostProcess Define if we are currently in post process mode or
   * not
   */
  void prepareDefines(IImageProcessingConfigurationDefines& defines,
                      bool forPostProcess = false);

  /**
   * @brief Returns true if all the image processing information are ready.
   * @returns True if ready, otherwise, false
   */
  bool isReady() const;

  /**
   * @brief Binds the image processing to the shader.
   * @param effect The effect to bind to
   * @param aspectRatio Define the current aspect ratio of the effect
   */
  void bind(Effect* effect, float aspectRatio = 1.f);

  /**
   * @brief Clones the current image processing instance.
   * @return The cloned image processing
   */
  std::unique_ptr<ImageProcessingConfiguration> clone();

  /**
   * @brief Serializes the current image processing instance to a json
   * representation.
   * @return a JSON representation
   */
  json serialize() const;

  /**
   * @brief Parses the image processing from a json representation.
   * @param source the JSON source to parse
   * @return The parsed image processing
   */
  static void Parse(const json& source);

protected:
  /**
   * @brief Gets wether the color curves effect is enabled.
   */
  bool get_colorCurvesEnabled() const;

  /**
   * @brief Sets wether the color curves effect is enabled.
   */
  void set_colorCurvesEnabled(bool value);

  /**
   * @brief Gets the color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */

  BaseTexturePtr& get_colorGradingTexture();

  /**
   * @brief Sets the color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */
  void set_colorGradingTexture(const BaseTexturePtr& value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  bool get_colorGradingEnabled() const;

  /**
   * @brief Sets wether the color grading effect is enabled.
   */
  void set_colorGradingEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is using a green depth for the
   * 3d Texture.
   */
  bool get_colorGradingWithGreenDepth() const;

  /**
   * @brief Sets wether the color grading effect is using a green depth for the
   * 3d Texture.
   */
  void set_colorGradingWithGreenDepth(bool value);

  /**
   * @brief Gets wether the color grading texture contains BGR values.
   */
  bool get_colorGradingBGR() const;

  /**
   * @brief Sets wether the color grading texture contains BGR values.
   */
  void set_colorGradingBGR(bool value);

  /**
   * @brief Gets the Exposure used in the effect.
   */
  float get_exposure() const;

  /**
   * @brief Sets the Exposure used in the effect.
   */
  void set_exposure(float value);

  /**
   * @brief Gets wether the tone mapping effect is enabled.
   */
  bool get_toneMappingEnabled() const;

  /**
   * @brief Sets wether the tone mapping effect is enabled.
   */
  void set_toneMappingEnabled(bool value);

  /**
   *  @brief Gets the type of tone mapping effect.
   */
  unsigned int get_toneMappingType() const;

  /**
   *  @brief Sets the type of tone mapping effect used in BabylonJS.
   */
  void set_toneMappingType(unsigned int value);

  /**
   * @brief Gets the contrast used in the effect.
   */
  float get_contrast() const;

  /**
   * @brief Sets the contrast used in the effect.
   */
  void set_contrast(float value);

  /**
   * @brief Gets the vignette blend mode allowing different kind of effect.
   */
  unsigned int get_vignetteBlendMode() const;

  /**
   * @brief Sets the vignette blend mode allowing different kind of effect.
   */
  void set_vignetteBlendMode(unsigned int value);

  /**
   * @brief Gets wether the vignette effect is enabled.
   */
  bool get_vignetteEnabled() const;

  /**
   * @brief Sets wether the vignette effect is enabled.
   */
  void set_vignetteEnabled(bool value);

  /**
   * @brief Gets wether the image processing is applied through a post process
   * or not.
   */
  bool get_applyByPostProcess() const;

  /**
   * @brief Sets wether the image processing is applied through a post process
   * or not.
   */
  void set_applyByPostProcess(bool value);

  /**
   * @brief Gets wether the image processing is enabled or not.
   */
  bool get_isEnabled() const;
  /**
   * @brief Sets wether the image processing is enabled or not.
   */
  void set_isEnabled(bool value);

  /**
   * Method called each time the image processing information changes requires
   * to recompile the effect.
   */
  void _updateParameters();

public:
  /**
   * Wether the color curves effect is enabled.
   */
  Property<ImageProcessingConfiguration, bool> colorCurvesEnabled;

  /**
   * Wether the color grading effect is enabled.
   */
  Property<ImageProcessingConfiguration, bool> colorGradingEnabled;

  /**
   * Wether the color grading effect is using a green depth for the 3d Texture.
   */
  Property<ImageProcessingConfiguration, bool> colorGradingWithGreenDepth;

  /**
   * Wether the color grading texture contains BGR values.
   */
  Property<ImageProcessingConfiguration, bool> colorGradingBGR;

  /**
   * Exposure used in the effect.
   */
  Property<ImageProcessingConfiguration, float> exposure;

  /**
   * Wether the tone mapping effect is enabled.
   */
  Property<ImageProcessingConfiguration, bool> toneMappingEnabled;

  /**
   * The type of tone mapping effect used in BabylonJS
   */
  Property<ImageProcessingConfiguration, unsigned int> toneMappingType;

  /**
   * Contrast used in the effect.
   */
  Property<ImageProcessingConfiguration, float> contrast;

  /**
   * Vignette blend mode allowing different kind of effect.
   */
  Property<ImageProcessingConfiguration, unsigned int> vignetteBlendMode;

  /**
   *Wether the vignette effect is enabled.
   */
  Property<ImageProcessingConfiguration, bool> vignetteEnabled;

  /**
   * Wether the image processing is applied through a post process or not.
   */
  Property<ImageProcessingConfiguration, bool> applyByPostProcess;

  /**
   * Wether the image processing is enabled or not.
   */
  Property<ImageProcessingConfiguration, bool> isEnabled;

  /**
   * Color curves setup used in the effect if colorCurvesEnabled is set to true
   */
  ColorCurvesPtr colorCurves;

  /**
   * Color grading LUT texture used in the effect if colorGradingEnabled is set
   * to true.
   */
  Property<ImageProcessingConfiguration, BaseTexturePtr> colorGradingTexture;

  /**
   * Hidden
   */
  float _exposure;

  /**
   * Vignette stretch size.
   */
  float vignetteStretch;

  /**
   * Vignette centre X Offset.
   */
  float vignetteCentreX;

  /**
   * Vignette centre Y Offset.
   */
  float vignetteCentreY;

  /**
   * Vignette weight or intensity of the vignette effect.
   */
  float vignetteWeight;

  /**
   * Color of the vignette applied on the screen through the chosen blend mode
   * (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  Color4 vignetteColor;

  /**
   * Camera field of view used by the Vignette effect.
   */
  float vignetteCameraFov;

  /**
   * An event triggered when the configuration changes and requires Shader to
   * Update some parameters.
   */
  Observable<ImageProcessingConfiguration> onUpdateParameters;

protected:
  float _contrast;

private:
  bool _colorCurvesEnabled;
  BaseTexturePtr _colorGradingTexture;
  bool _colorGradingEnabled;
  bool _colorGradingWithGreenDepth;
  bool _colorGradingBGR;
  bool _toneMappingEnabled;
  unsigned int _toneMappingType;
  unsigned int _vignetteBlendMode;
  bool _vignetteEnabled;
  bool _applyByPostProcess;
  bool _isEnabled;

}; // end of class ImageProcessingConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H
