#ifndef BABYLON_ACTIONS_IACTION_EVENT_H
#define BABYLON_ACTIONS_IACTION_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class AbstractMesh;
class Scene;
class Sprite;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using SpritePtr       = std::shared_ptr<Sprite>;

/**
 * @brief Interface used to define ActionEvent
 */
struct BABYLON_SHARED_EXPORT IActionEvent {

  IActionEvent(const AbstractMeshPtr& source, int pointerX, int pointerY,
               AbstractMesh* meshUnderPointer,
               const std::optional<Event>& sourceEvent,
               const std::string& additionalData = "");
  IActionEvent(const IActionEvent& other);
  IActionEvent(IActionEvent&& other);
  IActionEvent& operator=(const IActionEvent& other);
  IActionEvent& operator=(IActionEvent&& other);
  ~IActionEvent();

  /** The mesh that triggered the action */
  AbstractMeshPtr source;
  /** The sprite that triggered the action */
  SpritePtr sprite;
  /** The X mouse cursor position at the time of the event */
  int pointerX;
  /** The Y mouse cursor position at the time of the event */
  int pointerY;
  /** The mesh that is currently pointed at (can be null) */
  AbstractMesh* meshUnderPointer;
  /** The original (browser) event that triggered the ActionEvent */
  std::optional<Event> sourceEvent;
  /** Additional data for the event */
  std::string additionalData;

}; // end of struct IActionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_IACTION_EVENT_H
