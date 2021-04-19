#include <babylon/events/keyboard_info.h>

namespace BABYLON {

KeyboardInfo::KeyboardInfo(unsigned int iType, const IKeyboardEvent& iEvent)
    : type{iType}, event{iEvent}
{
}

KeyboardInfo::~KeyboardInfo() = default;

} // end of namespace BABYLON
