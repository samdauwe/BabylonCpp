#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _ProceduralTexturesLibrarySamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return "Procedural Textures Library";
  }

public:
  _ProceduralTexturesLibrarySamplesIndex();
  virtual ~_ProceduralTexturesLibrarySamplesIndex();

}; // end of class _ProceduralTexturesLibrarySamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
