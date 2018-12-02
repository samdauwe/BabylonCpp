#ifndef BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_TRACKER_H
#define BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_TRACKER_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class IInternalTextureTracker;
using IInternalTextureTrackerPtr = std::shared_ptr<IInternalTextureTracker>;

/**
 * @brief Internal interface used to track InternalTexture already bound to the
 * GL context.
 */
struct BABYLON_SHARED_EXPORT IInternalTextureTracker {
  /**
   * Gets or set the previous tracker in the list
   */
  IInternalTextureTrackerPtr previous = nullptr;
  /**
   * Gets or set the next tracker in the list
   */
  IInternalTextureTrackerPtr next = nullptr;
}; // end of struct IInternalTextureTracker

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_TRACKER_H
