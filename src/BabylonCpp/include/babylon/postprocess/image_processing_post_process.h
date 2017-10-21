#ifndef BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H
#define BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT ImageProcessingPostProcess : public PostProcess {

public:
  ImageProcessingPostProcess(const string_t& name, float renderRatio,
                             Camera* camera, unsigned int samplingMode,
                             Engine* engine, bool reusable = false,
                             unsigned int textureType
                             = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~ImageProcessingPostProcess();

  /**
   * @brief Returns the string "ImageProcessingPostProcess"
   */
  const char* getClassName() const;

  /**
   * @brief Gets the image processing configuration used either in this
   * material.
   */
  ImageProcessingConfiguration* imageProcessingConfiguration();

  /**
   * @brief Sets the Default image processing configuration used either in the
   * this material.
   *
   * If sets to null, the scene one is in use.
   */
  void setImageProcessingConfiguration(ImageProcessingConfiguration* value);

  /**
   * @brief Gets Color curves setup used in the effect if colorCurvesEnabled is
   * set to true .
   */
  ColorCurves* colorCurves() const;

  /**
   * Sets Color curves setup used in the effect if colorCurvesEnabled is set to
   * true .
   */
  void setColorCurves(ColorCurves* value);

  /**
   * @brief Gets wether the color curves effect is enabled.
   */
  bool colorCurvesEnabled() const;

  /**
   * Sets wether the color curves effect is enabled.
   */
  void setColorCurvesEnabled(bool value);

  /**
   * @brief Gets Color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */
  BaseTexture* colorGradingTexture() const;

  /**
   * @brief Sets Color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */
  void setColorGradingTexture(BaseTexture* value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  bool colorGradingEnabled() const;

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  void setColorGradingEnabled(bool value);

  /**
   * @brief Gets exposure used in the effect.
   */
  float exposure() const;

  /**
   * @brief Sets exposure used in the effect.
   */
  void setExposure(float value);

  /**
   * @brief Gets wether tonemapping is enabled or not.
   */
  bool toneMappingEnabled() const;

  /**
   * @brief Sets wether tonemapping is enabled or not
   */
  void setToneMappingEnabled(bool value);

  /**
   * @brief Gets contrast used in the effect.
   */
  float contrast() const;

  /**
   * @brief Sets contrast used in the effect.
   */
  void setContrast(float value);

  /**
   * @brief Gets Vignette stretch size.
   */
  float vignetteStretch() const;

  /**
   * @brief Sets Vignette stretch size.
   */
  void setVignetteStretch(float value);

  /**
   * @brief Gets Vignette centre X Offset.
   */
  float vignetteCentreX() const;

  /**
   * @brief Sets Vignette centre X Offset.
   */
  void setVignetteCentreX(float value);

  /**
   * @brief Gets Vignette centre Y Offset.
   */
  float vignetteCentreY() const;

  /**
   * @brief Sets Vignette centre Y Offset.
   */
  void setVignetteCentreY(float value);

  /**
   * @brief Gets Vignette weight or intensity of the vignette effect.
   */
  float vignetteWeight() const;

  /**
   * @brief Sets Vignette weight or intensity of the vignette effect.
   */
  void setVignetteWeight(float value);

  /**
   * @brief Gets Color of the vignette applied on the screen through the chosen
   * blend mode (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  Color4& vignetteColor();

  /**
   * @brief Sets Color of the vignette applied on the screen through the chosen
   * blend mode (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  void setVignetteColor(const Color4& value);

  /**
   * @brief Gets Camera field of view used by the Vignette effect.
   */
  float vignetteCameraFov() const;

  /**
   * @brief Sets Camera field of view used by the Vignette effect.
   */
  void setVignetteCameraFov(float value);

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
   * @brief Gets wether the input of the processing is in Gamma or Linear Space.
   */
  bool fromLinearSpace() const;

  /**
   * @brief Sets wether the input of the processing is in Gamma or Linear Space.
   */
  void setFromLinearSpace(bool value);

  void dispose(Camera* camera = nullptr);

protected:
  /**
   * @brief Attaches a new image processing configuration to the PBR Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(
    ImageProcessingConfiguration* configuration, bool doNotBuild = false);

  void _updateParameters();

protected:
  /**
   * Default configuration related to image processing available in the PBR
   * Material.
   */
  ImageProcessingConfiguration* _imageProcessingConfiguration;

private:
  /**
   * Keep track of the image processing observer to allow dispose and replace.
   */
  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingObserver;
  bool _fromLinearSpace;

  /**
   * Defines cache preventing GC.
   */
  IImageProcessingConfigurationDefines _defines;

}; // end of class ImageProcessingPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H
