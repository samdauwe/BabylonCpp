#ifndef BABYLON_CORE_STRUCTS_H
#define BABYLON_CORE_STRUCTS_H

#include <unordered_map>

#include <babylon/babylon_enums.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class ICanvas;
class Node;

struct Bounds {
  Vector2 min;
  Vector2 max;
  float width;
  float height;
}; // end of struct Bounds

struct VRMetrics {
  float aspectRatioFov;
  float aspectRatio;
}; // end of struct VRMetrics

struct CameraRigParams {
  float interaxialDistance;
  float stereoHalfAngle;
  Matrix vrHMatrix;
  Matrix vrWorkMatrix;
  bool vrPreViewMatrixSet = false;
  Matrix vrPreViewMatrix;
  VRMetrics vrMetrics;
  std::unordered_map<std::string, float> params;
}; // end of struct CameraRigParams

struct ClientRect {
  int bottom;
  int height;
  int left;
  int right;
  int top;
  int width;
}; // end of struct CameraRigParams

struct ClipboardEvent {
}; // end of struct ClipboardEvent

struct DeviceOrientationEvent {
  float gamma;
  float beta;
}; // end of struct DeviceOrientationEvent

struct DragMoveEvent {
  Vector3 delta;
  Vector3 dragPlanePoint;
  Vector3 dragPlaneNormal;
  float dragDistance;
  int pointerId;
}; // end of struct DragMoveEvent

struct DragStartOrEndEvent {
  Vector3 dragPlanePoint;
  int pointerId;
}; // end of struct DragStartOrEndEvent

struct Event {
  EventType type;
  int charCode;
  int keyCode;
}; // end of struct Event

struct Exception {
  std::string message;
  std::exception exception;
}; // end of struct Exception

struct Extents {
  float min;
  float max;
}; // end of struct Extents

struct FocusEvent {
}; // end of struct FocusEvent

struct GestureEvent {
  float scale;
  void stopPropagation()
  {
  }
  void preventDefault()
  {
  }
}; // end of struct GestureEvent

struct Image {
  Image() : width(0), height(0), mode(0)
  {
  }
  Image(unsigned char* buffer, int bufferLength, int iWidth, int iHeight,
        int iDepth, unsigned int iMode)
      : data(buffer, buffer + bufferLength)
      , width{iWidth}
      , height{iHeight}
      , depth{iDepth}
      , mode{iMode}
  {
  }
  Image(ArrayBuffer iData, int iWidth, int iHeight, int iDepth,
        unsigned int iMode)
      : data(std::move(iData))
      , width{iWidth}
      , height{iHeight}
      , depth{iDepth}
      , mode{iMode}
  {
  }
  bool valid() const
  {
    return data.size() > 0 && width > 0 && height > 0 && depth > 0;
  }
  ArrayBuffer data;
  int width, height;
  int depth;
  unsigned int mode;
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
  float root;
  bool found;
}; // end of struct LowestRoot

struct MinMagFilter {
  int mag;
  int min;
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
  int clientX;
  int clientY;
  int movementX;
  int movementY;
  void preventDefault()
  {
  }
}; // end of struct MouseEvent

struct NodeCache {
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
  unsigned int idx;
  size_t faceId;
}; // end of struct PickedParticle

struct PointerEvent : public Event {
  PointerEvent()
  {
  }
  PointerEvent(const std::string& /*type*/)
  {
  }

  PointerEvent(const PointerEvent& other) = default;
  PointerEvent(PointerEvent&& other) = default;
  
  PointerEvent& operator=(const PointerEvent& other) = default;
  PointerEvent& operator=(PointerEvent&& other) = default;

  int clientX   = 0;
  int clientY   = 0;
  int movementX = 0;
  int movementY = 0;
  int offsetX   = 0;
  int offsetY   = 0;
  int pointerId = 0;
  PointerType pointerType;
  bool altKey   = false;
  bool ctrlKey  = false;
  bool shiftKey = false;
  bool metaKey  = false;
  MouseButtonType button;
  int buttons         = 0;
  ICanvas* srcElement = nullptr;
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
  MouseWheelEvent()
  {
  }
  MouseWheelEvent(const MouseWheelEvent& other)
      : PointerEvent{other}, wheelDelta{other.wheelDelta}, detail{other.detail}
  {
  }
  MouseWheelEvent(MouseWheelEvent&& other)
      : PointerEvent{std::move(other)}
      , wheelDelta{std::move(other.wheelDelta)}
      , detail{std::move(other.detail)}
  {
  }
  MouseWheelEvent& operator=(const MouseWheelEvent& other)
  {
    if (&other != this) {
      PointerEvent::operator=(other);
      wheelDelta            = other.wheelDelta;
      detail                = other.detail;
    }

    return *this;
  }
  MouseWheelEvent& operator=(MouseWheelEvent&& other)
  {
    if (&other != this) {
      PointerEvent::operator=(std::move(other));
      wheelDelta            = std::move(other.wheelDelta);
      detail                = std::move(other.detail);
    }

    return *this;
  }
  float wheelDelta;
  float detail;
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
  int x;
  int y;
}; // end of struct PositionCoord

struct PostProcessOptions {
  int width;
  int height;
}; // end of struct PostProcessOptions

struct RootResult {
  bool found;
  double root;
}; // end of struct RootResult

struct SamplingParameters {
  int min;
  int mag;
}; // end of struct SamplingParameters

struct SizeF {
  float width;
  float height;
}; // end of struct SizeF

struct SolidParticleOptions {
  bool updatable;
  bool isPickable;
}; // end of struct SolidParticleOptions

struct Style {
  std::string cursor;
}; // end of struct cursor

struct SubdivisionsPerAxis {
  unsigned int max;
  unsigned int X;
  unsigned int Y;
  unsigned int Z;
}; // end of struct SubdivisionsPerAxis

struct SnapEvent {
  float snapDistance;
}; // end of struct SnapEvent

struct TriggerOptions {
  std::string parameter;
  unsigned int trigger;
}; // end of struct TriggerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRUCTS_H
