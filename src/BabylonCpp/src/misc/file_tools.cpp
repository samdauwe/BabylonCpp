#include <babylon/misc/file_tools.h>

#define STB_IMAGE_IMPLEMENTATION
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wtype-limits"
#if __GNUC__ > 5
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#endif
#if __GNUC__ > 6
// Use of GNU statement expression extension
#endif
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#if _MSC_VER && !__INTEL_COMPILER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
#include <stb_image/stb_image.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

#include <babylon/core/array_buffer_view.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/logging.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/loading/progress_event.h>
#include <babylon/misc/string_tools.h>
#include <babylon/utils/base64.h>

#include <stdexcept>

namespace BABYLON {

namespace sync_io_impl {

struct ErrorMessage {
  std::string errorMessage;
  explicit ErrorMessage(const std::string& message) : errorMessage(message){};
};

using ArrayBufferOrErrorMessage = std::variant<ArrayBuffer, ErrorMessage>;

static std::string base64JpgDataprefix = "data:image/jpg;base64,";

static bool IsBase64JpgDataUri(const std::string& uri)
{
  return StringTools::startsWith(uri, base64JpgDataprefix);
}

static std::string ArrayBufferToString(const ArrayBuffer& dataUint8)
{
  std::string dataString;
  dataString.resize(dataUint8.size());
  for (size_t i = 0; i < dataUint8.size(); ++i) {
    dataString[i] = static_cast<char>(dataUint8[i]);
  }
  dataString = BABYLON::StringTools::replace(dataString, "\r\n", "\n");
  return dataString;
}

static ArrayBuffer DecodeBase64JpgDataUri(const std::string& uri)
{
  assert(IsBase64JpgDataUri(uri));
  std::string base64Data = uri.substr(base64JpgDataprefix.size());

  std::string dataAsString = Base64::atob(base64Data);
  ArrayBuffer dataAsBuffer;
  for (char c : dataAsString)
    dataAsBuffer.push_back(static_cast<uint8_t>(c));

  return dataAsBuffer;
}

static ArrayBufferOrErrorMessage LoadFileSync_Binary(const std::string& filename,
                                                     const OnProgressFunction& onProgressFunction)
{
  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::ate);
  if (!ifs.good()) {
    std::string message = "LoadFileSync_Binary: Could not open file " + std::string(filename);
    return ErrorMessage(message);
  }

  size_t fileSize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  ArrayBuffer buffer;
  buffer.resize(fileSize);

  size_t alreadyReadSize = 0;
  size_t blockSize       = 1024 * 1024; // blocks of 1MB

  while (alreadyReadSize < fileSize) {
    if (onProgressFunction) {
      onProgressFunction(true, alreadyReadSize, fileSize);
    }

    size_t sizeToRead
      = fileSize - alreadyReadSize > blockSize ? blockSize : fileSize - alreadyReadSize;
    char* bufferPosition = (char*)(buffer.data() + alreadyReadSize);
    ifs.read(bufferPosition, sizeToRead);
    alreadyReadSize += sizeToRead;
  }

  if (onProgressFunction) {
    onProgressFunction(true, alreadyReadSize, fileSize);
  }

