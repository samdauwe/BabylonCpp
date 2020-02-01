#ifndef BABYLON_CORE_STRUCTS_H
#define BABYLON_CORE_STRUCTS_H

#include <unordered_map>

#include <babylon/babylon_enums.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class ICanvas;
class Node;

struct Bounds {
  Vector2 min;
  Vector2 max;
  float width  = 0.f;
  float height = 0.f;
}; // end of struct Bounds

struct VRMetrics {
  float aspectRatioFov = 0.f;
  float aspectRatio    = 0.f;
}; // end of struct VRMetrics

struct CameraRigParams {
  float interaxialDistance = 0.f;
  float stereoHalfAngle    = 0.f;
  Matrix vrHMatrix;
  Matrix vrWorkMatrix;
  bool vrPreViewMatrixSet = false;
  Matrix vrPreViewMatrix;
  VRMetrics vrMetrics;
  std::unordered_map<std::string, float> params;
}; // end of struct CameraRigParams

struct ClientRect {
  int bottom = 0;
  int height = 0;
  int left   = 0;
  int right  = 0;
  int top    = 0;
  int width  = 0;
}; // end of struct CameraRigParams

struct ClipboardEvent {
}; // end of struct ClipboardEvent

struct DeviceOrientationEvent {
  float gamma = 0.f;
  float beta  = 0.f;
}; // end of struct DeviceOrientationEvent

struct DragMoveEvent {
  Vector3 delta;
  Vector3 dragPlanePoint;
  Vector3 dragPlaneNormal;
  float dragDistance = 0.f;
  int pointerId      = 0;
}; // end of struct DragMoveEvent

struct DragStartOrEndEvent {
  Vector3 dragPlanePoint;
  int pointerId = 0;
}; // end of struct DragStartOrEndEvent

struct Event {
  EventType type = EventType::UNKNOWN;
  int charCode   = 0;
  int keyCode    = 0;
}; // end of struct Event

struct Exception {
  std::string message;
  std::exception exception;
}; // end of struct Exception

struct Extents {
  float min = 0.f;
  float max = 0.f;
}; // end of struct Extents

struct FocusEvent {
}; // end of struct FocusEvent

struct GestureEvent {
  float scale = 0.f;
  void stopPropagation()
  {
  }
  void preventDefault()
  {
  }
}; // end of struct GestureEvent

struct Image {
  Image() = default;

  Image(unsigned char* buffer, int bufferLength, int iWidth, int iHeight, int iDepth,
        unsigned int iMode)
      : data(buffer, buffer + bufferLength)
      , width{iWidth}
      , height{iHeight}
      , depth{iDepth}
      , mode{iMode}
  {
  }
  Image(ArrayBuffer iData, int iWidth, int iHeight, int iDepth, unsigned int iMode)
      : data(std::move(iData)), width{iWidth}, height{iHeight}, depth{iDepth}, mode{iMode}
  {
  }
  [[nodiscard]] bool valid() const
  {
    return !data.empty() && width > 0 && height > 0 && depth > 0;
  }
  ArrayBuffer data;
  int width = 0, height = 0;
  int depth         = 0;
  unsigned int mode = 0;
}; // end of struct Image

struct KeyboardEvent : public Event {
  bool ctrlKey  = false;
  bool altKey   = false;
  bool metaKey  = false;
  bool shiftKey = false;
  std::string code;
  void preventDefault() const
  {
  }
}; // end of struct KeyboardEvent

struct LowestRoot {
  float root = 0.f;
  bool found = false;
}; // end of struct LowestRoot

struct MinMagFilter {
  int mag = 0;
  int min = 0;
}; // end of struct MinMagFilter

struct MinMax {
  Vector3 min = Vector3::Zero();
  Vector3 max = Vector3::Zero();
}; // end of struct MinMax

struct MinMaxDistance {
  Vector3 min    = Vector3::Zero();
  Vector3 max    = Vector3::Zero();
  float distance = 0.f;
}; // end of struct MinMaxDistance

struct MinMaxVector2 {
  Vector2 min;
  Vector2 max;
}; // end of struct MinMaxVector2

struct MouseEvent {
  int clientX   = 0;
  int clientY   = 0;
  int movementX = 0;
  int movementY = 0;
  void preventDefault()
  {
  }
}; // end of struct MouseEvent

struct NodeCache {
  bool cache_inited = false;

