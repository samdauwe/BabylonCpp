#ifndef BABYLON_POSTPROCESSES_IMAGE_PROCESSING_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_IMAGE_PROCESSING_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class BaseTexture;
class ColorCurves;
class ImageProcessingConfiguration;
class ImageProcessingPostProcess;
using BaseTexturePtr                = std::shared_ptr<BaseTexture>;
using ImageProcessingPostProcessPtr = std::shared_ptr<ImageProcessingPostProcess>;

/**
 * @brief ImageProcessingPostProcess
 * @see
 * https://doc.babylonjs.com/how_to/how_to_use_postprocesses#imageprocessing
 */
class BABYLON_SHARED_EXPORT ImageProcessingPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static ImageProcessingPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<ImageProcessingPostProcess>(
      new ImageProcessingPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~ImageProcessingPostProcess() override; // = default

  /**
   * "ImageProcessingPostProcess"
   * @returns "ImageProcessingPostProcess"
   */
  [[nodiscard]] std::string getClassName() const override;

  void dispose(Camera* camera = nullptr) override;

protected:
  ImageProcessingPostProcess(const std::string& name, float renderRatio, const CameraPtr& camera,
                             unsigned int samplingMode, Engine* engine, bool reusable = false,
                             unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                             ImageProcessingConfiguration* imageProcessingConfiguration = nullptr);

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
   * @brief Gets Color curves setup used in the effect if colorCurvesEnabled is
   * set to true .
   */
  std::shared_ptr<ColorCurves>& get_colorCurves();

  /**
   * Sets Color curves setup used in the effect if colorCurvesEnabled is set to
   * true .
   */
  void set_colorCurves(const std::shared_ptr<ColorCurves>& value);

  /**
   * @brief Gets wether the color curves effect is enabled.
   */
  [[nodiscard]] bool get_colorCurvesEnabled() const;

  /**
   * Sets wether the color curves effect is enabled.
   */
  void set_colorCurvesEnabled(bool value);

  /**
   * @brief Gets Color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */
  BaseTexturePtr& get_colorGradingTexture();

  /**
   * @brief Sets Color grading LUT texture used in the effect if
   * colorGradingEnabled is set to true.
   */
  void set_colorGradingTexture(const BaseTexturePtr& value);

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  [[nodiscard]] bool get_colorGradingEnabled() const;

  /**
   * @brief Gets wether the color grading effect is enabled.
   */
  void set_colorGradingEnabled(bool value);

  /**
   * @brief Gets exposure used in the effect.
   */
  [[nodiscard]] float get_exposure() const;

  /**
   * @brief Sets exposure used in the effect.
   */
  void set_exposure(float value);

  /**
   * @brief Gets wether tonemapping is enabled or not.
   */
  [[nodiscard]] bool get_toneMappingEnabled() const;

  /**
   * @brief Sets wether tonemapping is enabled or not
   */
  void set_toneMappingEnabled(bool value);

  /**
   * @brief Gets the type of tone mapping effect.
   */
  [[nodiscard]] unsigned int get_toneMappingType() const;

  /**
   * @brief Sets the type of tone mapping effect.
   */
  void set_toneMappingType(unsigned int value);

  /**
   * @brief Gets contrast used in the effect.
   */
  [[nodiscard]] float get_contrast() const;

  /**
   * @brief Sets contrast used in the effect.
   */
  void set_contrast(float value);

  /**
   * @brief Gets Vignette stretch size.
   */
  [[nodiscard]] float get_vignetteStretch() const;

  /**
   * @brief Sets Vignette stretch size.
   */
  void set_vignetteStretch(float value);

  /**
   * @brief Gets Vignette centre X Offset.
   */
  [[nodiscard]] float get_vignetteCentreX() const;

  /**
   * @brief Sets Vignette centre X Offset.
   */
  void set_vignetteCentreX(float value);

  /**
   * @brief Gets Vignette centre Y Offset.
   */
  [[nodiscard]] float get_vignetteCentreY() const;

  /**
   * @brief Sets Vignette centre Y Offset.
   */
  void set_vignetteCentreY(float value);

  /**
   * @brief Gets Vignette weight or intensity of the vignette effect.
   */
  [[nodiscard]] float get_vignetteWeight() const;

  /**
   * @brief Sets Vignette weight or intensity of the vignette effect.
   */
  void set_vignetteWeight(float value);

  /**
   * @brief Gets Color of the vignette applied on the screen through the chosen
   * blend mode (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  Color4& get_vignetteColor();

  /**
   * @brief Sets Color of the vignette applied on the screen through the chosen
   * blend mode (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  void set_vignetteColor(const Color4& value);

  /**
   * @brief Gets Camera field of view used by the Vignette effect.
   */
  [[nodiscard]] float get_vignetteCameraFov() const;

  /**
   * @brief Sets Camera field of view used by the Vignette effect.
   */
  void set_vignetteCameraFov(float value);

  /**
   * @brief Gets the vignette blend mode allowing different kind of effect.
   */
  [[nodiscard]] unsigned int get_vignetteBlendMode() const;

  /**
   * @brief Sets the vignette blend mode allowing different kind of effect.
   */
  void set_vignetteBlendMode(unsigned int value);

  /**
   * @brief Gets wether the vignette effect is enabled.
   */
  [[nodiscard]] bool get_vignetteEnabled() const;

  /**
   * @brief Sets wether the vignette effect is enabled.
   */
  void set_vignetteEnabled(bool value);

  /**
   * @brief Gets wether the input of the processing is in Gamma or Linear Space.
   */
  [[nodiscard]] bool get_fromLinearSpace() const;

  /**
   * @brief Sets wether the input of the processing is in Gamma or Linear Space.
   */
  void set_fromLinearSpace(bool value);

  /**
   * @brief Attaches a new image processing configuration to the PBR Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(ImageProcessingConfiguration* configuration,
                                           bool doNotBuild = false);

  void _updateParameters();

public:
  /**
   * Image processing configuration used either in this material.
   */
  Property<ImageProcessingPostProcess, ImageProcessingConfiguration*> imageProcessingConfiguration;

  /**
   * Color curves setup used in the effect if colorCurvesEnabled is set to true
   * .
   */
  Property<ImageProcessingPostProcess, std::shared_ptr<ColorCurves>> colorCurves;

  /**
   * Wether the color curves effect is enabled.
   */
  Property<ImageProcessingPostProcess, bool> colorCurvesEnabled;

  /**
   * Color grading LUT texture used in the effect if colorGradingEnabled is set
   * to true.
   */
  Property<ImageProcessingPostProcess, BaseTexturePtr> colorGradingTexture;

  /**
   * Wether the color grading effect is enabled.
   */
  Property<ImageProcessingPostProcess, bool> colorGradingEnabled;

  /**
   * Exposure used in the effect.
   */
  Property<ImageProcessingPostProcess, float> exposure;

  /**
   * Wether tonemapping is enabled or not.
   */
  Property<ImageProcessingPostProcess, bool> toneMappingEnabled;

  /**
   * The type of tone mapping effect
   */
  Property<ImageProcessingPostProcess, unsigned int> toneMappingType;

  /**
   * Contrast used in the effect.
   */
  Property<ImageProcessingPostProcess, float> contrast;

  /**
   * Vignette stretch size.
   */
  Property<ImageProcessingPostProcess, float> vignetteStretch;

  /**
   * Vignette centre X Offset.
   */
  Property<ImageProcessingPostProcess, float> vignetteCentreX;

  /**
   * Vignette centre Y Offset.
   */
  Property<ImageProcessingPostProcess, float> vignetteCentreY;

  /**
   * Vignette weight or intensity of the vignette effect.
   */
  Property<ImageProcessingPostProcess, float> vignetteWeight;

  /**
   * Color of the vignette applied on the screen through the chosen blend mode
   * (vignetteBlendMode) if vignetteEnabled is set to true.
   */
  Property<ImageProcessingPostProcess, Color4> vignetteColor;

  /**
   * Gets Camera field of view used by the Vignette effect.
   */
  Property<ImageProcessingPostProcess, float> vignetteCameraFov;

  /**
   * Gets the vignette blend mode allowing different kind of effect.
   */
  Property<ImageProcessingPostProcess, unsigned int> vignetteBlendMode;

  /**
   * Wether the vignette effect is enabled.
   */
  Property<ImageProcessingPostProcess, bool> vignetteEnabled;

  /**
   * Wether the input of the processing is in Gamma or Linear Space.
   */
  Property<ImageProcessingPostProcess, bool> fromLinearSpace;

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

#endif // end of BABYLON_POSTPROCESSES_IMAGE_PROCESSING_POST_PROCESS_H