  BABYLON_LOG_DEBUG("LoadFileSync_Binary", "Finished loading ", filename.c_str());
  return buffer;
}

static void LoadFileSync_Binary(const std::string& filename,
                                const OnSuccessFunction<ArrayBuffer>& onSuccessFunction,
                                const OnErrorFunction& onErrorFunction,
                                const OnProgressFunction& onProgressFunction)
{
  ArrayBufferOrErrorMessage r = LoadFileSync_Binary(filename, onProgressFunction);
  if (std::holds_alternative<ErrorMessage>(r)) {
    onErrorFunction(std::get<ErrorMessage>(r).errorMessage);
  }
  else {
    onSuccessFunction(std::get<ArrayBuffer>(r));
  }
}

static void
LoadAssetSync_Binary(const std::string& assetPath,
                     const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
                     const OnErrorFunction& onErrorFunction       = nullptr,
                     const OnProgressFunction& onProgressFunction = nullptr)
{
  if (IsBase64JpgDataUri(assetPath)) {
    onSuccessFunction(DecodeBase64JpgDataUri(assetPath));
    return;
  }

  std::string filename = assets_folder() + assetPath;
  LoadFileSync_Binary(filename, onSuccessFunction, onErrorFunction, onProgressFunction);
}

static void LoadFileSync_Text(const std::string& filename,
                              const OnSuccessFunction<std::string>& onSuccessFunction,
                              const OnErrorFunction& onErrorFunction,
                              const OnProgressFunction& onProgressFunction)
{
  ArrayBufferOrErrorMessage r = LoadFileSync_Binary(filename, onProgressFunction);
  if (std::holds_alternative<ErrorMessage>(r)) {
    onErrorFunction(std::get<ErrorMessage>(r).errorMessage);
  }
  else {
    auto onSuccessFunctionArrayBuffer = [onSuccessFunction](const ArrayBuffer& dataUint8) {
      onSuccessFunction(ArrayBufferToString(dataUint8));
    };
    onSuccessFunctionArrayBuffer(std::get<ArrayBuffer>(r));
  }
}

} // end of namespace sync_io_impl

std::string FileTools::PreprocessUrl(const std::string& url)
{
  return url;
}

std::string FileTools::_CleanUrl(std::string url)
{
  StringTools::replaceInPlace(url, "#", "%23");
  return url;
}

void FileTools::LoadAssetSync_Text(const std::string& assetPath,
                                   const OnSuccessFunction<std::string>& onSuccessFunction,
                                   const OnErrorFunction& onErrorFunction,
                                   const OnProgressFunction& onProgressFunction)
{
  std::string filename = assets_folder() + assetPath;
  sync_io_impl::LoadFileSync_Text(filename, onSuccessFunction, onErrorFunction, onProgressFunction);
}

void FileTools::LoadAssetSync_Binary(
  const std::string& assetPath,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction, const OnProgressFunction& onProgressFunction)
{
  sync_io_impl::LoadAssetSync_Binary(assetPath, onSuccessFunction, onErrorFunction,
                                     onProgressFunction);
}

void FileTools::LoadImageFromUrl(
  std::string url, const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  bool flipVertically)
{
  url = FileTools::_CleanUrl(url);
  url = FileTools::PreprocessUrl(url);

  std::string filename = url;

  auto onArrayBufferReceived = [=](const ArrayBuffer& buffer) {
    auto image = ArrayBufferToImage(buffer, flipVertically);
    onLoad(image);
  };
  auto onErrorWrapper = [=](const std::string& errorMessage) { onError(errorMessage, ""); };

  LoadAssetSync_Binary(url, onArrayBufferReceived, onErrorWrapper);
}

void FileTools::LoadImageFromBuffer(
  const std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>& input, bool invertY,
  const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  if (!onLoad) {
    return;
  }

  if (std::holds_alternative<std::string>(input)) {
    onLoad(FileTools::StringToImage(std::get<std::string>(input), invertY));
  }
  else if (std::holds_alternative<ArrayBuffer>(input)) {
    onLoad(FileTools::ArrayBufferToImage(std::get<ArrayBuffer>(input), invertY));
  }
  else if (std::holds_alternative<ArrayBufferView>(input)) {
    onLoad(FileTools::ArrayBufferToImage(std::get<ArrayBufferView>(input).uint8Array(), invertY));
  }
  else if (std::holds_alternative<Image>(input)) {
    onLoad(std::get<Image>(input));
  }
  else {
    auto errorMessage = "Loading image from url not supported";
    if (onError) {
      onError(errorMessage, "");
    }
    else {
      throw std::runtime_error(errorMessage);
    }
  }
}

