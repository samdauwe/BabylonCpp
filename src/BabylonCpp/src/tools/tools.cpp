#include <babylon/tools/tools.h>

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
#pragma GCC diagnostic ignored "-Wgnu"
#endif
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include <babylon/utils/stb_image.h>
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

#include <babylon/core/filesystem.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/loading/progress_event.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

::std::function<string_t(string_t url)> Tools::PreprocessUrl
  = [](string_t url) {
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

int Tools::CeilingPOT(int x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

int Tools::FloorPOT(int x)
{
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  return x - (x >> 1);
}

int Tools::NearestPOT(int x)
{
  auto c = Tools::CeilingPOT(x);
  auto f = Tools::FloorPOT(x);
  return (c - x) > (x - f) ? f : c;
}

int Tools::GetExponentOfTwo(int value, int max, unsigned int mode)
{
  int pot;

  switch (mode) {
    case EngineConstants::SCALEMODE_FLOOR:
      pot = Tools::FloorPOT(value);
      break;
    case EngineConstants::SCALEMODE_NEAREST:
      pot = Tools::NearestPOT(value);
      break;
    case EngineConstants::SCALEMODE_CEILING:
      pot = Tools::CeilingPOT(value);
      break;
  }

  return ::std::min(pot, max);
}

string_t Tools::GetFilename(const string_t& path)
{
  const auto index = String::lastIndexOf(path, "/");
  if (index < 0) {
    return path;
  }

  return path.substr(static_cast<size_t>(index) + 1);
}

string_t Tools::GetFolderPath(const string_t& uri,
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

MinMax Tools::ExtractMinAndMaxIndexed(const Float32Array& positions,
                                      const Uint32Array& indices,
                                      size_t indexStart, size_t indexCount,
                                      const Nullable<Vector2>& bias)
{
  Vector3 minimum(numeric_limits_t<float>::max(),
                  numeric_limits_t<float>::max(),
                  numeric_limits_t<float>::max());
  Vector3 maximum(numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest());

  for (size_t index = indexStart; index < indexStart + indexCount; ++index) {
    Vector3 current(positions[indices[index] * 3],
                    positions[indices[index] * 3 + 1],
                    positions[indices[index] * 3 + 2]);

    minimum = Vector3::Minimize(current, minimum);
    maximum = Vector3::Maximize(current, maximum);
  }

  if (bias) {
    const auto& _bias = *bias;
    minimum.x -= minimum.x * _bias.x + _bias.y;
    minimum.y -= minimum.y * _bias.x + _bias.y;
    minimum.z -= minimum.z * _bias.x + _bias.y;
    maximum.x += maximum.x * _bias.x + _bias.y;
    maximum.y += maximum.y * _bias.x + _bias.y;
    maximum.z += maximum.z * _bias.x + _bias.y;
  }

  return {minimum, maximum};
}

MinMax Tools::ExtractMinAndMax(const Float32Array& positions, size_t start,
                               size_t count, const Nullable<Vector2>& bias,
                               Nullable<unsigned int> stride)
{
  Vector3 minimum(numeric_limits_t<float>::max(),
                  numeric_limits_t<float>::max(),
                  numeric_limits_t<float>::max());
  Vector3 maximum(numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest());

  if (!stride) {
    stride = 3;
  }

  auto _stride = *stride;
  for (size_t index = start; index < start + count; ++index) {
    Vector3 current(positions[index * _stride], positions[index * _stride + 1],
                    positions[index * _stride + 2]);

    minimum = Vector3::Minimize(current, minimum);
    maximum = Vector3::Maximize(current, maximum);
  }

  if (bias) {
    const auto& _bias = *bias;
    minimum.x -= minimum.x * _bias.x + _bias.y;
    minimum.y -= minimum.y * _bias.x + _bias.y;
    minimum.z -= minimum.z * _bias.x + _bias.y;
    maximum.x += maximum.x * _bias.x + _bias.y;
    maximum.y += maximum.y * _bias.x + _bias.y;
    maximum.z += maximum.z * _bias.x + _bias.y;
  }

  return {minimum, maximum};
}

MinMaxVector2 Tools::ExtractMinAndMaxVector2(
  const ::std::function<Nullable<Vector2>(std::size_t index)>& feeder,
  const Nullable<Vector2>& bias)
{
  Vector2 minimum(numeric_limits_t<float>::max(),
                  numeric_limits_t<float>::max());
  Vector2 maximum(numeric_limits_t<float>::lowest(),
                  numeric_limits_t<float>::lowest());

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
      auto floorX = static_cast<std::uint8_t>(::std::floor(xf / (size / 8.f)));
      auto floorY = static_cast<std::uint8_t>(::std::floor(yf / (size / 8.f)));

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
      ::std::floor((randomNumbers[i] * (0.02f - 0.95f) + 0.95f) * 255.f));
    imageData[i]     = value;
    imageData[i + 1] = value;
    imageData[i + 2] = value;
    imageData[i + 3] = 255;
  }

  return Image(imageData, width, height, depth, mode);
}

string_t Tools::CleanUrl(string_t url)
{
  String::replaceInPlace(url, "#", "%23");
  return url;
}

string_t Tools::DecodeURIComponent(const string_t& s)
{
  return s;
}

void Tools::LoadImage(string_t url,
                      const ::std::function<void(const Image& img)>& onLoad,
                      const ::std::function<void(const string_t& msg)>& onError,
                      bool flipVertically)
{
  url = Tools::CleanUrl(url);

  url = Tools::PreprocessUrl(url);

  if (String::startsWith(url, "file:")) {
    using stbi_ptr
      = unique_ptr_t<unsigned char, ::std::function<void(unsigned char*)>>;

    int w, h, n;
    stbi_set_flip_vertically_on_load(flipVertically);
    stbi_ptr data(stbi_load(url.substr(5).c_str(), &w, &h, &n, STBI_rgb_alpha),
                  [](unsigned char* _data) {
                    if (_data) {
                      stbi_image_free(_data);
                    }
                  });

    if (!data) {
      onError("Error loading image from file " + url);
      return;
    }

    n = STBI_rgb_alpha;
    Image image(data.get(), w * h * n, w, h, n, (n == 3) ? GL::RGB : GL::RGBA);
    onLoad(image);
  }
}

void Tools::LoadFile(
  string_t url,
  const ::std::function<void(const string_t& data,
                             const string_t& responseURL)>& onSuccess,
  const ::std::function<void(const ProgressEvent& event)>& onProgress,
  bool useArrayBuffer,
  const ::std::function<void(const string_t& exception)>& onError)
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
    onError("Unable to load file from location " + url);
  }
}

