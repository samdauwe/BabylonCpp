#include <babylon/events/keyboard_info.h>

namespace BABYLON {

KeyboardInfo::KeyboardInfo(unsigned int iType, const KeyboardEvent& iEvent)
    : type{iType}, event{iEvent}
{
}

KeyboardInfo::~KeyboardInfo()
{
}

} // end of namespace BABYLON
