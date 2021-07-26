#ifndef BABYLON_LIGHTS_ISORTABLE_LIGHT_H
#define BABYLON_LIGHTS_ISORTABLE_LIGHT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the common interface of sortable lights.
 */
struct BABYLON_SHARED_EXPORT ISortableLight {
  /**
   * Gets or sets whether or not the shadows are enabled for this light. This can help turning
   * off/on shadow without detaching the current shadow generator.
   */
  bool shadowEnabled;
  /**
   * Defines the rendering priority of the lights. It can help in case of fallback or number of
   * lights exceeding the number allowed of the materials.
   */
  int renderPriority;
}; // end of struct ISortableLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_ISORTABLE_LIGHT_H
