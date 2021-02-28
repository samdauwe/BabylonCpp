#include <babylon/events/keyboard_info_pre.h>

namespace BABYLON {

KeyboardInfoPre::KeyboardInfoPre(unsigned int iType, const KeyboardEvent& iEvent)
    : KeyboardInfo{iType, iEvent}, skipOnPointerObservable{false}
{
}

KeyboardInfoPre::~KeyboardInfoPre() = default;

} // end of namespace BABYLON
