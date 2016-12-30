#ifndef BABYLON_CAMERAS_VR_VR_FOV_H
#define BABYLON_CAMERAS_VR_VR_FOV_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT VRFov {
  float upDegrees;
  float downDegrees;
  float leftDegrees;
  float rightDegrees;
}; // end of struct WebVRFov

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VR_VR_FOV_H