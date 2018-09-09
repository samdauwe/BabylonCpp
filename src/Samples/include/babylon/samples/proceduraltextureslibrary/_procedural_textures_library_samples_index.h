#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class BABYLON_SHARED_EXPORT _ProceduralTexturesLibrarySamplesIndex
    : public _ISamplesIndex {

private:
  static constexpr const char* _CategoryName = "Procedural Textures Library";

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const string_t CategoryName()
  {
    return _ProceduralTexturesLibrarySamplesIndex::_CategoryName;
  }

public:
  _ProceduralTexturesLibrarySamplesIndex();
  virtual ~_ProceduralTexturesLibrarySamplesIndex();

}; // end of class _ProceduralTexturesLibrarySamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_PROCEDURAL_TEXTURES_LIBRARY_PROCEDURAL_TEXTURES_LIBRARY_SAMPLES_INDEX_H
