#include <babylon/misc/tools.h>

#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine_store.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>
#include <babylon/utils/base64.h>

namespace BABYLON {

bool Tools::UseFallbackTexture()
{
  return EngineStore::UseFallbackTexture;
}

void Tools::setUseFallbackTexture(bool value)
{
  EngineStore::UseFallbackTexture = value;
}

std::string Tools::FallbackTexture()
{
  return EngineStore::FallbackTexture;
}

void Tools::setFallbackTexture(const std::string& value)
{
  EngineStore::FallbackTexture = value;
}

void Tools::FetchToRef(int u, int v, int width, int height, const Uint8Array& pixels, Color4& color)
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

std::string Tools::GetFolderPath(const std::string& uri, bool returnUnchangedIfNoSlash)
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
    auto xf = static_cast<float>(x);
    for (unsigned int y = 0; y < size; ++y) {
      auto yf               = static_cast<float>(y);
      unsigned int position = (x + size * y) * 4;
      auto floorX           = static_cast<std::uint8_t>(std::floor(xf / (size / 8.f)));
      auto floorY           = static_cast<std::uint8_t>(std::floor(yf / (size / 8.f)));

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
    value
      = static_cast<std::uint8_t>(std::floor((randomNumbers[i] * (0.02f - 0.95f) + 0.95f) * 255.f));
    imageData[i]     = value;
    imageData[i + 1] = value;
    imageData[i + 2] = value;
    imageData[i + 3] = 255;
  }

  return Image(imageData, width, height, depth, mode);
}

std::string Tools::DecodeURIComponent(const std::string& s)
{
  return s;
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
