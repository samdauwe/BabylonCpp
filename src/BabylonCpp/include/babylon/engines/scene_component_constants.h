#ifndef BABYLON_ENGINES_SCENE_COMPONENT_CONSTANTS_H
#define BABYLON_ENGINES_SCENE_COMPONENT_CONSTANTS_H

#include <babylon/babylon_api.h>

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
  static constexpr const char* NAME_SPRITE            = "Sprite";
  static constexpr const char* NAME_OUTLINERENDERER   = "Outline";
  static constexpr const char* NAME_PROCEDURALTEXTURE = "ProceduralTexture";
  static constexpr const char* NAME_SHADOWGENERATOR   = "ShadowGenerator";
  static constexpr const char* NAME_OCTREE            = "Octree";
  static constexpr const char* NAME_PHYSICSENGINE     = "PhysicsEngine";
  static constexpr const char* NAME_AUDIO             = "Audio";

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

  static constexpr const unsigned int STEP_BEFORERENDERTARGETDRAW_LAYER = 0;

  static constexpr const unsigned int STEP_BEFORERENDERINGMESH_OUTLINE = 0;

  static constexpr const unsigned int STEP_AFTERRENDERINGMESH_OUTLINE = 0;

  static constexpr const unsigned int
    STEP_AFTERRENDERINGGROUPDRAW_EFFECTLAYER_DRAW
    = 0;
  static constexpr const unsigned int
    STEP_AFTERRENDERINGGROUPDRAW_BOUNDINGBOXRENDERER
    = 1;

  static constexpr const unsigned int
    STEP_BEFORECAMERAUPDATE_SIMPLIFICATIONQUEUE
    = 0;
  static constexpr const unsigned int STEP_BEFORECAMERAUPDATE_GAMEPAD = 1;

  static constexpr const unsigned int STEP_BEFORECLEAR_PROCEDURALTEXTURE = 0;

  static constexpr const unsigned int STEP_AFTERRENDERTARGETDRAW_LAYER = 0;

  static constexpr const unsigned int STEP_AFTERCAMERADRAW_EFFECTLAYER     = 0;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_LENSFLARESYSTEM = 1;

  static constexpr const unsigned int STEP_AFTERCAMERADRAW_EFFECTLAYER_DRAW = 2;
  static constexpr const unsigned int STEP_AFTERCAMERADRAW_LAYER            = 3;

  static constexpr const unsigned int STEP_AFTERRENDER_AUDIO = 0;

  static constexpr const unsigned int STEP_GATHERRENDERTARGETS_SHADOWGENERATOR
    = 0;
  static constexpr const unsigned int
    STEP_GATHERRENDERTARGETS_GEOMETRYBUFFERRENDERER
    = 1;
  static constexpr const unsigned int STEP_GATHERRENDERTARGETS_DEPTHRENDERER
    = 2;
  static constexpr const unsigned int
    STEP_GATHERRENDERTARGETS_POSTPROCESSRENDERPIPELINEMANAGER
    = 3;

  static constexpr const unsigned int
    STEP_GATHERACTIVECAMERARENDERTARGETS_DEPTHRENDERER
    = 0;

  static constexpr const unsigned int STEP_POINTERMOVE_SPRITE = 0;
  static constexpr const unsigned int STEP_POINTERDOWN_SPRITE = 0;
  static constexpr const unsigned int STEP_POINTERUP_SPRITE   = 0;

}; // end of struct SceneComponentConstants

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SCENE_COMPONENT_CONSTANTS_H
