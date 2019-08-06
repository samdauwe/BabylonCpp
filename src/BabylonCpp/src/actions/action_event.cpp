#include <babylon/actions/action_event.h>

#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

ActionEvent::ActionEvent(const AbstractMeshPtr& iSource, int iPointerX,
                         int iPointerY, AbstractMesh* iMeshUnderPointer,
                         const std::optional<Event>& iSourceEvent,
                         const std::string& iAdditionalData)
    : IActionEvent{iSource,           iPointerX,    iPointerY,
                   iMeshUnderPointer, iSourceEvent, iAdditionalData}
{
}

ActionEvent::ActionEvent(const ActionEvent& other) : IActionEvent{other}
{
}

ActionEvent::ActionEvent(ActionEvent&& other) : IActionEvent{std::move(other)}
{
}

ActionEvent& ActionEvent::operator=(const ActionEvent& other)
{
  IActionEvent::operator=(other);

  return *this;
}

ActionEvent& ActionEvent::operator=(ActionEvent&& other)
{
  IActionEvent::operator=(std::move(other));

  return *this;
}

ActionEvent::~ActionEvent()
{
}

ActionEvent ActionEvent::CreateNew(const AbstractMeshPtr& iSource,
                                   const std::optional<Event>& evt)
{
  auto scene = iSource->getScene();
  return ActionEvent(iSource, scene->pointerX(), scene->pointerY(),
                     scene->meshUnderPointer(), evt);
}

ActionEvent ActionEvent::CreateNewFromSprite(const SpritePtr& iSource,
                                             Scene* scene, const Event& evt)
{
  ActionEvent actionEvent(nullptr, scene->pointerX(), scene->pointerY(),
                          scene->meshUnderPointer(), evt);
  actionEvent.sprite = iSource;
  return actionEvent;
}

ActionEvent ActionEvent::CreateNewFromScene(Scene* scene, const Event& evt)
{
  return ActionEvent(nullptr, scene->pointerX(), scene->pointerY(),
                     scene->meshUnderPointer(), evt);
}

ActionEvent
ActionEvent::CreateNewFromPrimitive(const AbstractMeshPtr& prim,
                                    const Vector2& pointerPos, const Event& evt,
                                    const std::string& iAdditionalData)
{
  return ActionEvent(prim, static_cast<int>(pointerPos.x),
                     static_cast<int>(pointerPos.y), nullptr, evt,
                     iAdditionalData);
}

} // end of namespace BABYLON
