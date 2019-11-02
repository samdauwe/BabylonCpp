#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_LOADERS_BABYLON_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_LOADERS_BABYLON_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _LoadersBabylonSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Loaders - babylon format";
  }

public:
  _LoadersBabylonSamplesIndex();
  virtual ~_LoadersBabylonSamplesIndex() = default;

}; // end of class _LoadersBabylonSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_LOADERS_BABYLON_SAMPLES_INDEX_H
