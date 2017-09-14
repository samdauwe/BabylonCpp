#ifndef BABYLON_INTERFACES_ILOADING_SCREEN_H
#define BABYLON_INTERFACES_ILOADING_SCREEN_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ILoadingScreen {

public:
  virtual void displayLoadingUI() = 0;
  virtual void hideLoadingUI()    = 0;

public:
  string_t loadingUIBackgroundColor;
  string_t loadingUIText;
}; // end of class ILoadingScreen

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_ILOADING_SCREEN_H