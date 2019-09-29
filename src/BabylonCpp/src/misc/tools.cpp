#include <babylon/misc/tools.h>

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
#define NOMINMAX
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
#include <babylon/utils/stb_image.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

#include <babylon/core/filesystem.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/loading/progress_event.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

bool Tools::UseFallbackTexture = true;

std::string Tools::fallbackTexture
  = "data:image/jpg;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/"
    "4QBmRXhpZgAATU0AKgAAAAgABAEaAAUAAAABAAAAPgEbAAUAAAABAAAARgEoAAMAAAABAAIAAA"
    "ExAAIAAAAQAAAATgAAAAAAAABgAAAAAQAAAGAAAAABcGFpbnQubmV0IDQuMC41AP/"
    "bAEMABAIDAwMCBAMDAwQEBAQFCQYFBQUFCwgIBgkNCw0NDQsMDA4QFBEODxMPDAwSGBITFRYXF"
    "xcOERkbGRYaFBYXFv/"
    "bAEMBBAQEBQUFCgYGChYPDA8WFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWF"
    "hYWFhYWFhYWFhYWFv/AABEIAQABAAMBIgACEQEDEQH/"
    "xAAfAAABBQEBAQEBAQAAAAAAAAAAAQIDBAUGBwgJCgv/"
    "xAC1EAACAQMDAgQDBQUEBAAAAX0BAgMABBEFEiExQQYTUWEHInEUMoGRoQgjQrHBFVLR8CQzYn"
    "KCCQoWFxgZGiUmJygpKjQ1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoOE"
    "hYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4eLj5O"
    "Xm5+jp6vHy8/T19vf4+fr/xAAfAQADAQEBAQEBAQEBAAAAAAAAAQIDBAUGBwgJCgv/"
    "xAC1EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVict"
    "EKFiQ04SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqC"
    "g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri4+"
    "Tl5ufo6ery8/T19vf4+fr/2gAMAwEAAhEDEQA/APH6KKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FCiiigD6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++gooooA+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gUKKKKAPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76CiiigD5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BQooooA+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "voKKKKAPl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FCiiigD6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++gooooA+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gUKKKKAPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76CiiigD5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BQooooA+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "voKKKKAPl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FCiiigD6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++gooooA+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/"
    "gUKKKKAPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76Pl+iiivuj+BT6gooor4U/"
    "vo+X6KKK+6P4FPqCiiivhT++j5fooor7o/gU+oKKKK+FP76P//Z";

std::function<std::string(std::string url)> Tools::PreprocessUrl
  = [](std::string url) {
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
      absolutePath = Filesystem::absolutePath("../assets/" + url);
      if (Filesystem::exists(absolutePath)) {
        return String::concat("file:", absolutePath);
      }
      return url;
    };

void Tools::FetchToRef(int u, int v, int width, int height,
                       const Uint8Array& pixels, Color4& color)
{
  auto wrappedU = ((std::abs(u) * width) % width);
  auto wrappedV = ((std::abs(v) * height) % height);

  auto position = static_cast<size_t>((wrappedU + wrappedV * width) * 4);
  color.r       = pixels[position] / 255.f;
  color.g       = pixels[position + 1] / 255.f;
  color.b       = pixels[position + 2] / 255.f;
  color.a       = pixels[position + 3] / 255.f;
}

float Tools::Mix(float a, float b, float alpha)
{
  return a * (1 - alpha) + b * alpha;
}

bool Tools::IsExponentOfTwo(size_t value)
{
  size_t count = 1;

  do {
    count *= 2;
  } while (count < value);

  return count == value;
}

std::string Tools::GetFilename(const std::string& path)
{
  const auto index = String::lastIndexOf(path, "/");
  if (index < 0) {
    return path;
  }

  return path.substr(static_cast<size_t>(index) + 1);
}

std::string Tools::GetFolderPath(const std::string& uri,
                                 bool returnUnchangedIfNoSlash)
{
  const auto index = String::lastIndexOf(uri, "/");
  if (index < 0) {
    if (returnUnchangedIfNoSlash) {
      return uri;
    }
    return "";
  }

  return uri.substr(0, static_cast<size_t>(index) + 1);
}