Image FileTools::ArrayBufferToImage(const ArrayBuffer& buffer, bool flipVertically)
{
  if (buffer.empty()) {
    return Image();
  }
  auto bufferSize = static_cast<int>(buffer.size());
  int w = -1, h = -1, n = -1;
  int req_comp = STBI_rgb_alpha;

  stbi_set_flip_vertically_on_load(flipVertically);
  unsigned char* ucharBuffer
    = stbi_load_from_memory(buffer.data(), bufferSize, &w, &h, &n, req_comp);
  stbi_set_flip_vertically_on_load(false);

  if (!ucharBuffer)
    return Image();

  n = STBI_rgb_alpha;
  Image image(ucharBuffer, w * h * n, w, h, n, (n == 3) ? GL::RGB : GL::RGBA);
  stbi_image_free(ucharBuffer);
  return image;
}

Image FileTools::StringToImage(const std::string& uri, bool flipVertically)
{
  const auto IsDataURI = [](const std::string& in) -> bool {
    std::string header = "data:application/octet-stream;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:image/jpeg;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:image/png;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:image/bmp;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:image/gif;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:text/plain;base64,";
    if (in.find(header) == 0) {
      return true;
    }

    header = "data:application/gltf-buffer;base64,";
    return in.find(header) == 0;
  };

  const auto DecodeDataURI = [](std::vector<unsigned char>* out, std::string& mime_type,
                                const std::string& in, size_t reqBytes, bool checkSize) -> bool {
    std::string header = "data:application/octet-stream;base64,";
    std::string data;
    if (in.find(header) == 0) {
      data = Base64::decode(in.substr(header.size())); // cut mime string.
    }

    if (data.empty()) {
      header = "data:image/jpeg;base64,";
      if (in.find(header) == 0) {
        mime_type = "image/jpeg";
        data      = Base64::decode(in.substr(header.size())); // cut mime string.
      }
    }

    if (data.empty()) {
      header = "data:image/png;base64,";
      if (in.find(header) == 0) {
        mime_type = "image/png";
        data      = Base64::decode(in.substr(header.size())); // cut mime string.
      }
    }

    if (data.empty()) {
      header = "data:image/bmp;base64,";
      if (in.find(header) == 0) {
        mime_type = "image/bmp";
        data      = Base64::decode(in.substr(header.size())); // cut mime string.
      }
    }

    if (data.empty()) {
      header = "data:image/gif;base64,";
      if (in.find(header) == 0) {
        mime_type = "image/gif";
        data      = Base64::decode(in.substr(header.size())); // cut mime string.
      }
    }

    if (data.empty()) {
      header = "data:text/plain;base64,";
      if (in.find(header) == 0) {
        mime_type = "text/plain";
        data      = Base64::decode(in.substr(header.size()));
      }
    }

    if (data.empty()) {
      header = "data:application/gltf-buffer;base64,";
      if (in.find(header) == 0) {
        data = Base64::decode(in.substr(header.size()));
      }
    }

    if (data.empty()) {
      return false;
    }

    if (checkSize) {
      if (data.size() != reqBytes) {
        return false;
      }
      out->resize(reqBytes);
    }
    else {
      out->resize(data.size());
    }
    std::copy(data.begin(), data.end(), out->begin());
    return true;
  };

  const auto LoadImageData = [](Image& image, int req_width, int req_height,
                                const unsigned char* bytes, int size, bool flipVertically) -> bool {
    int w = 0, h = 0, comp = 0, req_comp = 0;

    unsigned char* data = nullptr;

    // force 32-bit textures for common Vulkan compatibility. It appears that
    // some GPU drivers do not support 24-bit images for Vulkan
    req_comp = 4;
    int bits = 8;

    stbi_set_flip_vertically_on_load(flipVertically);

    // It is possible that the image we want to load is a 16bit per channel
    // image We are going to attempt to load it as 16bit per channel, and if it
    // worked, set the image data accodingly. We are casting the returned
    // pointer into unsigned char, because we are representing "bytes". But we
    // are updating the Image metadata to signal that this image uses 2 bytes
    // (16bits) per channel:
    if (stbi_is_16_bit_from_memory(bytes, size)) {
      data = reinterpret_cast<unsigned char*>(
        stbi_load_16_from_memory(bytes, size, &w, &h, &comp, req_comp));
      if (data) {
        bits = 16;
      }
    }

    // at this point, if data is still NULL, it means that the image wasn't
    // 16bit per channel, we are going to load it as a normal 8bit per channel
    // mage as we used to do:
    // if image cannot be decoded, ignore parsing and keep it by its path
    // don't break in this case
    // FIXME we should only enter this function if the image is embedded. If
    // image->uri references
    // an image file, it should be left as it is. Image loading should not be
    // mandatory (to support other formats)
    if (!data)
      data = stbi_load_from_memory(bytes, size, &w, &h, &comp, req_comp);
    if (!data) {
      BABYLON_LOG_WARN("StringToImage",
                       "Unknown image format. STB cannot decode image data for image")
      return false;
    }

    stbi_set_flip_vertically_on_load(false);

    if ((w < 1) || (h < 1)) {
      stbi_image_free(data);
      BABYLON_LOG_ERROR("StringToImage", "Invalid image data for image")
      return false;
    }

    if (req_width > 0) {
      if (req_width != w) {
        stbi_image_free(data);
        BABYLON_LOG_ERROR("StringToImage", "Image width mismatch for image")
        return false;
      }
    }

    if (req_height > 0) {
      if (req_height != h) {
        stbi_image_free(data);
        BABYLON_LOG_ERROR("StringToImage", "Image height mismatch. for image")
        return false;
      }
    }

    image.width  = w;
    image.height = h;
    image.depth  = req_comp;
    image.mode   = (req_comp == 3) ? GL::RGB : GL::RGBA;
    image.data.resize(static_cast<size_t>(w * h * req_comp) * size_t(bits / 8));
    std::copy(data, data + w * h * req_comp * (bits / 8), image.data.begin());
    stbi_image_free(data);

    return true;
  };

  Image image;
  std::vector<unsigned char> img;
  std::string mimeType;
  if (IsDataURI(uri)) {
    if (!DecodeDataURI(&img, mimeType, uri, 0, false)) {
      BABYLON_LOG_ERROR("Failed to decode image from uri: %s", uri.c_str())
      return Image();
    }
  }

  if (LoadImageData(image, 0, 0, &img.at(0), static_cast<int>(img.size()), flipVertically)) {
    return image;
  }

  return Image();
}

