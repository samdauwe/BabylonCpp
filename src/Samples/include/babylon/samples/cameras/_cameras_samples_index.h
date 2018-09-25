#ifndef BABYLON_SAMPLES_CAMERAS_CAMERAS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_CAMERAS_CAMERAS_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _CamerasSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Cameras";
  }

public:
  _CamerasSamplesIndex();
  virtual ~_CamerasSamplesIndex();

}; // end of class _CamerasSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_CAMERAS_CAMERAS_SAMPLES_INDEX_H