  Node* parent = nullptr;
  // AbstractMesh
  Vector3 position;
  Vector3 scaling;
  bool pivotMatrixUpdated       = false;
  Quaternion rotationQuaternion = Quaternion::Zero();
  bool localMatrixUpdated       = false;
  Vector3 rotation;
  Vector3 upVector;
  unsigned int billboardMode = 0;
  bool infiniteDistance      = false;
  // Camera
  unsigned int mode    = 0;
  float minZ           = 0.f;
  float maxZ           = 0.f;
  float fov            = 0.f;
  unsigned int fovMode = 0;
  float aspectRatio    = 0.f;
  float orthoLeft      = 0.f;
  float orthoRight     = 0.f;
  float orthoTop       = 0.f;
  float orthoBottom    = 0.f;
  int renderWidth      = 0;
  int renderHeight     = 0;
  // TargetCamera
  std::unique_ptr<Vector3> lockedTarget = nullptr;
  // ArcRotateCamera
  Vector3 _target;
  float alpha  = 0.f;
  float beta   = 0.f;
  float gamma  = 0.f;
  float radius = 0.f;
  Vector2 targetScreenOffset;
}; // end of struct NodeCache

struct PickedParticle {
  size_t idx;
  size_t faceId;
}; // end of struct PickedParticle

struct PointerEvent : public Event {
  PointerEvent() = default;
  PointerEvent(const std::string& /*type*/)
  {
  }

  PointerEvent(const PointerEvent& other) = default;
  PointerEvent(PointerEvent&& other)      = default;

  PointerEvent& operator=(const PointerEvent& other) = default;
  PointerEvent& operator=(PointerEvent&& other) = default;

  int clientX             = 0;
  int clientY             = 0;
  int movementX           = 0;
  int movementY           = 0;
  int offsetX             = 0;
  int offsetY             = 0;
  int pointerId           = 0;
  PointerType pointerType = PointerType::UNDEFINED;
  bool altKey             = false;
  bool ctrlKey            = false;
  bool shiftKey           = false;
  bool metaKey            = false;
  MouseButtonType button  = MouseButtonType::UNDEFINED;
  int buttons             = 0;
  ICanvas* srcElement     = nullptr;
  void preventDefault()
  {
  }
}; // end of struct PointerEvent

struct PointerTouch {
  int x                   = 0;
  int y                   = 0;
  int pointerId           = 0;
  PointerType pointerType = PointerType::UNDEFINED;
  bool isPaning           = false;
  bool isPinching         = false;
}; // end of struct PointerTouch

struct MouseWheelEvent : public PointerEvent {
  MouseWheelEvent()                             = default;
  MouseWheelEvent(const MouseWheelEvent& other) = default;
  MouseWheelEvent(MouseWheelEvent&& other)      = default;
  MouseWheelEvent& operator=(const MouseWheelEvent& other) = default;
  MouseWheelEvent& operator=(MouseWheelEvent&& other) = default;

  float wheelDelta = 0.f;
  float detail     = 0.f;
  void preventDefault() const
  {
  }
}; // end of struct MouseWheelEvent

struct Polyhedron {
  std::string name;
  std::string category;
  std::vector<Float32Array> vertices;
  std::vector<Uint32Array> faces;
}; // end of struct Polyhedron

struct PositionCoord {
  int x = 0;
  int y = 0;
}; // end of struct PositionCoord

struct PostProcessOptions {
  int width  = 0;
  int height = 0;
}; // end of struct PostProcessOptions

struct RootResult {
  bool found  = false;
  double root = 0.;
}; // end of struct RootResult

struct SamplingParameters {
  int min = 0;
  int mag = 0;
}; // end of struct SamplingParameters

struct SizeF {
  float width  = 0.f;
  float height = 0.f;
}; // end of struct SizeF

struct SolidParticleOptions {
  bool updatable  = false;
  bool isPickable = false;
}; // end of struct SolidParticleOptions

struct Style {
  std::string cursor;
}; // end of struct cursor

struct SubdivisionsPerAxis {
  unsigned int max = 0;
  unsigned int X   = 0;
  unsigned int Y   = 0;
  unsigned int Z   = 0;
}; // end of struct SubdivisionsPerAxis

struct SnapEvent {
  float snapDistance = 0.f;
}; // end of struct SnapEvent

struct TriggerOptions {
  std::string parameter;
  unsigned int trigger = 0;
}; // end of struct TriggerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRUCTS_H
