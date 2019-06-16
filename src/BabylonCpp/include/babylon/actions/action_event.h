#ifndef BABYLON_ACTIONS_ACTIONEVENT_H
#define BABYLON_ACTIONS_ACTIONEVENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class AbstractMesh;
class Scene;
class Sprite;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using SpritePtr       = std::shared_ptr<Sprite>;

/**
 * @brief ActionEvent is the event being sent when an action is triggered.
 */
class BABYLON_SHARED_EXPORT ActionEvent {

public:
  /**
   * @brief Creates a new ActionEvent.
   * @param source The mesh or sprite that triggered the action
   * @param pointerX The X mouse cursor position at the time of the event
   * @param pointerY The Y mouse cursor position at the time of the event
   * @param meshUnderPointer The mesh that is currently pointed at (can be null)
   * @param sourceEvent the original (browser) event that triggered the
   * ActionEvent
   * @param additionalData additional data for the event
   */
  ActionEvent(AbstractMesh* source, int pointerX, int pointerY,
              AbstractMesh* meshUnderPointer,
              const std::optional<Event>& sourceEvent,
              const std::string& additionalData = "");
  ActionEvent(const ActionEvent& other);
  ActionEvent(ActionEvent&& other);
  ActionEvent& operator=(const ActionEvent& other);
  ActionEvent& operator=(ActionEvent&& other);
  ~ActionEvent();

  /**
   * @brief Helper function to auto-create an ActionEvent from a source mesh.
   * @param source The source mesh that triggered the event
   * @param evt The original (browser) event
   * @param additionalData additional data for the event
   * @returns the new ActionEvent
   */
  static ActionEvent CreateNew(AbstractMesh* source,
                               const std::optional<Event>& evt = std::nullopt);

  /**
   * @brief Helper function to auto-create an ActionEvent from a source sprite.
   * @param source The source sprite that triggered the event
   * @param scene Scene associated with the sprite
   * @param evt The original (browser) event
   * @param additionalData additional data for the event
   * @returns the new ActionEvent
   */
  static ActionEvent CreateNewFromSprite(const SpritePtr& source, Scene* scene,
                                         const Event& evt);

  /**
   * @brief Helper function to auto-create an ActionEvent from a scene. If
   * triggered by a mesh use ActionEvent.CreateNew.
   * @param scene the scene where the event occurred
   * @param evt The original (browser) event
   * @returns the new ActionEvent
   */
  static ActionEvent CreateNewFromScene(Scene* scene, const Event& evt);

  /**
   * @brief Helper function to auto-create an ActionEvent from a primitive.
   * @param prim defines the target primitive
   * @param pointerPos defines the pointer position
   * @param evt The original (browser) event
   * @param additionalData additional data for the event
   * @returns the new ActionEvent
   */
  static ActionEvent
  CreateNewFromPrimitive(AbstractMesh* prim, const Vector2& pointerPos,
                         const Event& evt,
                         const std::string& additionalData = "");

public:
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

}; // end of class ActionEvent

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTIONEVENT_H
