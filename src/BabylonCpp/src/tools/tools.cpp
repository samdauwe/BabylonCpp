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
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include <babylon/utils/stb_image.h>
#if defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

#include <babylon/core/random.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

float Tools::Mix(float a, float b, float alpha)
{
  return a * (1 - alpha) + b * alpha;
}

bool Tools::IsExponentOfTwo(int value)
{
  int count = 1;

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

  return std::min(pot, max);
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
                                      size_t indexStart, size_t indexCount)
{
  Vector3 minimum(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  Vector3 maximum(-std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max());

  for (size_t index = indexStart; index < indexStart + indexCount; ++index) {
    Vector3 current(positions[indices[index] * 3],
                    positions[indices[index] * 3 + 1],
                    positions[indices[index] * 3 + 2]);

    minimum = Vector3::Minimize(current, minimum);
    maximum = Vector3::Maximize(current, maximum);
  }

  return {minimum, maximum};
}

MinMax Tools::ExtractMinAndMaxIndexed(const Float32Array& positions,
                                      const Uint32Array& indices,
                                      size_t indexStart, size_t indexCount,
                                      const Vector2& bias)
{
  MinMax minMax = Tools::ExtractMinAndMaxIndexed(positions, indices, indexStart,
                                                 indexCount);

  Vector3& minimum = minMax.min;
  Vector3& maximum = minMax.max;

  minimum.x -= minimum.x * bias.x + bias.y;
  minimum.y -= minimum.y * bias.x + bias.y;
  minimum.z -= minimum.z * bias.x + bias.y;
  maximum.x += maximum.x * bias.x + bias.y;
  maximum.y += maximum.y * bias.x + bias.y;
  maximum.z += maximum.z * bias.x + bias.y;

  return {minimum, maximum};
}

MinMax Tools::ExtractMinAndMax(const Float32Array& positions, size_t start,
                               size_t count, unsigned int stride)
{
  Vector3 minimum(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  Vector3 maximum(-std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max());

  for (size_t index = start; index < start + count; ++index) {
    Vector3 current(positions[index * stride], positions[index * stride + 1],
                    positions[index * stride + 2]);

    minimum = Vector3::Minimize(current, minimum);
    maximum = Vector3::Maximize(current, maximum);
  }

  return {minimum, maximum};
}

MinMax Tools::ExtractMinAndMax(const Float32Array& positions, size_t start,
                               size_t count, const Vector2& bias,
                               unsigned int stride)
{
  auto minMax = Tools::ExtractMinAndMax(positions, start, count, stride);

  auto& minimum = minMax.min;
  auto& maximum = minMax.max;

  minimum.x -= minimum.x * bias.x + bias.y;
  minimum.y -= minimum.y * bias.x + bias.y;
  minimum.z -= minimum.z * bias.x + bias.y;
  maximum.x += maximum.x * bias.x + bias.y;
  maximum.y += maximum.y * bias.x + bias.y;
  maximum.z += maximum.z * bias.x + bias.y;

  return {minimum, maximum};
}

void Tools::LoadImage(
  const std::string& url, const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& msg)>& onError,
  bool flipVertically)
{
  typedef std::unique_ptr<unsigned char, std::function<void(unsigned char*)>>
    stbi_ptr;

  int w, h, n;
  stbi_set_flip_vertically_on_load(flipVertically);
  stbi_ptr data(stbi_load(url.c_str(), &w, &h, &n, STBI_rgb_alpha),
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

void Tools::LoadFile(
  const std::string& /*url*/,
  const std::function<void(const std::string& text)>& /*callback*/,
  const std::function<void()>& /*progressCallBack*/, bool /*useArrayBuffer*/)
{
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

std::string Tools::RandomId()
{
  // from
  // http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
  std::string randomId = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  const std::array<unsigned int, 16> yconv{
    {8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11, 8, 9, 10, 11}};
  const std::array<char, 16> hex{{'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                  '9', 'a', 'b', 'c', 'd', 'e', 'f'}};
  const std::vector<float> randomFloats
    = Math::randomList(0.f, 1.f, randomId.size());
  for (unsigned int i = 0; i < randomId.size(); ++i) {
    const char c = randomId[i];
    if (c == 'x' || c == 'y') {
      const unsigned int r = static_cast<unsigned int>(randomFloats[i] * 16);
      randomId[i]          = (c == 'x') ? hex[r] : hex[yconv[r]];
    }
  }
  return randomId;
}

void Tools::SetImmediate(const std::function<void()>& /*immediate*/)
{
}

void Tools::DumpFramebuffer(int /*width*/, int /*height*/, Engine* /*engine*/)
{
}

} // end of namespace BABYLON
