#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_OPTIMIZATIONS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_OPTIMIZATIONS_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _OptimizationsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Optimizations";
  }

public:
  _OptimizationsSamplesIndex();
  virtual ~_OptimizationsSamplesIndex();

}; // end of class _OptimizationsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_OPTIMIZATIONS_SAMPLES_INDEX_H
