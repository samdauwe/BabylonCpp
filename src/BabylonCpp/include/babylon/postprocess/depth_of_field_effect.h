#ifndef BABYLON_POSTPROCESS_DEPTH_OF_FIELD_EFFECT_H
#define BABYLON_POSTPROCESS_DEPTH_OF_FIELD_EFFECT_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/depth_of_field_effect_blur_level.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>

namespace BABYLON {

/**
 * @brief The depth of field effect applies a blur to objects that are closer or
 * further from where the camera is focusing.
 */
class BABYLON_SHARED_EXPORT DepthOfFieldEffect
    : public PostProcessRenderEffect {

public:
  /**
   * @brief Creates a new instance of @see DepthOfFieldEffect
   * @param scene The scene the effect belongs to.
   * @param pipelineTextureType The type of texture to be used when performing
   * the post processing.
   */
  DepthOfFieldEffect(Scene* scene,
                     DepthOfFieldEffectBlurLevel blurLevel
                     = DepthOfFieldEffectBlurLevel::Low,
                     unsigned int pipelineTextureType = 0);
  ~DepthOfFieldEffect();

  /**
   * @brief The focal the length of the camera used in the effect
   */
  void setFocalLength(float value);
  float focalLength() const;

  /**
   * @brief F-Stop of the effect's camera. The diamater of the resulting
   * aperture can be computed by lensSize/fStop. (default: 1.4)
   */
  void setFStop(float value);
  float fStop() const;

  /**
   * @brief Distance away from the camera to focus on in scene units/1000 (eg.
   * millimeter). (default: 2000)
   */
  void setFocusDistance(float value);
  float focusDistance() const;

  /**
   * @brief Max lens size in scene units/1000 (eg. millimeter). Standard cameras
   * are 50mm. (default: 50) The diamater of the resulting aperture can be
   * computed by lensSize/fStop.
   */
  void setLensSize(float value);
  float lensSize() const;

  /**
   * @brief Disposes each of the internal effects for a given camera.
   * @param camera The camera to dispose the effect on.
   */
  void disposeEffects(Camera* camera);

private:
  unique_ptr_t<PassPostProcess> _depthOfFieldPass;
  unique_ptr_t<CircleOfConfusionPostProcess> _circleOfConfusion;
  std::vector<unique_ptr_t<DepthOfFieldBlurPostProcess>> _depthOfFieldBlurX;
  std::vector<unique_ptr_t<DepthOfFieldBlurPostProcess>> _depthOfFieldBlurY;
  unique_ptr_t<DepthOfFieldMergePostProcess> _depthOfFieldMerge;

}; // end of class ConvolutionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_DEPTH_OF_FIELD_EFFECT_H
