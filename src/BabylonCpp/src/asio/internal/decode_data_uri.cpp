#include <babylon/misc/string_tools.h>
#include <babylon/utils/base64.h>
#include <cassert>

#include <babylon/asio/internal/decode_data_uri.h>

namespace BABYLON {
namespace asio {

std::string base64JpgDataprefix = "data:image/jpg;base64,";

bool IsBase64JpgDataUri(const std::string& uri)
{
  return StringTools::startsWith(uri, base64JpgDataprefix);
}

ArrayBuffer DecodeBase64JpgDataUri(const std::string& uri)
{
  assert(IsBase64JpgDataUri(uri));
  std::string base64Data = uri.substr(base64JpgDataprefix.size());

  std::string dataAsString = Base64::atob(base64Data);
  ArrayBuffer dataAsBuffer;
  for (char c : dataAsString)
    dataAsBuffer.push_back(static_cast<uint8_t>(c));

  return dataAsBuffer;
}

} // namespace asio
} // namespace BABYLON
