#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H

#include <babylon/babylon_stl.h>

namespace BABYLON {

struct ICameraPOV {

  virtual void setPOV() = 0;

  virtual string_t getCurrentActiveCamera() const = 0;

  virtual string_t cameraId() const = 0;

}; // end of struct ICameraPOV

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H
