#include <babylon/extensions/noisegeneration/perlin_noise.h>

#include <algorithm>
#include <numeric>
#include <random>

namespace BABYLON {
namespace Extensions {

// Initialize with the reference values for the permutation vector
PerlinNoise::PerlinNoise()
{
  // Initialize the permutation vector with the reference values
  p = {{151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225, 140, 36,
        103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148, 247, 120, 234, 75,
        0,   26,  197, 62,  94,  252, 219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149,
        56,  87,  174, 20,  125, 136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166,
        77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
        245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187,
        208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
        3,   64,  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212,
        207, 206, 59,  227, 47,  16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248,
        152, 2,   44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
        19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228, 251, 34,
        242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,  145, 235, 249, 14,  239, 107,
        49,  192, 214, 31,  181, 199, 106, 157, 184, 84,  204, 176, 115, 121, 50,  45,  127, 4,
        150, 254, 138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,
        215, 61,  156, 180}};
}

PerlinNoise::PerlinNoise(uint32_t seed)
{
  if (!seed) {
    seed = static_cast<uint32_t>(time(nullptr));
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

double PerlinNoise::noise(double x) const
{
  return noise(x, 0, 0);
}

double PerlinNoise::noise(double x, double y) const
{
  return noise(x, y, 0);
}

double PerlinNoise::noise(double x, double y, double z) const
{
  // See here for algorithm: http://cs.nyu.edu/~perlin/noise/

  // Find the unit cube that contains the point
  const int32_t X = static_cast<int32_t>(std::floor(x)) & 255;
  const int32_t Y = static_cast<int32_t>(std::floor(y)) & 255;
  const int32_t Z = static_cast<int32_t>(std::floor(z)) & 255;

  // Find relative x, y,z of point in cube
  x -= std::floor(x);
  y -= std::floor(y);
  z -= std::floor(z);

  // Compute fade curves for each of x, y, z
  const double u = fade(x);
  const double v = fade(y);
  const double w = fade(z);

  // Hash coordinates of the 8 cube corners
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

  // Add blended results from 8 corners of cube
  const auto a = lerp(v, lerp(u, grad(PAA, x, y, z), grad(PBA, x - 1, y, z)),
                      lerp(u, grad(PAB, x, y - 1, z), grad(PBB, x - 1, y - 1, z)));

  const auto b = lerp(v, lerp(u, grad(PAA1, x, y, z - 1), grad(PBA1, x - 1, y, z - 1)),
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
