#ifndef BABYLON_ACTIONS_ACTIONEVENT_H
#define BABYLON_ACTIONS_ACTIONEVENT_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>

namespace BABYLON {

/**
 * @brief ActionEvent is the event beint sent when an action is triggered.
 */
class BABYLON_SHARED_EXPORT ActionEvent {

public:
  /**
   * Constructor
   * @param source The mesh that triggered the action.
   * @param pointerX the X mouse cursor position at the time of the event
   * @param pointerY the Y mouse cursor position at the time of the event
   * @param meshUnderPointer The mesh that is currently pointed at (can be null)
   * @param sourceEvent the original (browser) event that triggered the
   * ActionEvent
   */
  ActionEvent(AbstractMesh* source, int pointerX, int pointerY,
              AbstractMesh* meshUnderPointer, const Event& sourceEvent);
  ActionEvent(const ActionEvent& other);
  ActionEvent(ActionEvent&& other);
  ActionEvent& operator=(const ActionEvent& other);
  ActionEvent& operator=(ActionEvent&& other);
  ~ActionEvent();

  /**
   * Helper function to auto-create an ActionEvent from a source mesh.
   * @param source the source mesh that triggered the event
   * @param evt {Event} The original (browser) event
   */
  static ActionEvent CreateNew(AbstractMesh* source, const Event& evt);

  /**
   * Helper function to auto-create an ActionEvent from a source mesh.
   * @param source The source sprite that triggered the event
   * @param scene Scene associated with the sprite
   * @param evt {Event} The original (browser) event
   */
  static ActionEvent CreateNewFromSprite(Sprite* source, Scene* scene,
                                         const Event& evt);

  /**
   * Helper function to auto-create an ActionEvent from a scene. If triggered by
   * a mesh use ActionEvent.CreateNew
   * @param scene the scene where the event occurred
   * @param evt {Event} The original (browser) event
   */
  static ActionEvent CreateNewFromScene(Scene* scene, const Event& evt);

public:
  AbstractMesh* source;
  Sprite* sprite;
  int pointerX;
  int pointerY;
  AbstractMesh* meshUnderPointer;
  Event sourceEvent;

}; // end of class ActionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTIONEVENT_H
