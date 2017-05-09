#include <babylon/extensions/noisegeneration/perlin_noise.h>

namespace BABYLON {
namespace Extensions {

PerlinNoise::PerlinNoise(uint32_t seed)
{
  if (!seed) {
    seed = time(0);
  }

  auto mid_range = p.begin() + 256;

  std::mt19937 engine(seed);

  // Generate sequential numbers in the lower half
  std::iota(p.begin(), mid_range, 0);
  // Shuffle the lower half
  std::shuffle(p.begin(), mid_range, engine);
  // Copy the lower half to the upper half, p now has the numbers 0-255,
  // shuffled, and duplicated
  std::copy(p.begin(), mid_range, mid_range);
}

double PerlinNoise::noise(double x, double y, double z) const
{
  // See here for algorithm: http://cs.nyu.edu/~perlin/noise/

  const int32_t X = static_cast<int32_t>(std::floor(x)) & 255;
  const int32_t Y = static_cast<int32_t>(std::floor(y)) & 255;
  const int32_t Z = static_cast<int32_t>(std::floor(z)) & 255;

  x -= std::floor(x);
  y -= std::floor(y);
  z -= std::floor(z);

  const double u = fade(x);
  const double v = fade(y);
  const double w = fade(z);

  const auto A  = p[static_cast<unsigned>(X)] + Y;
  const auto AA = p[static_cast<unsigned>(A)] + Z;
  const auto AB = p[static_cast<unsigned>(A + 1)] + Z;
  const auto B  = p[static_cast<unsigned>(X + 1)] + Y;
  const auto BA = p[static_cast<unsigned>(B)] + Z;
  const auto BB = p[static_cast<unsigned>(B + 1)] + Z;

  const auto PAA  = p[static_cast<unsigned>(AA)];
  const auto PBA  = p[static_cast<unsigned>(BA)];
  const auto PAB  = p[static_cast<unsigned>(AB)];
  const auto PBB  = p[static_cast<unsigned>(BB)];
  const auto PAA1 = p[static_cast<unsigned>(AA + 1)];
  const auto PBA1 = p[static_cast<unsigned>(BA + 1)];
  const auto PAB1 = p[static_cast<unsigned>(AB + 1)];
  const auto PBB1 = p[static_cast<unsigned>(BB + 1)];

  const auto a
    = lerp(v, lerp(u, grad(PAA, x, y, z), grad(PBA, x - 1, y, z)),
           lerp(u, grad(PAB, x, y - 1, z), grad(PBB, x - 1, y - 1, z)));

  const auto b = lerp(
    v, lerp(u, grad(PAA1, x, y, z - 1), grad(PBA1, x - 1, y, z - 1)),
    lerp(u, grad(PAB1, x, y - 1, z - 1), grad(PBB1, x - 1, y - 1, z - 1)));

  return lerp(w, a, b);
}

PerlinNoiseOctave::PerlinNoiseOctave(int octaves, uint32_t seed)
    : _perlinNoise{seed}, _octaves{octaves}
{
}

double PerlinNoiseOctave::noise(double x, double y, double z) const
{
  double result = 0.0;
  double amp    = 1.0;

  int i = _octaves;
  while (i--) {
    result += _perlinNoise.noise(x, y, z) * amp;
    x *= 2.0;
    y *= 2.0;
    z *= 2.0;
    amp *= 0.5;
  }

  return result;
}

} // end of namespace Extensions
} // end of namespace BABYLON
