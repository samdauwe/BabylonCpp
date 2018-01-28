#ifndef BABYLON_ENGINE_RENDERING_GROUP_INFO_H
#define BABYLON_ENGINE_RENDERING_GROUP_INFO_H

#include <babylon/babylon_global.h>

namespace BABYLON {

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
  Camera* camera = nullptr;

  /**
   * The ID of the renderingGroup being processed
   */
  unsigned int renderingGroupId;

  /**
   * The rendering stage, can be either STAGE_PRECLEAR, STAGE_PREOPAQUE,
   * STAGE_PRETRANSPARENT, STAGE_POSTTRANSPARENT
   */
  unsigned int renderStage;

  /**
   * Stage corresponding to the very first hook in the renderingGroup phase:
   * before the render buffer may be cleared
   * This stage will be fired no matter what
   */
  static constexpr unsigned int STAGE_PRECLEAR = 1;

  /**
   * Called before opaque object are rendered.
   * This stage will be fired only if there's 3D Opaque content to render
   */
  static constexpr unsigned int STAGE_PREOPAQUE = 2;

  /**
   * Called after the opaque objects are rendered and before the transparent
   * ones
   * This stage will be fired only if there's 3D transparent content to render
   */
  static constexpr unsigned int STAGE_PRETRANSPARENT = 3;

  /**
   * Called after the transparent object are rendered, last hook of the
   * renderingGroup phase
   * This stage will be fired no matter what
   */
  static constexpr unsigned int STAGE_POSTTRANSPARENT = 4;

}; // end of struct RenderingGroupInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_RENDERING_GROUP_INFO_H
