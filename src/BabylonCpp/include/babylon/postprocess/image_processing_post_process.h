#ifndef BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H
#define BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
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

  BaseTexture* colorGradingTexture();
  void setColorGradingTexture(BaseTexture* value);
  unsigned int vignetteBlendMode() const;
  void setVignetteBlendMode(unsigned int value);
  bool colorCurvesEnabled() const;
  void setColorCurvesEnabled(bool value);
  bool vignetteEnabled() const;
  void setVignetteEnabled(bool value);
  bool fromLinearSpace() const;
  void setFromLinearSpace(bool value);
  bool cameraToneMappingEnabled() const;
  void setCameraToneMappingEnabled(bool value);

private:
  void _updateParameters();
  // Statics
  static constexpr unsigned int VIGNETTEMODE_MULTIPLY = 0;
  static constexpr unsigned int VIGNETTEMODE_OPAQUE   = 1;

public:
  float colorGradingWeight;
  ColorCurves* colorCurves;
  float cameraFov;
  float vignetteStretch;
  float vignetteCentreX;
  float vignetteCentreY;
  float vignetteWeight;
  Color4 vignetteColor;
  float cameraContrast;
  float cameraExposure;

private:
  BaseTexture* _colorGradingTexture;
  bool _colorCurvesEnabled;
  unsigned int _vignetteBlendMode;
  bool _vignetteEnabled;
  bool _cameraToneMappingEnabled;
  bool _fromLinearSpace;

}; // end of class ImageProcessingPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_IMAGE_PROCESSING_POST_PROCESS_H
