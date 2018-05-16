#ifndef BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H
#define BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

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
  ~ImageProcessingConfiguration();

  /**
   * @brief Returns the string "ImageProcessingConfiguration".
   */
  const string_t getClassName() const;

  /**
   * @brief Gets wether the color curves effect is enabled.
   */
  bool colorCurvesEnabled() const;

  /**
   * @brief Sets wether the color curves effect is enabled.
   */
  void setColorCurvesEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  bool colorGradingEnabled() const;

  /**
   * @brief Sets wether the color grading effect is enabled.
   */
  void setColorGradingEnabled(bool value);

  /**
   * @brief Gets wether the color grading effect is using a green depth for the
   * 3d Texture.
   */
  bool colorGradingWithGreenDepth() const;

  /**
   * @brief Sets wether the color grading effect is using a green depth for the
   * 3d Texture.
   */
  void setColorGradingWithGreenDepth(bool value);

  /**
   * @brief Gets wether the color grading texture contains BGR values.
   */
  bool colorGradingBGR() const;

  /**
   * @brief Sets wether the color grading texture contains BGR values.
   */
  void setColorGradingBGR(bool value);

  /**
   * @brief Gets the Exposure used in the effect.
   */
  float exposure() const;

  /**
   * @brief Sets the Exposure used in the effect.
   */
  void setExposure(float value);

  /**
   * @brief Gets wether the tone mapping effect is enabled.
   */
  bool toneMappingEnabled() const;

  /**
   * @brief Sets wether the tone mapping effect is enabled.
   */
  void setToneMappingEnabled(bool value);

  /**
   * @brief Gets the contrast used in the effect.
   */
  float contrast() const;

  /**
   * @brief Sets the contrast used in the effect.
   */
  void setContrast(float value);

  /**
   * @brief Gets the vignette blend mode allowing different kind of effect.
   */
  unsigned int vignetteBlendMode() const;

  /**
   * @brief Sets the vignette blend mode allowing different kind of effect.
   */
  void setVignetteBlendMode(unsigned int value);

  /**
   * @brief Gets wether the vignette effect is enabled.
   */
  bool vignetteEnabled() const;

  /**
   * @brief Sets wether the vignette effect is enabled.
   */
  void setVignetteEnabled(bool value);

  /**
   * @brief Gets wether the image processing is applied through a post process
   * or not.
   */
  bool applyByPostProcess() const;

  /**
   * @brief Gets wether the image processing is enabled or not.
   */
  bool isEnabled() const;
  /**
   * @brief Sets wether the image processing is enabled or not.
   */
  void setIsEnabled(bool value);

  /**
   * @brief Sets wether the image processing is applied through a post process
   * or not.
   */
  void setApplyByPostProcess(bool value);

  /**
   * @brief Prepare the list of uniforms associated with the Image Processing
   * effects.
   * @param uniformsList The list of uniforms used in the effect
   * @param defines the list of defines currently in use
   */
  static void
  PrepareUniforms(vector_t<string_t>& uniforms,
                  const IImageProcessingConfigurationDefines& defines);

  /**
   * @brief Prepare the list of samplers associated with the Image Processing
   * effects.
   * @param uniformsList The list of uniforms used in the effect
   * @param defines the list of defines currently in use
   */
  static void
  PrepareSamplers(vector_t<string_t>& samplersList,
                  const IImageProcessingConfigurationDefines& defines);

  /**
   * @brief Prepare the list of defines associated to the shader.
   * @param defines the list of defines to complete
   */
  void prepareDefines(IImageProcessingConfigurationDefines& defines,
                      bool forPostProcess = false);

  /**
   * @brief Returns true if all the image processing information are ready.
   */
  bool isReady() const;

  /**
   * @brief Binds the image processing to the shader.
   * @param effect The effect to bind to
   */
  void bind(Effect* effect, float aspectRatio = 1.f);

  /**
   * @brief Clones the current image processing instance.
   * @return The cloned image processing
   */
  unique_ptr_t<ImageProcessingConfiguration> clone();

  /**
   * @brief Serializes the current image processing instance to a json
   * representation.
   * @return a JSON representation
   */
  Json::object serialize() const;

  /**
   * @brief Parses the image processing from a json representation.
   * @param source the JSON source to parse
   * @return The parsed image processing
   */
  static void Parse(const Json::value& source);

protected:
  /**
   * Method called each time the image processing information changes requires
   * to recompile the effect.
   */
  void _updateParameters();

public:
  /**
   * Color curves setup used in the effect if colorCurvesEnabled is set to true
   */
  shared_ptr_t<ColorCurves> colorCurves;

  /**
   * Color grading LUT texture used in the effect if colorGradingEnabled is set
   * to true.
   */
  BaseTexture* colorGradingTexture;

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
  bool _colorGradingEnabled;
  bool _colorGradingWithGreenDepth;
  bool _colorGradingBGR;
  bool _toneMappingEnabled;
  unsigned int _vignetteBlendMode;
  bool _vignetteEnabled;
  bool _applyByPostProcess;
  bool _isEnabled;

}; // end of class ImageProcessingConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_H
