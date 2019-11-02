#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _ProceduralTexturesLibrarySamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Procedural Textures Library";
  }

public:
  _ProceduralTexturesLibrarySamplesIndex();
  virtual ~_ProceduralTexturesLibrarySamplesIndex() = default;

}; // end of class _ProceduralTexturesLibrarySamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
