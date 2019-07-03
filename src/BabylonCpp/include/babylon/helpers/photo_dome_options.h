#ifndef BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
#define BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhotoDomeOptions {
  std::optional<unsigned int> resolution = std::nullopt;
  std::optional<unsigned int> size       = std::nullopt;
  std::optional<bool> useDirectMapping   = std::nullopt;
  std::optional<bool> faceForward        = std::nullopt;
}; // end of struct PhotoDomeOptions

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_OPTIONS_H
