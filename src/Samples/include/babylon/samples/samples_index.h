#ifndef BABYLON_SAMPLES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_SAMPLES_INDEX_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Samples {

struct Sample {
  using IRenderableScenePtr = std::unique_ptr<IRenderableScene>;
  bool enabled              = false;
  std::function<IRenderableScenePtr(ICanvas* iCanvas)> factoryMethod = nullptr;
}; // end of struct Sample

struct SamplesIndex {

  // Contains the mapping from sample name to enabled state
  static std::unordered_map<std::string, Sample> Samples;

  /**
   * @brief Returns whether or not the example is enabled.
   * @param sampleName the name of the sample to check
   * @return whether or not the example is enabled
   */
  static bool isSampleEnabled(const std::string& sampleName);

  /**
   * @brief Returns whether or not the example is exists.
   * @param sampleName the name of the sample to check
   * @return whether or not the example is exists
   */
  static bool sampleExists(const std::string& sampleName);

  /**
   * @brief Returns a sorted list containing the enabled examples.
   * @return a sorted list containing the enabled examples.
   */
  static std::vector<std::string> getSampleNames();

  /**
   * @brief Sample factory method
   * @param sampleName the name of the sample to create
   * @param iCanvas the canves to be used
   * @return the renderable scene
   */
  static std::unique_ptr<IRenderableScene>
  createRenderableScene(const std::string& sampleName, ICanvas* iCanvas);

}; // end of struct RotationAndScalingScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SAMPLES_INDEX_H
