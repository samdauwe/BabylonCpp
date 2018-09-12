#ifndef BABYLON_EVENTS_KEYBOARD_INFO_H
#define BABYLON_EVENTS_KEYBOARD_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT KeyboardInfo {

public:
  KeyboardInfo(unsigned int type, const KeyboardEvent& event);
  ~KeyboardInfo();

public:
  unsigned int type;
  KeyboardEvent event;

}; // end of class KeyboardInfo

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_KEYBOARD_INFO_H
