#ifndef BABYLON_SAMPLES_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_SAMPLES_INDEX_H

#include <babylon/babylon_api.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT SamplesIndex {

public:
  SamplesIndex();
  ~SamplesIndex();

  /**
   * @brief Returns whether or not the example is enabled.
   * @param sampleName the name of the sample to check
   * @return whether or not the example is enabled
   */
  bool isSampleEnabled(const std::string& sampleName);

  /**
   * @brief Returns whether or not the example exists.
   * @param sampleName the name of the sample to check
   * @return whether or not the example exists
   */
  bool sampleExists(const std::string& sampleName);

  /**
   * @brief Returns a sorted list containing the enabled examples.
   * @return a sorted list containing the enabled examples.
   */
  ::std::vector<std::string> getSampleNames() const;

  /**
   * @brief Returns a sorted list containing the category names.
   * @return a sorted list containing the category names.
   */
  ::std::vector<std::string> getCategoryNames() const;

  /**
   * @brief Returns whether or not the category exists.
   * @param categoryNameToSearch the name of the category to check
   * @return whether or not the category exists
   */
  bool categoryExists(const std::string& categoryNameToSearch);

  /**
   * @brief Returns a sorted list containing the enabled examples in the
   * specified category.
   * @return a sorted list containing the enabled examples in the specified
   * category.
   */
  ::std::vector<std::string>
  getSampleNamesInCategory(const std::string& categoryName) const;

  /**
   * @brief Sample factory method
   * @param sampleName the name of the sample to create
   * @param iCanvas the canves to be used
   * @return the renderable scene
   */
  std::unique_ptr<IRenderableScene>
  createRenderableScene(const std::string& sampleName, ICanvas* iCanvas) const;

private:
  // Contains the mapping from category to samples index
  ::std::unordered_map<std::string, _ISamplesIndex> _samplesIndex;

}; // end of class SamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SAMPLES_INDEX_H
