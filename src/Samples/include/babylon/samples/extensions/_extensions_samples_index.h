#ifndef BABYLON_SAMPLES_EXTENSIONS_EXTENSIONS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_EXTENSIONS_EXTENSIONS_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _ExtensionsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Extensions";
  }

public:
  _ExtensionsSamplesIndex();
  virtual ~_ExtensionsSamplesIndex();

}; // end of class _ExtensionsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_EXTENSIONS_EXTENSIONS_SAMPLES_INDEX_H
