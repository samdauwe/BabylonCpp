#ifndef BABYLON_ENGINE_RENDERING_GROUP_INFO_H
#define BABYLON_ENGINE_RENDERING_GROUP_INFO_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Camera;
class Scene;
using CameraPtr = std::shared_ptr<Camera>;

/**
 * @brief This class is used by the onRenderingGroupObservable
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
  unsigned int renderingGroupId;

}; // end of struct RenderingGroupInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_RENDERING_GROUP_INFO_H
