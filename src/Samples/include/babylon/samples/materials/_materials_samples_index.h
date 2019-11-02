#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _MaterialsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Materials";
  }

public:
  _MaterialsSamplesIndex();
  virtual ~_MaterialsSamplesIndex() = default;

}; // end of class _MaterialsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_MATERIALS_SAMPLES_INDEX_H
