#include <babylon/misc/file_tools.h>
#include <future>

#define STB_IMAGE_IMPLEMENTATION
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
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
#include <babylon/core/string.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/loading/progress_event.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

std::function<std::string(std::string url)> FileTools::PreprocessUrl = [](std::string url) {
  if (String::startsWith(url, "file:")) {
    url = url.substr(5);
  }
  // Check if the file is locally available
  // - Check in local folder
  auto absolutePath = Filesystem::absolutePath(url);
  if (Filesystem::exists(absolutePath)) {
    return String::concat("file:", absolutePath);
  }
  // - Check in assets folder
  absolutePath = Filesystem::absolutePath(BABYLON::assets_folder() + url);
  if (Filesystem::exists(absolutePath)) {
    return String::concat("file:", absolutePath);
  }
  return url;
};

std::string FileTools::_CleanUrl(std::string url)
{
  String::replaceInPlace(url, "#", "%23");
  return url;
}

std::optional<Image> LoadImage_Stbi_Impl(
  const char *filename,
  int nb_desired_channels,
  bool flipVertically)
{

  // Basic usage (see HDR discussion below for HDR usage):
  int w, h, nb_channels_real;
  stbi_set_flip_vertically_on_load(flipVertically);
  unsigned char *data = stbi_load(filename, &w, &h, &nb_channels_real, nb_desired_channels);
  stbi_set_flip_vertically_on_load(false);
  if (data)
  {
    unsigned int glColorMode = (nb_desired_channels == 3) ? GL::RGB : GL::RGBA;
    Image image(data, w * h * nb_desired_channels, w, h, nb_desired_channels, glColorMode);
    stbi_image_free(data);
    return image;
  }
  return std::nullopt;
}


void FileTools::LoadImageFromUrl(
  std::string url, const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  bool flipVertically)
{
  url = FileTools::_CleanUrl(url);
  url = FileTools::PreprocessUrl(url);

  if (!String::startsWith(url, "file:"))
    throw std::runtime_error("FileTools::LoadImageFromUrl only supports file urls");

  const char *filename = url.substr(5).c_str();
  std::optional<Image> image;

  // Try to load RGBA, then load RGB on fail (is this really required? we could instead trust stbi)
  image = LoadImage_Stbi_Impl(filename, STBI_rgb_alpha, flipVertically);
  if (!image)
    image = LoadImage_Stbi_Impl(filename, STBI_rgb, flipVertically);

  if (image)
      onLoad(*image);
  else if (onError)
    onError("Error loading image from file " + url, "");
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
    onLoad(FileTools::ArrayBufferToImage(std::get<ArrayBufferView>(input).uint8Array, invertY));
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
  unsigned char * ucharBuffer = stbi_load_from_memory(buffer.data(), bufferSize, &w, &h, &n, req_comp);
  stbi_set_flip_vertically_on_load(false);

  if (!ucharBuffer)
    return Image();

  n = STBI_rgb_alpha;
  Image image (ucharBuffer, w * h * n, w, h, n, (n == 3) ? GL::RGB : GL::RGBA);
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

void FileTools::ReadFile(
  std::string fileToLoad,
  const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                           const std::string& responseURL)>& callback,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer)
{
  if (!Filesystem::exists(fileToLoad)) {
    BABYLON_LOGF_ERROR("Tools", "Error while reading file: %s", fileToLoad.c_str())
    if (callback) {
      callback("", "");
    }
    if (onProgress) {
      onProgress(ProgressEvent{"ReadFileEvent", true, 100, 100});
    }
    return;
  }

  if (!useArrayBuffer) {
    // Read file contents
    if (callback) {
      callback(Filesystem::readFileContents(fileToLoad.c_str()), "");
    }
    if (onProgress) {
      onProgress(ProgressEvent{"ReadFileEvent", true, 100, 100});
    }
    return;
  }
  else {
    // Read file contents
    if (callback) {
      callback(Filesystem::readBinaryFile(fileToLoad.c_str()), "");
    }
    if (onProgress) {
      onProgress(ProgressEvent{"ReadFileEvent", true, 100, 100});
    }
    return;
  }
}

void FileTools::LoadFile(
  std::string url,
  const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  url = FileTools::_CleanUrl(url);

  url = FileTools::PreprocessUrl(url);

  // If file and file input are set
  if (String::startsWith(url, "file:")) {
    const auto fileName = url.substr(5);
    if (!fileName.empty()) {
      FileTools::ReadFile(fileName, onSuccess, onProgress, useArrayBuffer);
      return;
    }
  }

  // Report error
  if (onError) {
    onError("Unable to load file from location " + url, "");
  }
}

} // end of namespace BABYLON
