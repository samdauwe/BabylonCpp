#ifndef BABYLON_CAMERAS_ICAMERA_INPUT_H
#define BABYLON_CAMERAS_ICAMERA_INPUT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class ICanvas;

template <class TCamera>
struct BABYLON_SHARED_EXPORT ICameraInput {
  TCamera* camera                           = nullptr;
  bool hasCheckInputs                       = true;
  virtual const char* getClassName() const  = 0;
  virtual const char* getSimpleName() const = 0;
  virtual void attachControl(ICanvas* canvas, bool noPreventDefault = false)
    = 0;
  virtual void detachControl(ICanvas* canvas) = 0;
  virtual void checkInputs()                  = 0;

}; // end of class ICameraInput<TCamera>

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ICAMERA_INPUT_H
