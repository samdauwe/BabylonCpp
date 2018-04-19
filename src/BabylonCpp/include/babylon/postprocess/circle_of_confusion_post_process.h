#ifndef BABYLON_POSTPROCESS_CIRCLE_OF_CONFUSION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_CIRCLE_OF_CONFUSION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The CircleOfConfusionPostProcess computes the circle of confusion
 * value for each pixel given required lens parameters. See
 * https://en.wikipedia.org/wiki/Circle_of_confusion
 */
class BABYLON_SHARED_EXPORT CircleOfConfusionPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new instance CircleOfConfusionPostProcess.
   * @param name The name of the effect.
   * @param depthTexture The depth texture of the scene to compute the circle of
   * confusion. This must be set in order for this to function but may be set
   * after initialization if needed.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType Type of textures used when performing the post process.
   * (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  CircleOfConfusionPostProcess(
    const string_t& name, RenderTargetTexture* depthTexture,
    const Variant<float, PostProcessOptions>& options, Camera* camera,
    unsigned int samplingMode = 0, Engine* engine = nullptr,
    bool reusable            = false,
    unsigned int textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    bool blockCompilation    = false);
  ~CircleOfConfusionPostProcess();

protected:
  /**
   * @brief Depth texture to be used to compute the circle of confusion. This
   * must be set here or in the constructor in order for the post process to
   * function.
   */
  void set_depthTexture(RenderTargetTexture* const& value);

public:
  /**
   * Max lens size in scene units/1000 (eg. millimeter). Standard cameras are
   * 50mm. (default: 50) The diamater of the resulting aperture can be computed
   * by lensSize/fStop.
   */
  float lensSize;

  /**
   * F-Stop of the effect's camera. The diamater of the resulting aperture can
   * be computed by lensSize/fStop. (default: 1.4)
   */
  float fStop;

  /**
   * Distance away from the camera to focus on in scene units/1000 (eg.
   * millimeter). (default: 2000)
   */
  float focusDistance;

  /**
   * Focal length of the effect's camera in scene units/1000 (eg. millimeter).
   * (default: 50)
   */
  float focalLength;

  /**
   * Depth texture to be used to compute the circle of confusion. This must be
   * set here or in the constructor in order for the post process to function.
   */
  WriteOnlyProperty<CircleOfConfusionPostProcess, RenderTargetTexture*>
    depthTexture;

private:
  RenderTargetTexture* _depthTexture;

}; // end of class DepthOfFieldMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_CIRCLE_OF_CONFUSION_POST_PROCESS_H
