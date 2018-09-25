#ifndef BABYLON_EXTENSIONS_MANIPULATION_HELPERS_MANIPULATOR_INTERACTION_HELPER_H
#define BABYLON_EXTENSIONS_MANIPULATION_HELPERS_MANIPULATOR_INTERACTION_HELPER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/extensions/manipulationhelpers/radix.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/event_state.h>

namespace BABYLON {

class PointerInfo;
class Ray;

namespace Extensions {

enum class ManFlags : int {
  None     = 0x00,
  DragMode = 0x01,
  FirstHit = 0x02,
  Plane2   = 0x04,
  Exiting  = 0x08
}; // end of enum class ManFlags

using T = std::underlying_type_t<ManFlags>;

inline constexpr ManFlags operator&(ManFlags lhs, ManFlags rhs)

{
  return static_cast<ManFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline constexpr ManFlags& operator&=(ManFlags& lhs, ManFlags rhs)
{
  lhs = static_cast<ManFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
  return lhs;
}

inline constexpr ManFlags operator|(ManFlags lhs, ManFlags rhs)

{
  return static_cast<ManFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline constexpr ManFlags& operator|=(ManFlags& lhs, ManFlags rhs)
{
  lhs = static_cast<ManFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
  return lhs;
}

inline constexpr ManFlags operator~(ManFlags rhs)

{
  return static_cast<ManFlags>(~static_cast<T>(rhs));
}

/**
 * @brief This class is used to manipulated a single node.
 * Right now only node of type AbstractMesh is support.
 * In the future, manipulation on multiple selection could be possible.
 *
 * A manipulation start when left clicking and moving the mouse. It can be
 * cancelled if the right mouse button is clicked before releasing the left one
 * (this feature is only possible if noPreventContextMenu is false).
 * Per default translation is peformed when manipulating the arrow (axis or
 * cone) or the plane anchor. If you press the shift key it will switch to
 * rotation manipulation. The Shift key can be toggle while manipulating, the
 * current manipulation is accept and a new one starts.
 *
 * You can set the rotation/translationStep (in radian) to enable snapping.
 *
 * The current implementation of this class creates a radix with all the
 * features selected.
 */
class BABYLON_SHARED_EXPORT ManipulatorInteractionHelper {

public:
  ManipulatorInteractionHelper(Scene* scene);
  ~ManipulatorInteractionHelper();

  /**
   * @brief Attach a node to manipulate. Right now, only manipulation on a
   * single node is supported, but this api will allow manipulation on a
   * multiple selection in the future.
   * @param node
   */
  void attachManipulatedNode(Node* node);

  /**
   * @brief Detach the node to manipulate. Right now, only manipulation on a
   * single node is supported, but this api will allow manipulation on a
   * multiple selection in the future.
   */
  void detachManipulatedNode(Node* node);

private:
  void onBeforeRender(Scene* scene, EventState& state);
  void onPointer(PointerInfo& e, EventState& state);
  void beginDrag(const Vector2& rayPos, const PointerEvent& event);
  void endDragMode();
  void doRot(const Vector2& rayPos);
  void doPos(const Vector2& rayPos);
  bool hasManipulatedMode(RadixFeatures value);
  bool hasManFlags(ManFlags value);
  ManFlags clearManFlags(ManFlags values);
  ManFlags setManFlags(ManFlags values);
  static Vector3 ComputeRayHit(const Ray& ray, float distance);
  static bool instanceofAbstractMesh(Node* node);
  void setManipulatedNodeWorldMatrix(Matrix mtx);
  Matrix* getManipulatedNodeWorldMatrix();
  std::tuple<Plane, Vector3> setupIntersectionPlane(RadixFeatures mode,
                                                    bool plane2);
  std::tuple<Plane, Plane> setupIntersectionPlanes(RadixFeatures mode);
  Vector2 getRayPosition(const PointerEvent& event);
  void renderManipulator();
  float fromScreenToWorld(float l, float z);
  static Vector3 evalPosition(const Ray& ray, float u);

public:
  /**
   * Rotation Step in Radian to perform rotation with the given step instead of
   * a smooth one.
   * Set back to null/undefined to disable
   */
  float rotationStep;

  /**
   * Translation Step in World unit to perform translation at the given step
   * instread of a smooth one.
   * Set back to null/undefined to disable
   */
  float translationStep;

  /**
   * Set to true if you want the context menu to be displayed while
   * manipulating. The manipulation cancel feature (which is triggered by a
   * right click) won't work in this case. Default value is false (context menu
   * is not showed when right clicking during manipulation) and this should fit
   * most of the cases.
   */
  bool noPreventContextMenu;

private:
  ManFlags _flags;
  Vector2 _firstMousePos;
  Vector2 _prevMousePos;
  bool _shiftKeyState;
  Vector3 _pos;
  Vector3 _right;
  Vector3 _up;
  Vector3 _view;
  Vector3 _oldPos;
  Vector3 _prevHit;
  Matrix _firstTransform;
  Scene* _scene;
  RadixFeatures _manipulatedMode;
  float _rotationFactor;
  Node* _manipulatedNode;
  Radix _radix;

}; // end of class ManipulatorInteractionHelper

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
// BABYLON_EXTENSIONS_MANIPULATION_HELPERS_MANIPULATOR_INTERACTION_HELPER_H
