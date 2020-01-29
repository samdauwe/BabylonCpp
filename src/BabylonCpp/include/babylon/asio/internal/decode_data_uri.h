#ifndef BABYLONCPP_DECODE_DATA_URL_H
#define BABYLONCPP_DECODE_DATA_URL_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <optional>
#include <string>

namespace BABYLON {
namespace asio {

BABYLON_SHARED_EXPORT bool IsBase64JpgDataUri(const std::string& uri);
BABYLON_SHARED_EXPORT ArrayBuffer DecodeBase64JpgDataUri(const std::string& uri);

} // namespace asio
} // namespace BABYLON

#endif // BABYLONCPP_DECODE_DATA_URL_H