float Tools::ToDegrees(float angle)
{
  return angle * 180.f / Math::PI;
}

float Tools::ToRadians(float angle)
{
  return angle * Math::PI / 180.f;
}

MinMaxVector2 Tools::ExtractMinAndMaxVector2(
  const std::function<std::optional<Vector2>(std::size_t index)>& feeder,
  const std::optional<Vector2>& bias)
{
  Vector2 minimum(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  Vector2 maximum(std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest());

  std::size_t i = 0;
  auto cur      = feeder(i++);
  while (cur) {
    minimum = Vector2::Minimize(*cur, minimum);
    maximum = Vector2::Maximize(*cur, maximum);

    cur = feeder(i++);
  }

  if (bias) {
    const auto& _bias = *bias;
    minimum.x -= minimum.x * _bias.x + _bias.y;
    minimum.y -= minimum.y * _bias.x + _bias.y;
    maximum.x += maximum.x * _bias.x + _bias.y;
    maximum.y += maximum.y * _bias.x + _bias.y;
  }

  return {minimum, maximum};
}

Image Tools::CreateCheckerboardImage(unsigned int size)
{
  const int width         = static_cast<int>(size);
  const int height        = static_cast<int>(size);
  const int depth         = 4;
  const unsigned int mode = GL::RGBA;

  Uint8Array imageData(static_cast<std::size_t>(width * height * depth));

  std::uint8_t r = 0;
  std::uint8_t g = 0;
  std::uint8_t b = 0;

  for (unsigned int x = 0; x < size; ++x) {
    float xf = static_cast<float>(x);
    for (unsigned int y = 0; y < size; ++y) {
      float yf              = static_cast<float>(y);
      unsigned int position = (x + size * y) * 4;
      auto floorX = static_cast<std::uint8_t>(std::floor(xf / (size / 8.f)));
      auto floorY = static_cast<std::uint8_t>(std::floor(yf / (size / 8.f)));

      if ((floorX + floorY) % 2 == 0) {
        r = g = b = 255;
      }
      else {
        r = 255;
        g = b = 0;
      }

      imageData[position + 0] = r;
      imageData[position + 1] = g;
      imageData[position + 2] = b;
      imageData[position + 3] = 255;
    }
  }

  return Image(imageData, width, height, depth, mode);
}

Image Tools::CreateNoiseImage(unsigned int size)
{
  const int width         = static_cast<int>(size);
  const int height        = static_cast<int>(size);
  const int depth         = 4;
  const unsigned int mode = GL::RGBA;

  const std::size_t totalPixelsCount = size * size * 4;
  Uint8Array imageData(totalPixelsCount);
  const auto randomNumbers = Math::randomList(0.f, 1.f, totalPixelsCount);

  std::uint8_t value = 0;
  for (std::size_t i = 0; i < totalPixelsCount; i += 4) {
    value = static_cast<std::uint8_t>(
      std::floor((randomNumbers[i] * (0.02f - 0.95f) + 0.95f) * 255.f));
    imageData[i]     = value;
    imageData[i + 1] = value;
    imageData[i + 2] = value;
    imageData[i + 3] = 255;
  }

  return Image(imageData, width, height, depth, mode);
}

std::string Tools::CleanUrl(std::string url)
{
  String::replaceInPlace(url, "#", "%23");
  return url;
}

std::string Tools::DecodeURIComponent(const std::string& s)
{
  return s;
}

Image Tools::ArrayBufferToImage(const ArrayBuffer& buffer, bool flipVertically)
{
  if (buffer.empty()) {
    return Image();
  }

  using stbi_ptr
    = std::unique_ptr<unsigned char, std::function<void(unsigned char*)>>;

  auto bufferSize = static_cast<int>(buffer.size());
  auto w = -1, h = -1, n = -1;
  auto req_comp = STBI_rgb_alpha;
  stbi_set_flip_vertically_on_load(flipVertically);
  stbi_ptr data(
    stbi_load_from_memory(buffer.data(), bufferSize, &w, &h, &n, req_comp),
    [](unsigned char* _data) {
      if (_data) {
        stbi_image_free(_data);
      }
    });

  if (!data) {
    return Image();
  }

  n = STBI_rgb_alpha;
  return Image(data.get(), w * h * n, w, h, n, (n == 3) ? GL::RGB : GL::RGBA);
}

void Tools::LoadImageFromUrl(
  std::string url, const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError,
  bool flipVertically)
{
  url = Tools::CleanUrl(url);

  url = Tools::PreprocessUrl(url);

  if (String::startsWith(url, "file:")) {
    using stbi_ptr
      = std::unique_ptr<unsigned char, std::function<void(unsigned char*)>>;

    int w = -1, h = -1, n = -1;
    stbi_set_flip_vertically_on_load(flipVertically);
    stbi_ptr data(stbi_load(url.substr(5).c_str(), &w, &h, &n, STBI_rgb_alpha),
                  [](unsigned char* _data) {
                    if (_data) {
                      stbi_image_free(_data);
                    }
                  });

    if (!data && onError) {
      onError("Error loading image from file " + url, "");
      return;
    }

    n = STBI_rgb_alpha;
    Image image(data.get(), w * h * n, w, h, n, (n == 3) ? GL::RGB : GL::RGBA);
    onLoad(image);
  }
}

void Tools::LoadImageFromBuffer(
  const std::variant<std::string, ArrayBuffer, Image>& input,
  const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError)
{
  if (!onLoad) {
    return;
  }

  if (std::holds_alternative<ArrayBuffer>(input)) {
    onLoad(Tools::ArrayBufferToImage(std::get<ArrayBuffer>(input)));
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

void Tools::LoadFile(
  std::string url,
  const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress,
  bool useArrayBuffer,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError)
{
  url = Tools::CleanUrl(url);

  url = Tools::PreprocessUrl(url);

  // If file and file input are set
  if (String::startsWith(url, "file:")) {
    const auto fileName = url.substr(5);
    if (!fileName.empty()) {
      Tools::ReadFile(fileName, onSuccess, onProgress, useArrayBuffer);
      return;
    }
  }

  // Report error
  if (onError) {
    onError("Unable to load file from location " + url, "");
  }
}

void Tools::ReadFile(
  std::string fileToLoad,
  const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                           const std::string& responseURL)>& callback,
  const std::function<void(const ProgressEvent& event)>& onProgress,
  bool useArrayBuffer)
{
  if (!Filesystem::exists(fileToLoad)) {
    BABYLON_LOGF_ERROR("Tools", "Error while reading file: %s",
                       fileToLoad.c_str())
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

std::string Tools::RandomId()
{
  // from
  // http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
  std::string randomId = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  const std::array<unsigned int, 16> yconv{
    {8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11}};
  const std::array<char, 16> hex{{'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                  '9', 'a', 'b', 'c', 'd', 'e', 'f'}};
  const auto randomFloats = Math::randomList(0.f, 1.f, randomId.size());
  for (unsigned int i = 0; i < randomId.size(); ++i) {
    const char c = randomId[i];
    if (c == 'x' || c == 'y') {
      const unsigned int r = static_cast<unsigned int>(randomFloats[i] * 16);
      randomId[i]          = (c == 'x') ? hex[r] : hex[yconv[r]];
    }
  }
  return randomId;
}

bool Tools::IsBase64(const std::string& uri)
{
  return uri.size() < 5 ? false : uri.substr(0, 5) == "data:";
}

ArrayBuffer Tools::DecodeBase64(const std::string& uri)
{
  auto uriSplit = String::split(uri, ',');
  if (uriSplit.size() < 2) {
    return ArrayBuffer();
  }

  const auto decodedString = Base64::atob(uriSplit[1]);
  const auto bufferLength  = decodedString.size();
  Uint8Array bufferView(bufferLength);

  for (size_t i = 0; i < bufferLength; ++i) {
    bufferView[i] = String::charCodeAt(decodedString, i);
  }

  return bufferView;
}

void Tools::SetImmediate(const std::function<void()>& immediate)
{
  if (immediate) {
    immediate();
  }
}

void Tools::DumpFramebuffer(int /*width*/, int /*height*/, Engine* /*engine*/)
{
}

} // end of namespace BABYLON
