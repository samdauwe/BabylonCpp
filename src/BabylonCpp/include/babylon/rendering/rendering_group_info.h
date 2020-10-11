#ifndef BABYLON_RENDERING_RENDERING_GROUP_INFO_H
#define BABYLON_RENDERING_RENDERING_GROUP_INFO_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(Camera)

/**
 * @brief This class is used by the onRenderingGroupObservable.
 */
struct BABYLON_SHARED_EXPORT RenderingGroupInfo {
  /**
   * The Scene that being rendered
   */
  Scene* scene = nullptr;

  /**
   * The camera currently used for the rendering pass
   */
  CameraPtr camera = nullptr;

  /**
   * The ID of the renderingGroup being processed
   */
  int renderingGroupId = -1;
}; // end of class RenderingGroupInfo

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_RENDERING_GROUP_INFO_H
