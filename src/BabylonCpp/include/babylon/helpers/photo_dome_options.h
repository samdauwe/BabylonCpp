#ifndef BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
#define BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhotoDomeOptions {
  Nullable<unsigned int> resolution = nullptr;
  Nullable<unsigned int> size       = nullptr;
  Nullable<bool> useDirectMapping   = nullptr;
}; // end of struct PhotoDomeOptions

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
