#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H

#include <string>

namespace BABYLON {

struct ICameraPOV {

  virtual void setPOV() = 0;

  virtual std::string getCurrentActiveCamera() const = 0;

  virtual std::string cameraId() const = 0;

}; // end of struct ICameraPOV

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ICAMERA_POV_H
