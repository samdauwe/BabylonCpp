#ifndef BABYLON_CORE_STRUCTS_H
#define BABYLON_CORE_STRUCTS_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct ArrayBufferView {
  enum class Type {
    FLOAT32ARRAY_TYPE = 0,
    INT16ARRAY_TYPE   = 1,
    UINT8ARRAY_TYPE   = 2,
    UINT16ARRAY_TYPE  = 3,
    UINT32ARRAY_TYPE  = 4
  }; // end of enum class Type

  ArrayBufferView() : type{Type::UINT8ARRAY_TYPE}
  {
  }
  ArrayBufferView(Float32Array iFloat32Array)
      : type{Type::FLOAT32ARRAY_TYPE}, float32Array{iFloat32Array}
  {
  }
  ArrayBufferView(Int16Array iInt16Array)
      : type{Type::INT16ARRAY_TYPE}, int16Array{iInt16Array}
  {
  }
  ArrayBufferView(Uint8Array iUint8Array)
      : type{Type::UINT8ARRAY_TYPE}, uint8Array{iUint8Array}
  {
  }
  ArrayBufferView(Uint16Array iUint16Array)
      : type{Type::UINT16ARRAY_TYPE}, uint16Array{iUint16Array}
  {
  }
  ArrayBufferView(Uint32Array iUint32Array)
      : type{Type::UINT32ARRAY_TYPE}, uint32Array{iUint32Array}
  {
  }
  ArrayBufferView(const ArrayBufferView& other)
      : type{other.type}
      , float32Array{other.float32Array}
      , int16Array{other.int16Array}
      , uint8Array{other.uint8Array}
      , uint16Array{other.uint16Array}
      , uint32Array{other.uint32Array}
  {
  }
  ArrayBufferView(ArrayBufferView&& other)
      : type{::std::move(other.type)}
      , float32Array{::std::move(other.float32Array)}
      , int16Array{::std::move(other.int16Array)}
      , uint8Array{::std::move(other.uint8Array)}
      , uint16Array{::std::move(other.uint16Array)}
      , uint32Array{::std::move(other.uint32Array)}
  {
  }
  ArrayBufferView& operator=(const ArrayBufferView& other)
  {
    if (&other != this) {
      type         = other.type;
      float32Array = other.float32Array;
      int16Array   = other.int16Array;
      uint8Array   = other.uint8Array;
      uint16Array  = other.uint16Array;
      uint32Array  = other.uint32Array;
    }

    return *this;
  }
  ArrayBufferView& operator=(ArrayBufferView&& other)
  {
    if (&other != this) {
      type         = ::std::move(other.type);
      float32Array = ::std::move(other.float32Array);
      int16Array   = ::std::move(other.int16Array);
      uint8Array   = ::std::move(other.uint8Array);
      uint16Array  = ::std::move(other.uint16Array);
      uint32Array  = ::std::move(other.uint32Array);
    }

    return *this;
  }
  ~ArrayBufferView()
  {
  }

  Type type;
  Float32Array float32Array;
  Int16Array int16Array;
  Uint8Array uint8Array;
  Uint16Array uint16Array;
  Uint32Array uint32Array;
}; // end of struct ArrayBufferView

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
  float stereoHalfAngle;
  Matrix vrHMatrix;
  Matrix vrWorkMatrix;
  bool vrPreViewMatrixSet = false;
  Matrix vrPreViewMatrix;
  VRMetrics vrMetrics;
  unordered_map_t<string_t, float> params;
}; // end of struct CameraRigParams

struct ClientRect {
  int bottom;
  int height;
  int left;
  int right;
  int top;
  int width;
}; // end of struct CameraRigParams

struct DeviceOrientationEvent {
  float gamma;
  float beta;
}; // end of struct DeviceOrientationEvent

struct Event {
  char* charCode;
  char* keyCode;
}; // end of struct Event

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
      : data(::std::move(iData))
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

struct KeyboardEvent {
  bool ctrlKey;
  bool altKey;
  int keyCode;
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
  Vector3 min;
  Vector3 max;
}; // end of struct MinMax

struct MinMaxDistance {
  Vector3 min;
  Vector3 max;
  float distance;
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

struct MouseWheelEvent {
  float wheelDelta;
  float detail;
  void preventDefault() const
  {
  }
}; // end of struct MouseWheelEvent

struct NodeCache {
  Node* parent;
  // AbstractMesh
  Vector3 position;
  Vector3 scaling;
  bool pivotMatrixUpdated;
  Quaternion rotationQuaternion;
  bool localMatrixUpdated;
  Vector3 rotation;
  Vector3 upVector;
  unsigned int billboardMode;
  // Camera
  unsigned int mode;
  float minZ;
  float maxZ;
  float fov;
  unsigned int fovMode;
  float aspectRatio;
  float orthoLeft;
  float orthoRight;
  float orthoTop;
  float orthoBottom;
  int renderWidth;
  int renderHeight;
  // TargetCamera
  unique_ptr_t<Vector3> lockedTarget;
  // ArcRotateCamera
  Vector3 _target;
  float alpha;
  float beta;
  float gamma;
  float radius;
  Vector2 targetScreenOffset;
}; // end of struct NodeCache

struct PickedParticle {
  unsigned int idx;
  unsigned int faceId;
}; // end of struct PickedParticle

struct PointerEvent : public Event {
  PointerEvent()
  {
  }
  PointerEvent(const string_t& /*type*/)
  {
  }
  int clientX;
  int clientY;
  int movementX;
  int movementY;
  int pointerId;
  PointerType pointerType;
  bool ctrlKey;
  bool shiftKey;
  EventType type;
  MouseButtonType button;
  int buttons;
  ICanvas* srcElement;
  void preventDefault()
  {
  }
}; // end of struct PointerEvent

struct Polyhedron {
  string_t name;
  string_t category;
  vector_t<Float32Array> vertices;
  vector_t<Uint32Array> faces;
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
  string_t cursor;
}; // end of struct cursor

struct SubdivisionsPerAxis {
  unsigned int max;
  unsigned int X;
  unsigned int Y;
  unsigned int Z;
}; // end of struct SubdivisionsPerAxis

struct TriggerOptions {
  string_t parameter;
  unsigned int trigger;
}; // end of struct TriggerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_STRUCTS_H
