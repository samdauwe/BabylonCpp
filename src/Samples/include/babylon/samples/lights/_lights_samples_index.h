#ifndef BABYLON_SAMPLES_LIGHTS_LIGHTS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_LIGHTS_LIGHTS_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT _LightsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Lights";
  }

public:
  _LightsSamplesIndex();
  virtual ~_LightsSamplesIndex();

}; // end of class _LightsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LIGHTS_LIGHTS_SAMPLES_INDEX_H
