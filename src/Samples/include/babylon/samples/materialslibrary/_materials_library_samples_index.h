#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT _MaterialsLibrarySamplesIndex
    : public _ISamplesIndex {

private:
  static constexpr const char* _CategoryName = "Materials Library";

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return _MaterialsLibrarySamplesIndex::_CategoryName;
  }

public:
  _MaterialsLibrarySamplesIndex();
  virtual ~_MaterialsLibrarySamplesIndex();

}; // end of class _MaterialsLibrarySamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SAMPLES_INDEX_H
