#ifndef BABYLON_HELPERS_ISCENE_SIZE_H
#define BABYLON_HELPERS_ISCENE_SIZE_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISceneSize {
  int groundSize;
  int skyboxSize;
  Vector3 rootPosition;
}; // end of class ISceneSize

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_ISCENE_SIZE_H