// Superfluous params?
constexpr const char* dummyResponseUrl     = "";
constexpr const char* dummyExceptionString = "";
constexpr const char* dummyProgressType    = "";

void FileTools::LoadFile(
  const std::string& url,
  const std::function<void(const std::variant<std::string, ArrayBufferView>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  // LoadFile's signature is extremely complicated for no good reason
  // Let's write some wrappers from the simple callbacks of BABYLON::asio
  // We will need to refactor this later

  std::string url_clean = FileTools::_CleanUrl(url);
  url_clean             = FileTools::PreprocessUrl(url);

  auto onErrorWrapper = [onError](const std::string& errorMessage) {
    if (onError)
      onError(errorMessage, dummyExceptionString);
  };
  auto onProgressWrapper = [onProgress](bool lengthComputable, size_t loaded, size_t total) {
    if (onProgress)
      onProgress({dummyProgressType, lengthComputable, loaded, total});
  };

  if (useArrayBuffer) {
    auto onSuccessWrapper = [onSuccess](const ArrayBuffer& data) {
      if (onSuccess)
        onSuccess(data, dummyResponseUrl);
    };
    LoadAssetSync_Binary(url_clean, onSuccessWrapper, onErrorWrapper, onProgressWrapper);
  }
  else {
    auto onSuccessWrapper = [onSuccess](const std::string& data) {
      if (onSuccess) {
        onSuccess(data, dummyResponseUrl);
      }
    };
    LoadAssetSync_Text(url_clean, onSuccessWrapper, onErrorWrapper, onProgressWrapper);
  }

  // asio::Service_WaitAll_Sync();
  // std::cout << "WaitAll finished\n";
}

} // end of namespace BABYLON