void Tools::ReadFile(
  string_t fileToLoad,
  const ::std::function<void(const string_t& data,
                             const string_t& responseURL)>& callback,
  const ::std::function<void(const ProgressEvent& event)>& onProgress,
  bool useArrayBuffer)
{
  if (!Filesystem::exists(fileToLoad)) {
    BABYLON_LOGF_ERROR("Tools", "Error while reading file: %s",
                       fileToLoad.c_str());
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
    // Not implemented yet
  }
}

void Tools::CheckExtends(Vector3& v, Vector3& min, Vector3& max)
{
  if (v.x < min.x) {
    min.x = v.x;
  }
  if (v.y < min.y) {
    min.y = v.y;
  }
  if (v.z < min.z) {
    min.z = v.z;
  }

  if (v.x > max.x) {
    max.x = v.x;
  }
  if (v.y > max.y) {
    max.y = v.y;
  }
  if (v.z > max.z) {
    max.z = v.z;
  }
}

string_t Tools::RandomId()
{
  // from
  // http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
  string_t randomId = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  const array_t<unsigned int, 16> yconv{
    {8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11}};
  const array_t<char, 16> hex{{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                               'a', 'b', 'c', 'd', 'e', 'f'}};
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

void Tools::SetImmediate(const ::std::function<void()>& immediate)
{
  if (immediate) {
    immediate();
  }
}

void Tools::DumpFramebuffer(int /*width*/, int /*height*/, Engine* /*engine*/)
{
}

void Tools::GetCurrentGradient(
  float ratio, const vector_t<IValueGradient*>& gradients,
  const ::std::function<void(IValueGradient* current, IValueGradient* next,
                             float scale)>& updateFunc)
{
  for (size_t gradientIndex = 0; gradientIndex < gradients.size() - 1;
       ++gradientIndex) {
    auto currentGradient = gradients[gradientIndex];
    auto nextGradient    = gradients[gradientIndex + 1];

    if (ratio >= currentGradient->gradient && ratio <= nextGradient->gradient) {
      const auto scale = (ratio - currentGradient->gradient)
                         / (nextGradient->gradient - currentGradient->gradient);
      updateFunc(currentGradient, nextGradient, scale);
    }
  }
}

} // end of namespace BABYLON
