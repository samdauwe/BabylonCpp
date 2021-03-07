#include <babylon/actions/iaction_event.h>

namespace BABYLON {

IActionEvent::IActionEvent(const AbstractMeshPtr& iSource, int iPointerX, int iPointerY,
                           const AbstractMeshPtr& iMeshUnderPointer, const IEventPtr& iSourceEvent,
                           const std::string& iAdditionalData)
    : source{iSource}
    , pointerX{iPointerX}
    , pointerY{iPointerY}
    , meshUnderPointer{iMeshUnderPointer}
    , sourceEvent{iSourceEvent}
    , additionalData{iAdditionalData}
{
}

IActionEvent::IActionEvent(const IActionEvent& other) = default;

IActionEvent::IActionEvent(IActionEvent&& other) = default;

IActionEvent& IActionEvent::operator=(const IActionEvent& other) = default;

IActionEvent& IActionEvent::operator=(IActionEvent&& other) = default;

IActionEvent::~IActionEvent() = default;

} // end of namespace BABYLON
