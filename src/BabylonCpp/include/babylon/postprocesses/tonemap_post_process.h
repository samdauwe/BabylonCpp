#ifndef BABYLON_POSTPROCESSES_TONEMAP_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_TONEMAP_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief Defines a post process to apply tone mapping.
 */
class BABYLON_SHARED_EXPORT TonemapPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new TonemapPostProcess.
   * @param name defines the name of the postprocess
   * @param _operator defines the operator to use
   * @param exposureAdjustment defines the required exposure adjustement
   * @param camera defines the camera to use (can be null)
   * @param samplingMode defines the required sampling mode
   * (BABYLON.Texture.BILINEAR_SAMPLINGMODE by default)
   * @param engine defines the hosting engine (can be ignore if camera is set)
   * @param textureFormat defines the texture format to use
   * (BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT by default)
   */
  TonemapPostProcess(const std::string& name,
                     const TonemappingOperator& operator_,
                     float exposureAdjustment, const CameraPtr& camera,
                     unsigned int samplingMode
                     = TextureConstants::BILINEAR_SAMPLINGMODE,
                     Engine* engine = nullptr,
                     unsigned int textureFormat
                     = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  virtual ~TonemapPostProcess();

private:
  TonemappingOperator _operator;

}; // end of class TonemapPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_TONEMAP_POST_PROCESS_H
