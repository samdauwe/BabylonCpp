#include <babylon/loading/glTF/2.0/gltf_loader_utils.h>

#include <babylon/core/string.h>
#include <babylon/utils/base64.h>

namespace BABYLON {
namespace GLTF2 {

bool GLTFUtils::IsBase64(const string_t& uri)
{
  return uri.size() < 5 ? false : uri.substr(0, 5) == "data:";
}

Uint8Array GLTFUtils::DecodeBase64(const string_t& uri)
{
  const auto uriSplit = String::split(uri, ',');
  if (uriSplit.empty()) {
    return Uint8Array();
  }

  const auto decodedString = base64_atob(uriSplit[1]);
  const auto bufferLength  = decodedString.size();
  auto buffer              = Uint8Array(bufferLength);

  for (size_t i = 0; i < bufferLength; i++) {
    buffer[i] = String::charCodeAt(decodedString, i);
  }

  return buffer;
}

bool GLTFUtils::ValidateUri(const string_t& uri)
{
  return (!String::contains(uri, ".."));
}

} // end of namespace GLTF2
} // end of namespace BABYLON
