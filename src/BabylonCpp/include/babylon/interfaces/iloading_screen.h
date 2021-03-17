#ifndef BABYLON_INTERFACES_ILOADING_SCREEN_H
#define BABYLON_INTERFACES_ILOADING_SCREEN_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ILoadingScreen {

public:
  virtual ~ILoadingScreen()       = default;
  virtual void displayLoadingUI() = 0;
  virtual void hideLoadingUI()    = 0;

public:
  std::string loadingUIBackgroundColor;
  std::string loadingUIText;

}; // end of class ILoadingScreen

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_ILOADING_SCREEN_H
