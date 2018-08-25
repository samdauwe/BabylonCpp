#ifndef BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
#define BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhotoDomeOptions {
  nullable_t<unsigned int> resolution = nullopt_t;
  nullable_t<unsigned int> size       = nullopt_t;
  nullable_t<bool> useDirectMapping   = nullopt_t;
}; // end of struct PhotoDomeOptions

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
