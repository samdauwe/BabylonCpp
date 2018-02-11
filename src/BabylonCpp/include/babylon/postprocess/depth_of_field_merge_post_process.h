#ifndef BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The DepthOfFieldMergePostProcess merges blurred images with the
 * original based on the values of the circle of confusion.
 */
struct BABYLON_SHARED_EXPORT DepthOfFieldMergePostProcess : public PostProcess {

  /**
   * @brief Creates a new instance of @see CircleOfConfusionPostProcess
   * @param name The name of the effect.
   * @param original The non-blurred image to be modified
   * @param circleOfConfusion The circle of confusion post process that will
   * determine how blurred each pixel should become.
   * @param blurSteps Incrimental bluring post processes.
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
   */
  DepthOfFieldMergePostProcess(
    const string_t& name, PostProcess* original, PostProcess* circleOfConfusion,
    const vector_t<PostProcess*>& blurSteps,
    const Variant<float, PostProcessOptions>& options, Camera* camera,
    unsigned int samplingMode = 0, Engine* engine = nullptr,
    bool reusable            = false,
    unsigned int textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~DepthOfFieldMergePostProcess();

}; // end of class DepthOfFieldMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
