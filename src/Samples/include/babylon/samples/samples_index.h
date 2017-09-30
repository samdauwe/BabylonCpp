#ifndef BABYLON_SAMPLES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_SAMPLES_INDEX_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT SamplesIndex {

public:
  using IRenderableScenePtr = ::std::unique_ptr<IRenderableScene>;
  using FactoryMethod = ::std::function<IRenderableScenePtr(ICanvas* iCanvas)>;
  using Sample        = ::std::tuple<bool, FactoryMethod>;

public:
  SamplesIndex();
  ~SamplesIndex();

  /**
   * @brief Returns whether or not the example is enabled.
   * @param sampleName the name of the sample to check
   * @return whether or not the example is enabled
   */
  bool isSampleEnabled(const string_t& sampleName);

  /**
   * @brief Returns whether or not the example is exists.
   * @param sampleName the name of the sample to check
   * @return whether or not the example is exists
   */
  bool sampleExists(const string_t& sampleName);

  /**
   * @brief Returns a sorted list containing the enabled examples.
   * @return a sorted list containing the enabled examples.
   */
  ::std::vector<string_t> getSampleNames();

  /**
   * @brief Sample factory method
   * @param sampleName the name of the sample to create
   * @param iCanvas the canves to be used
   * @return the renderable scene
   */
  std::unique_ptr<IRenderableScene>
  createRenderableScene(const string_t& sampleName, ICanvas* iCanvas);

private:
  // Contains the mapping from sample name to enabled state
  ::std::unordered_map<string_t, Sample> _samples;

}; // end of class SamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SAMPLES_INDEX_H
