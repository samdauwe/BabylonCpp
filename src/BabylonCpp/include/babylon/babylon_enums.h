#ifndef BABYLON_ENUMS_H
#define BABYLON_ENUMS_H

#include <cstddef>

namespace BABYLON {

struct EnumClassHashFunction {
  template <typename T>
  std::size_t operator()(T t) const
  {
    return static_cast<std::size_t>(t);
  }
};

/** Animation types **/

/**
 * @brief Enum for the animation key frame interpolation type.
 */
enum class AnimationKeyInterpolation {
  /**
   * Do not interpolate between keys and use the start key value only. Tangents
   * are ignored
   */
  STEP = 1
}; // end of enum class AnimationKeyInterpolation

/**
 * @brief Event types
 */
enum class EventType {
  CONTEXT_MENU      = 0,
  MOUSE_BUTTON_DOWN = 1,
  MOUSE_BUTTON_UP   = 2,
  MOUSE_OUT         = 3,
  MOUSE_MOVE        = 4,
  MOUSE_WHEEL       = 5,
  DOM_MOUSE_SCROLL  = 6,
  KEY_DOWN          = 7,
  KEY_UP            = 8,
  BLUR              = 9,
  UNKNOWN           = 10,
}; // end of enum class EventType

/**
 * @brief Mouse button types
 */
enum class MouseButtonType {
  UNDEFINED = -1,
  LEFT      = 0,
  MIDDLE    = 1,
  RIGHT     = 2
}; // end of enum class MouseButtonType

/**
 * @brief Pointer types
 */
enum class PointerType {
  UNDEFINED = -1,
  MOUSE     = 0,
  TOUCH     = 1,
}; // end of enum class MouseButtonType

/**
 * @brief Property types
 */
enum class PropertyType : unsigned int {
  ROTATION = 100
}; // end of enum class PropertyType

/**
 * @brief Defines supported task for worker process.
 */
enum class WorkerTaskType {
  /** Initialization */
  INIT = 0,
  /** Update of geometry */
  UPDATE = 1,
  /** Evaluate collision */
  COLLIDE = 2
}; // end of enum class WorkerTaskType

/**
 * @brief Defines kind of replies returned by worker
 */
enum class WorkerReplyType {
  /** Success */
  SUCCESS = 0,
  /** Unkown error */
  UNKNOWN_ERROR = 1
}; // end of enum class WorkerReplyType

/**
 * @brief Defines operator used for tonemapping
 */
enum class TonemappingOperator {
  /** Hable */
  HABLE = 0,
  /** Reinhard */
  REINHARD = 1,
  /** HejiDawson */
  HEJIDAWSON = 2,
  /** Photographic */
  PHOTOGRAPHIC = 3,
}; // end of enum class TonemappingOperator

/** Math Enums **/

/**
 * @brief Defines supported spaces.
 */
enum class Space {
  /** Local (object) space */
  LOCAL = 0,
  /** World space */
  WORLD = 1,
  /** Bone space */
  BONE = 2
}; // end of enum class Space

/**
 * @brief Defines potential orientation for back face culling.
 */
enum class Orientation {
  /** Clockwise */
  CW = 0,
  /** Counter clockwise */
  CCW = 1
}; // end of enum class Orientation

/** Mesh Enums **/

/**
 * @brief The implemented types of simplification.
 * At the moment only Quadratic Error Decimation is implemented
 * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
 */
enum class SimplificationType {
  /** Quadratic error decimation */
  QUADRATIC
}; // end of enum class SimplificationType

enum class Type : unsigned int {
  NODE  = 0,
  SCENE = 1,
  // Bones
  BONE     = 2,
  SKELETON = 3,
  // Cameras
  CAMERA                  = 10,
  ARCFOLLOWCAMERA         = 11,
  ARCROTATECAMERA         = 12,
  DEVICEORIENTATIONCAMERA = 13,
  FOLLOWCAMERA            = 14,
  FREECAMERA              = 15,
  TARGETCAMERA            = 16,
  // Lights
  LIGHT            = 50,
  DIRECTIONALLIGHT = 51,
  HEMISPHERICLIGHT = 52,
  POINTLIGHT       = 53,
  SPOTLIGHT        = 54,
  // Materials
  MATERIAL         = 100,
  MULTIMATERIAL    = 101,
  SHADERMATERIAL   = 102,
  STANDARDMATERIAL = 103,
  PBRMATERIAL      = 104,
  NORMALMATERIAL   = 105,
  PUSHMATERIAL     = 106,
  // Meshes
  ABSTRACTMESH  = 150,
  GROUNDMESH    = 151,
  INSTANCEDMESH = 152,
  LINESMESH     = 153,
  MESH          = 154,
  TRANSFORMNODE = 155,
  // Particles
  PARTICLE          = 200,
  PARTICLESYSTEM    = 201,
  GPUPARTICLESYSTEM = 202,
  // Texture
  BASETEXTURE = 250,
  TEXTURE     = 251,
  // Morph
  MORPHTARGET = 300,
  // Piplines
  STANDARDRENDERINGPIPLINE = 350
}; // end of enum class Type

} // end of namespace BABYLON

#endif // end of BABYLON_ENUMS_H
