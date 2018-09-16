#ifndef BABYLON_SAMPLES_SPECIAL_FX_SPECIAL_FX_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_SPECIAL_FX_SPECIAL_FX_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT _SpecialFXSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Special FX";
  }

public:
  _SpecialFXSamplesIndex();
  virtual ~_SpecialFXSamplesIndex();

}; // end of class _SpecialFXSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_SPECIAL_FX_SAMPLES_INDEX_H
