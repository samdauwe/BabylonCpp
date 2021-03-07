#include <babylon/actions/action_event.h>

#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

ActionEvent::ActionEvent(const AbstractMeshPtr& iSource, int iPointerX, int iPointerY,
                         const AbstractMeshPtr& iMeshUnderPointer, const IEventPtr& iSourceEvent,
                         const std::optional<PickingInfo>& /*iAdditionalData*/)
    : IActionEvent{iSource,           iPointerX,    iPointerY,
                   iMeshUnderPointer, iSourceEvent, /*iAdditionalData*/ ""}
{
}

ActionEvent::ActionEvent(const ActionEvent& other) = default;

ActionEvent::ActionEvent(ActionEvent&& other) = default;

ActionEvent& ActionEvent::operator=(const ActionEvent& other) = default;

ActionEvent& ActionEvent::operator=(ActionEvent&& other) = default;

ActionEvent::~ActionEvent() = default;

ActionEvent ActionEvent::CreateNew(const AbstractMeshPtr& iSource, const IEventPtr& evt,
                                   const std::optional<PickingInfo>& iAdditionalData)
{
  auto scene = iSource->getScene();
  return ActionEvent(iSource, scene->pointerX(), scene->pointerY(), scene->meshUnderPointer(), evt,
                     iAdditionalData);
}

ActionEvent ActionEvent::CreateNewFromSprite(const SpritePtr& iSource, Scene* scene,
                                             const IEventPtr& evt)
{
  ActionEvent actionEvent(nullptr, scene->pointerX(), scene->pointerY(), scene->meshUnderPointer(),
                          evt);
  actionEvent.sprite = iSource;
  return actionEvent;
}

ActionEvent ActionEvent::CreateNewFromScene(Scene* scene, const IEventPtr& evt)
{
  return ActionEvent(nullptr, scene->pointerX(), scene->pointerY(), scene->meshUnderPointer(), evt);
}

ActionEvent ActionEvent::CreateNewFromPrimitive(const AbstractMeshPtr& prim,
                                                const Vector2& pointerPos, const IEventPtr& evt,
                                                const std::optional<PickingInfo>& iAdditionalData)
{
  return ActionEvent(prim, static_cast<int>(pointerPos.x), static_cast<int>(pointerPos.y), nullptr,
                     evt, iAdditionalData);
}

} // end of namespace BABYLON
