#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _MaterialsLibrarySamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Materials Library";
  }

public:
  _MaterialsLibrarySamplesIndex();
  virtual ~_MaterialsLibrarySamplesIndex();

}; // end of class _MaterialsLibrarySamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H
