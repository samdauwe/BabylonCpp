#include <babylon/events/clipboard_info.h>

namespace BABYLON {

ClipboardInfo::ClipboardInfo(unsigned int iType, const ClipboardEvent& iEvent)
    : type{iType}, event{iEvent}
{
}

ClipboardInfo::~ClipboardInfo() = default;

int ClipboardInfo::GetTypeFromCharacter(int keyCode)
{
  auto charCode = keyCode;
  // TODO: add codes for extended ASCII
  switch (charCode) {
    case 67:
      return ClipboardEventTypes::COPY;
    case 86:
      return ClipboardEventTypes::PASTE;
    case 88:
      return ClipboardEventTypes::CUT;
    default:
      return -1;
  }
}

} // end of namespace BABYLON
