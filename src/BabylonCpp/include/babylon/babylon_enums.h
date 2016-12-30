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

/** Event types **/
enum class EventType {
  CONTEXT_MENU      = 0,
  MOUSE_BUTTON_DOWN = 1,
  MOUSE_BUTTON_UP   = 2,
  MOUSE_OUT         = 3,
  MOUSE_MOVE        = 4,
  MOUSE_WHEEL       = 5,
  KEY_DOWN          = 6,
  KEY_UP            = 7,
  BLUR              = 8,
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

/** Collisions Enums **/
enum class WorkerTaskType {
  INIT    = 0,
  UPDATE  = 1,
  COLLIDE = 2
}; // end of enum class WorkerTaskType

enum class WorkerReplyType {
  SUCCESS       = 0,
  UNKNOWN_ERROR = 1
}; // end of enum class WorkerReplyType

/** Postproces Enums **/
enum class TonemappingOperator {
  HABLE        = 0,
  REINHARD     = 1,
  HEJIDAWSON   = 2,
  PHOTOGRAPHIC = 3,
}; // end of enum class TonemappingOperator

/** Math Enums **/
enum class Space { LOCAL = 0, WORLD = 1 }; // end of enum class Space

enum class Orientation { CW = 0, CCW = 1 }; // end of enum class Orientation

/** Mesh Enums **/
enum class SimplificationType {
  QUADRATIC
}; // end of enum class SimplificationType

} // end of namespace BABYLON

#endif // end of BABYLON_ENUMS_H
