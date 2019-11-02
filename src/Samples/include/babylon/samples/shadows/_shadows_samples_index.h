#ifndef BABYLON_SAMPLES_SHADOWS_SHADOWS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_SHADOWS_SHADOWS_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _ShadowsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Shadows";
  }

public:
  _ShadowsSamplesIndex();
  virtual ~_ShadowsSamplesIndex() = default;

}; // end of class _ShadowsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SHADOWS_SHADOWS_SAMPLES_INDEX_H
