#ifndef BABYLON_ACTIONS_IACTION_EVENT_H
#define BABYLON_ACTIONS_IACTION_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class Scene;
class Sprite;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(Sprite)

/**
 * @brief Interface used to define ActionEvent
 */
struct BABYLON_SHARED_EXPORT IActionEvent {

  IActionEvent(const AbstractMeshPtr& source, int pointerX, int pointerY,
               const AbstractMeshPtr& meshUnderPointer, const std::optional<Event>& sourceEvent,
               const std::string& additionalData = "");
  IActionEvent(const IActionEvent& other);
  IActionEvent(IActionEvent&& other);
  IActionEvent& operator=(const IActionEvent& other);
  IActionEvent& operator=(IActionEvent&& other);
  ~IActionEvent(); // = default

  /** The mesh that triggered the action */
  AbstractMeshPtr source;
  /** The sprite that triggered the action */
  SpritePtr sprite;
  /** The X mouse cursor position at the time of the event */
  int pointerX = 0;
  /** The Y mouse cursor position at the time of the event */
  int pointerY = 0;
  /** The mesh that is currently pointed at (can be null) */
  AbstractMeshPtr meshUnderPointer;
  /** The original (browser) event that triggered the ActionEvent */
  std::optional<Event> sourceEvent;
  /** Additional data for the event */
  std::string additionalData;

}; // end of struct IActionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_IACTION_EVENT_H
