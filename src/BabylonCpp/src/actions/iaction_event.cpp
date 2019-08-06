#include <babylon/actions/iaction_event.h>

namespace BABYLON {

IActionEvent::IActionEvent(const AbstractMeshPtr& iSource, int iPointerX,
                           int iPointerY, AbstractMesh* iMeshUnderPointer,
                           const std::optional<Event>& iSourceEvent,
                           const std::string& iAdditionalData)
    : source{iSource}
    , pointerX{iPointerX}
    , pointerY{iPointerY}
    , meshUnderPointer{iMeshUnderPointer}
    , sourceEvent{iSourceEvent}
    , additionalData{iAdditionalData}
{
}

IActionEvent::IActionEvent(const IActionEvent& other)
    : source{other.source}
    , sprite{other.sprite}
    , pointerX{other.pointerX}
    , pointerY{other.pointerY}
    , meshUnderPointer{other.meshUnderPointer}
    , sourceEvent{other.sourceEvent}
    , additionalData{other.additionalData}
{
}

IActionEvent::IActionEvent(IActionEvent&& other)
    : source{std::move(other.source)}
    , sprite{std::move(other.sprite)}
    , pointerX{std::move(other.pointerX)}
    , pointerY{std::move(other.pointerY)}
    , meshUnderPointer{std::move(other.meshUnderPointer)}
    , sourceEvent{std::move(other.sourceEvent)}
    , additionalData{std::move(other.additionalData)}
{
}

IActionEvent& IActionEvent::operator=(const IActionEvent& other)
{
  if (&other != this) {
    source           = other.source;
    sprite           = other.sprite;
    pointerX         = other.pointerX;
    pointerY         = other.pointerY;
    meshUnderPointer = other.meshUnderPointer;
    sourceEvent      = other.sourceEvent;
    additionalData   = other.additionalData;
  }

  return *this;
}

IActionEvent& IActionEvent::operator=(IActionEvent&& other)
{
  if (&other != this) {
    source           = std::move(other.source);
    sprite           = std::move(other.sprite);
    pointerX         = std::move(other.pointerX);
    pointerY         = std::move(other.pointerY);
    meshUnderPointer = std::move(other.meshUnderPointer);
    sourceEvent      = std::move(other.sourceEvent);
    additionalData   = std::move(other.additionalData);
  }

  return *this;
}

IActionEvent::~IActionEvent()
{
}

} // end of namespace BABYLON
