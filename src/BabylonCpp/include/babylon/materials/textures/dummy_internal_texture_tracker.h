#ifndef BABYLON_MATERIALS_TEXTURES_DUMMY_INTERNAL_TEXTURE_TRACKER_H
#define BABYLON_MATERIALS_TEXTURES_DUMMY_INTERNAL_TEXTURE_TRACKER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Internal class used by the engine to get list of
 * {BABYLON.InternalTexture} already bound to the GL context.
 */
struct BABYLON_SHARED_EXPORT DummyInternalTextureTracker {
  /**
   * Gets or set the previous tracker in the list
   */
  IInternalTextureTracker* previous = nullptr;
  /**
   * Gets or set the next tracker in the list
   */
  IInternalTextureTracker* next = nullptr;
}; // end of struct DummyInternalTextureTracker

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_DUMMY_INTERNAL_TEXTURE_TRACKER_H
