#ifndef BABYLON_ENGINE_SCENE_COMPONENT_CONSTANTS_H
#define BABYLON_ENGINE_SCENE_COMPONENT_CONSTANTS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Groups all the scene component constants in one place to ease
 * maintenance.
 */
struct BABYLON_SHARED_EXPORT SceneComponentConstants {

  static constexpr const char* NAME_EFFECTLAYER         = "EffectLayer";
  static constexpr const char* NAME_LAYER               = "Layer";
  static constexpr const char* NAME_LENSFLARESYSTEM     = "LensFlareSystem";
  static constexpr const char* NAME_BOUNDINGBOXRENDERER = "BoundingBoxRenderer";
  static constexpr const char* NAME_PARTICLESYSTEM      = "ParticleSystem";
  static constexpr const char* NAME_GAMEPAD             = "Gamepad";
  static constexpr const char* NAME_SIMPLIFICATIONQUEUE = "SimplificationQueue";
  static constexpr const char* NAME_GEOMETRYBUFFERRENDERER
    = "GeometryBufferRenderer";
  static constexpr const char* NAME_DEPTHRENDERER = "DepthRenderer";
  static constexpr const char* NAME_POSTPROCESSRENDERPIPELINEMANAGER
    = "PostProcessRenderPipelineManager";

  static constexpr const unsigned int STEP_ISREADYFORMESH_EFFECTLAYER = 0;

  static constexpr const unsigned int
    STEP_BEFOREEVALUATEACTIVEMESH_BOUNDINGBOXRENDERER
    = 0;

  static constexpr const unsigned int STEP_EVALUATESUBMESH_BOUNDINGBOXRENDERER
    = 0;

  static constexpr const unsigned int STEP_ACTIVEMESH_BOUNDINGBOXRENDERER = 0;

  static constexpr const unsigned int STEP_CAMERADRAWRENDERTARGET_EFFECTLAYER
    = 1;

  static constexpr const unsigned int STEP_BEFORECAMERADRAW_EFFECTLAYER = 0;
  static constexpr const unsigned int STEP_BEFORECAMERADRAW_LAYER       = 1;

  static constexpr const unsigned int
    STEP_AFTERRENDERINGGROUPDRAW_EFFECTLAYER_DRAW
    = 0;

  static constexpr const unsigned int
    STEP_BEFORECAMERAUPDATE_SIMPLIFICATIONQUEUE
    = 0;
  static constexpr const unsigned int STEP_BEFORECAMERAUPDATE_GAMEPAD = 1;

  static constexpr const unsigned int STEP_AFTERCAMERADRAW_EFFECTLAYER     = 0;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_LENSFLARESYSTEM = 1;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_BOUNDINGBOXRENDERER
    = 2;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_EFFECTLAYER_DRAW = 3;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_LAYER            = 4;

  static constexpr const unsigned int
    STEP_GATHERRENDERTARGETS_GEOMETRYBUFFERRENDERER
    = 0;
  static constexpr const unsigned int STEP_GATHERRENDERTARGETS_DEPTHRENDERER
    = 1;
  static constexpr const unsigned int
    STEP_GATHERRENDERTARGETS_POSTPROCESSRENDERPIPELINEMANAGER
    = 2;

  static constexpr const unsigned int
    STEP_REBUILDGEOMETRY_POSTPROCESSRENDERPIPELINEMANAGER
    = 0;

}; // end of struct SceneComponentConstants

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_SCENE_COMPONENT_CONSTANTS_H
