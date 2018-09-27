#ifndef BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_OPTIONS_H
#define BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_OPTIONS_H

#include <optional>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class PostProcess;

/**
 * @brief Parameters to perform the merge of the depth of field effect.
 */
struct DepthOfFieldOptions {
  PostProcess* circleOfConfusion = nullptr;
  std::vector<PostProcess*> blurSteps;
}; // end of struct DepthOfFieldOption

/**
 * @brief Parameters to perform the merge of bloom effect.
 */
struct BloomOptions {
  PostProcess* blurred = nullptr;
  float weight;
}; // end of struct DepthOfFieldOption

/**
 * @brief Options to be set when merging outputs from the default pipeline.
 */
struct BABYLON_SHARED_EXPORT DepthOfFieldMergePostProcessOptions {

  /**
   * The original image to merge on top of
   */
  PostProcess* originalFromInput = nullptr;

  /**
   * Parameters to perform the merge of the depth of field effect
   */
  std::optional<DepthOfFieldOptions> depthOfField = std::nullopt;

  /**
   * Parameters to perform the merge of bloom effect
   */
  std::optional<BloomOptions> bloom = std::nullopt;

}; // end of class DepthOfFieldMergePostProcessOptions

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_OPTIONS_H
