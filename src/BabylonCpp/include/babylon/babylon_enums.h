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
enum class AnimationKeyInterpolation {
  /**
   * Do not interpolate between keys and use the start key value only. Tangents
   * are ignored.
   */
  STEP = 1
}; // end of enum class AnimationKeyInterpolation

/** Event types **/
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
}; // end of enum class EventType

/** Mouse button types **/
enum class MouseButtonType {
  UNDEFINED = -1,
  LEFT      = 0,
  MIDDLE    = 1,
  RIGHT     = 2
}; // end of enum class MouseButtonType

/** Pointer types **/
enum class PointerType {
  UNDEFINED = -1,
  MOUSE     = 0,
  TOUCH     = 1,
}; // end of enum class MouseButtonType

/** Property types **/
enum class PropertyType : unsigned int {
  ROTATION = 100
}; // end of enum class PropertyType

/** Defines supported task for worker process **/
enum class WorkerTaskType {
  /** Initialization */
  INIT = 0,
  /** Update of geometry */
  UPDATE = 1,
  /** Evaluate collision */
  COLLIDE = 2
}; // end of enum class WorkerTaskType

/** Defines kind of replies returned by worker */
enum class WorkerReplyType {
  /** Success */
  SUCCESS = 0,
  /** Unkown error */
  UNKNOWN_ERROR = 1
}; // end of enum class WorkerReplyType

/** Defines operator used for tonemapping **/
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
enum class SimplificationType {
  QUADRATIC
}; // end of enum class SimplificationType

} // end of namespace BABYLON

#endif // end of BABYLON_ENUMS_H
