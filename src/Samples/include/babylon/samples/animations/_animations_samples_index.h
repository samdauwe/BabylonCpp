#ifndef BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _AnimationsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Animations";
  }

public:
  _AnimationsSamplesIndex();
  virtual ~_AnimationsSamplesIndex();

}; // end of class _AnimationsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_ANIMATIONS_SAMPLES_INDEX_H
