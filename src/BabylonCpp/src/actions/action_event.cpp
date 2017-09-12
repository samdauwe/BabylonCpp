#include <babylon/actions/action_event.h>

#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ActionEvent::ActionEvent(AbstractMesh* iSource, int iPointerX, int iPointerY,
                         AbstractMesh* iMeshUnderPointer,
                         const Event& iSourceEvent)
    : source{iSource}
    , pointerX{iPointerX}
    , pointerY{iPointerY}
    , meshUnderPointer{iMeshUnderPointer}
    , sourceEvent{iSourceEvent}
{
}

ActionEvent::ActionEvent(const ActionEvent& other)
    : source{other.source}
    , sprite{other.sprite}
    , pointerX{other.pointerX}
    , pointerY{other.pointerY}
    , meshUnderPointer{other.meshUnderPointer}
    , sourceEvent{other.sourceEvent}
{
}

ActionEvent::ActionEvent(ActionEvent&& other)
    : source{::std::move(other.source)}
    , sprite{::std::move(other.sprite)}
    , pointerX{::std::move(other.pointerX)}
    , pointerY{::std::move(other.pointerY)}
    , meshUnderPointer{::std::move(other.meshUnderPointer)}
    , sourceEvent{::std::move(other.sourceEvent)}
{
}

ActionEvent& ActionEvent::operator=(const ActionEvent& other)
{
  if (&other != this) {
    source           = other.source;
    sprite           = other.sprite;
    pointerX         = other.pointerX;
    pointerY         = other.pointerY;
    meshUnderPointer = other.meshUnderPointer;
    sourceEvent      = other.sourceEvent;
  }

  return *this;
}

ActionEvent& ActionEvent::operator=(ActionEvent&& other)
{
  if (&other != this) {
    source           = ::std::move(other.source);
    sprite           = ::std::move(other.sprite);
    pointerX         = ::std::move(other.pointerX);
    pointerY         = ::std::move(other.pointerY);
    meshUnderPointer = ::std::move(other.meshUnderPointer);
    sourceEvent      = ::std::move(other.sourceEvent);
  }

  return *this;
}

ActionEvent::~ActionEvent()
{
}

ActionEvent ActionEvent::CreateNew(AbstractMesh* iSource, const Event& evt)
{
  auto scene = iSource->getScene();
  return ActionEvent(iSource, scene->pointerX(), scene->pointerY(),
                     scene->meshUnderPointer(), evt);
}

ActionEvent ActionEvent::CreateNewFromSprite(Sprite* source, Scene* scene,
                                             const Event& evt)
{
  ActionEvent actionEvent(nullptr, scene->pointerX(), scene->pointerY(),
                          scene->meshUnderPointer(), evt);
  actionEvent.sprite = source;
  return actionEvent;
}

ActionEvent ActionEvent::CreateNewFromScene(Scene* scene, const Event& evt)
{
  return ActionEvent(nullptr, scene->pointerX(), scene->pointerY(),
                     scene->meshUnderPointer(), evt);
}

} // end of namespace BABYLON
