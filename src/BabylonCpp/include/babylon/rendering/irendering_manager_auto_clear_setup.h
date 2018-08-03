#ifndef BABYLON_RENDERING_IRENDERING_MANAGER_AUTO_CLEAR_SETUP_H
#define BABYLON_RENDERING_IRENDERING_MANAGER_AUTO_CLEAR_SETUP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Interface describing the different options available in the rendering
 * manager regarding Auto Clear between groups.
 */
struct BABYLON_SHARED_EXPORT IRenderingManagerAutoClearSetup {
  /**
   * Defines whether or not autoclear is enable.
   */
  bool autoClear;
  /**
   * Defines whether or not to autoclear the depth buffer.
   */
  bool depth;
  /**
   * Defines whether or not to autoclear the stencil buffer.
   */
  bool stencil;
}; // end of struct IRenderingManagerAutoClearSetup

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_IRENDERING_MANAGER_AUTO_CLEAR_SETUP_H