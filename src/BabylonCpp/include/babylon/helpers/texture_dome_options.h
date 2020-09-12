#ifndef BABYLON_HELPERS_TEXTURE_DOME_OPTIONS_H
#define BABYLON_HELPERS_TEXTURE_DOME_OPTIONS_H

#include <optional>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TextureDomeOptions {
  std::optional<unsigned int> resolution = std::nullopt;
  std::optional<bool> clickToPlay        = std::nullopt;
  std::optional<bool> autoPlay           = std::nullopt;
  std::optional<bool> loop               = std::nullopt;
  std::optional<float> size              = std::nullopt;
  std::optional<std::string> poster      = std::nullopt;
  std::optional<bool> faceForward        = std::nullopt;
  std::optional<bool> useDirectMapping   = std::nullopt;
  std::optional<bool> halfDomeMode       = std::nullopt;
}; // end of struct TextureDomeOptions

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_TEXTURE_DOME_OPTIONS_H
