#include <babylon/extensions/hexplanetgeneration/utils/fast_simplex_noise.h>

#include <babylon/babylon_constants.h>
#include <babylon/core/random.h>

namespace BABYLON {
namespace Extensions {

FastSimplexNoiseOptions::FastSimplexNoiseOptions()
    : amplitude{1.f}
    , frequency{1.f}
    , max{1.f}
    , min{-1.f}
    , octaves{1}
    , persistence{0.5f}
{
  random = []() -> float { return Math::random(); };
}

FastSimplexNoiseOptions::~FastSimplexNoiseOptions() = default;

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

const float FastSimplexNoise::G2 = (3.f - std::sqrt(3.f)) / 6.f;
const float FastSimplexNoise::G3 = 1.f / 6.f;
const float FastSimplexNoise::G4 = (5.f - std::sqrt(5.f)) / 20.f;

const std::array<std::array<float, 3>, 12> FastSimplexNoise::GRAD3
  = {{{{1.f, 1.f, 0.f}},
      {{-1.f, 1.f, 0.f}},
      {{1.f, -1.f, 0.f}},
      {{-1.f, -1.f, 0.f}},
      {{1.f, 0.f, 1.f}},
      {{-1.f, 0.f, 1.f}},
      {{1.f, 0.f, -1.f}},
      {{-1.f, 0.f, -1.f}},
      {{0.f, 1.f, 1.f}},
      {{0.f, -1.f, -1.f}},
      {{0.f, 1.f, -1.f}},
      {{0.f, -1.f, -1.f}}}};

const std::array<std::array<float, 4>, 32> FastSimplexNoise::GRAD4
  = {{{{0.f, 1.f, 1.f, 1.f}},   {{0.f, 1.f, 1.f, -1.f}},
      {{0.f, 1.f, -1.f, 1.f}},  {{0.f, 1.f, -1.f, -1.f}},
      {{0.f, -1.f, 1.f, 1.f}},  {{0.f, -1.f, 1.f, -1.f}},
      {{0.f, -1.f, -1.f, 1.f}}, {{0.f, -1.f, -1.f, -1.f}},
      {{1.f, 0.f, 1.f, 1.f}},   {{1.f, 0.f, 1.f, -1.f}},
      {{1.f, 0.f, -1.f, 1.f}},  {{1.f, 0.f, -1.f, -1.f}},
      {{-1.f, 0.f, 1.f, 1.f}},  {{-1.f, 0.f, 1.f, -1.f}},
      {{-1.f, 0.f, -1.f, 1.f}}, {{-1.f, 0.f, -1.f, -1.f}},
      {{1.f, 1.f, 0.f, 1.f}},   {{1.f, 1.f, 0.f, -1.f}},
      {{1.f, -1.f, 0.f, 1.f}},  {{1.f, -1, 0.f, -1.f}},
      {{-1.f, 1.f, 0.f, 1.f}},  {{-1.f, 1.f, 0.f, -1.f}},
      {{-1.f, -1.f, 0.f, 1.f}}, {{-1.f, -1.f, 0.f, -1.f}},
      {{1.f, 1.f, 1.f, 0.f}},   {{1.f, 1.f, -1.f, 0.f}},
      {{1.f, -1.f, 1.f, 0.f}},  {{1.f, -1.f, -1.f, 0.f}},
      {{-1.f, 1.f, 1.f, 0.f}},  {{-1.f, 1.f, -1.f, 0.f}},
      {{-1.f, -1.f, 1.f, 0.f}}, {{-1.f, -1.f, -1.f, 0.f}}}};

// -----------------------------------------------------------------------------

FastSimplexNoise::FastSimplexNoise(const FastSimplexNoiseOptions& options)
    : _amplitude{options.amplitude}
    , _frequency{options.frequency}
    , _max{options.max}
    , _min{options.min}
    , _octaves{options.octaves}
    , _persistence{options.persistence}
    , _random{options.random}
{
  if (_min >= _max) {
    // Min must be less than max
    std::swap(_min, _max);
  }

  _scale = [this](float value) {
    return _min + ((value + 1.f) / 2.f) * (_max - _min);
  };

  Uint8Array p(256);
  for (unsigned int i = 0; i < 256; ++i) {
    p[i] = static_cast<uint8_t>(i);
  }

  unsigned int n;
  uint8_t q;
  for (int i = 255; i > 0; i--) {
    auto _i = static_cast<uint8_t>(i);
    n     = static_cast<unsigned int>(std::floor((i + 1) * Math::random()));
    q     = p[_i];
    p[_i] = p[n];
    p[n]  = q;
  }

  _perm.resize(512);
  _permMod12.resize(512);
  for (unsigned int i = 0; i < 512; ++i) {
    _perm[i]      = p[i & 255];
    _permMod12[i] = _perm[i] % 12;
  }
}

FastSimplexNoise::~FastSimplexNoise() =  default;

float FastSimplexNoise::cylindrical(float c, const Float32Array& coords) const
{
  switch (coords.size()) {
    case 2:
      return cylindrical2D(c, coords[0], coords[1]);
    case 3:
      return cylindrical3D(c, coords[0], coords[1], coords[2]);
    default:
      return 0.f;
  }
}

float FastSimplexNoise::cylindrical2D(float c, float x, float y) const
{
  const float nx  = x / c;
  const float r   = c / (2 * Math::PI);
  const float rdx = nx * 2 * Math::PI;
  const float a   = r * std::sin(rdx);
  const float b   = r * std::cos(rdx);

  return scaled3D(a, b, y);
}

float FastSimplexNoise::cylindrical3D(float c, float x, float y, float z) const
{
  const float nx  = x / c;
  const float r   = c / (2.f * Math::PI);
  const float rdx = nx * 2.f * Math::PI;
  const float a   = r * std::sin(rdx);
  const float b   = r * std::cos(rdx);

  return scaled4D(a, b, y, z);
}

float FastSimplexNoise::raw(const Float32Array& coords) const
{
  switch (coords.size()) {
    case 2:
      return raw2D(coords[0], coords[1]);
    case 3:
      return raw3D(coords[0], coords[1], coords[2]);
    case 4:
      return raw4D(coords[0], coords[1], coords[2], coords[3]);
    default:
      return 0.f;
  }
}

float FastSimplexNoise::raw2D(float x, float y) const
{
  float n0, n1, n2; // Noise contributions from the three corners

  // Skew the input space to determine which simplex cell we're in
  float s = (x + y) * 0.5f * (std::sqrt(3.f) - 1.f); // Hairy factor for 2D
  auto i         = static_cast<unsigned int>(std::floor(x + s));
  auto j         = static_cast<unsigned int>(std::floor(y + s));
  float t        = static_cast<float>(i + j) * G2;
  // Unskew the cell origin back to (x,y) space
  float X0 = static_cast<float>(i) - t;
  float Y0 = static_cast<float>(j) - t;
  float x0 = x - X0; // The x,y distances from the cell origin
  float y0 = y - Y0;

  // For the 2D case, the simplex shape is an equilateral triangle.
  // Determine which simplex we are in.
  unsigned int i1,
    j1; // Offsets for second (middle) corner of simplex in (i,j) coords
  if (x0 > y0) { // Lower triangle, XY order: (0,0)->(1,0)->(1,1)
    i1 = 1;
    j1 = 0;
  }
  else { // Upper triangle, YX order: (0,0)->(0,1)->(1,1)
    i1 = 0;
    j1 = 1;
  }

  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
  // c = (3 - sqrt(3)) / 6

  // Offsets for middle corner in (x,y) unskewed coords
  float x1 = x0 - static_cast<float>(i1) + G2;
  float y1 = y0 - static_cast<float>(j1) + G2;
  float x2
    = x0 - 1.f + 2.f * G2; // Offsets for last corner in (x,y) unskewed coords
  float y2 = y0 - 1.f + 2.f * G2;

  // Work out the hashed gradient indices of the three simplex corners
  unsigned int ii  = i & 255;
  unsigned int jj  = j & 255;
  unsigned int gi0 = _permMod12[ii + _perm[jj]];
  unsigned int gi1 = _permMod12[ii + i1 + _perm[jj + j1]];
  unsigned int gi2 = _permMod12[ii + 1 + _perm[jj + 1]];

  // Calculate the contribution from the three corners
  float t0 = 0.5f - x0 * x0 - y0 * y0;
  if (t0 < 0) {
    n0 = 0.f;
  }
  else {
    // (x,y) of 3D gradient used for 2D gradient
    n0 = std::pow(t0, 4.f) * dot2D(GRAD3[gi0], x0, y0);
  }
  float t1 = 0.5f - x1 * x1 - y1 * y1;
  if (t1 < 0.f) {
    n1 = 0.f;
  }
  else {
    n1 = std::pow(t1, 4.f) * dot2D(GRAD3[gi1], x1, y1);
  }
  float t2 = 0.5f - x2 * x2 - y2 * y2;
  if (t2 < 0.f) {
    n2 = 0.f;
  }
  else {
    n2 = std::pow(t2, 4.f) * dot2D(GRAD3[gi2], x2, y2);
  }

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to return values in the interval [-1, 1]
  return 70.14805770654148f * (n0 + n1 + n2);
}

float FastSimplexNoise::raw3D(float x, float y, float z) const
{
  float n0, n1, n2, n3; // Noise contributions from the four corners

  // Skew the input space to determine which simplex cell we're in
  float s        = (x + y + z) / 3.f; // Very nice and simple skew factor for 3D
  auto i         = static_cast<unsigned int>(std::floor(x + s));
  auto j         = static_cast<unsigned int>(std::floor(y + s));
  auto k         = static_cast<unsigned int>(std::floor(z + s));
  float t        = static_cast<float>(i + j + k) * G3;
  // Unskew the cell origin back to (x,y,z) space
  float X0 = static_cast<float>(i) - t;
  float Y0 = static_cast<float>(j) - t;
  float Z0 = static_cast<float>(k) - t;
  // The x,y,z distances from the cell origin
  float x0 = x - X0;
  float y0 = y - Y0;
  float z0 = z - Z0;

  // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
  // Determine which simplex we are in.
  unsigned int i1, j1,
    k1; // Offsets for second corner of simplex in (i,j,k) coords
  unsigned int i2, j2,
    k2; // Offsets for third corner of simplex in (i,j,k) coords
  if (x0 >= y0) {
    if (y0 >= z0) { // X Y Z order
      i1 = i2 = j2 = 1;
      j1 = k1 = k2 = 0;
    }
    else if (x0 >= z0) { // X Z Y order
      i1 = i2 = k2 = 1;
      j1 = k1 = j2 = 0;
    }
    else { // Z X Y order
      k1 = i2 = k2 = 1;
      i1 = j1 = j2 = 0;
    }
  }
  else {           // x0 < y0
    if (y0 < z0) { // Z Y X order
      k1 = j2 = k2 = 1;
      i1 = j1 = i2 = 0;
    }
    else if (x0 < z0) { // Y Z X order
      j1 = j2 = k2 = 1;
      i1 = k1 = i2 = 0;
    }
    else { // Y X Z order
      j1 = i2 = j2 = 1;
      i1 = k1 = k2 = 0;
    }
  }

  // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
  // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
  // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
  // c = 1/6.

  // Offsets for second corner in (x,y,z) coords
  float x1 = x0 - static_cast<float>(i1) + G3;
  float y1 = y0 - static_cast<float>(j1) + G3;
  float z1 = z0 - static_cast<float>(k1) + G3;
  // Offsets for third corner in (x,y,z) coords
  float x2 = x0 - static_cast<float>(i2) + 2.f * G3;
  float y2 = y0 - static_cast<float>(j2) + 2.f * G3;
  float z2 = z0 - static_cast<float>(k2) + 2.f * G3;
  // Offsets for last corner in (x,y,z) coords
  float x3 = x0 - 1.f + 3.f * G3;
  float y3 = y0 - 1.f + 3.f * G3;
  float z3 = z0 - 1.f + 3.f * G3;

  // Work out the hashed gradient indices of the four simplex corners
  unsigned int ii  = i & 255;
  unsigned int jj  = j & 255;
  unsigned int kk  = k & 255;
  unsigned int gi0 = _permMod12[ii + _perm[jj + _perm[kk]]];
  unsigned int gi1 = _permMod12[ii + i1 + _perm[jj + j1 + _perm[kk + k1]]];
  unsigned int gi2 = _permMod12[ii + i2 + _perm[jj + j2 + _perm[kk + k2]]];
  unsigned int gi3 = _permMod12[ii + 1 + _perm[jj + 1 + _perm[kk + 1]]];

  // Calculate the contribution from the four corners
  float t0 = 0.5f - x0 * x0 - y0 * y0 - z0 * z0;
  if (t0 < 0.f) {
    n0 = 0.f;
  }
  else {
    n0 = std::pow(t0, 4.f) * dot3D(GRAD3[gi0], x0, y0, z0);
  }
  float t1 = 0.5f - x1 * x1 - y1 * y1 - z1 * z1;
  if (t1 < 0.f) {
    n1 = 0.f;
  }
  else {
    n1 = std::pow(t1, 4.f) * dot3D(GRAD3[gi1], x1, y1, z1);
  }
  float t2 = 0.5f - x2 * x2 - y2 * y2 - z2 * z2;
  if (t2 < 0.f) {
    n2 = 0.f;
  }
  else {
    n2 = std::pow(t2, 4.f) * dot3D(GRAD3[gi2], x2, y2, z2);
  }
  float t3 = 0.5f - x3 * x3 - y3 * y3 - z3 * z3;
  if (t3 < 0.f) {
    n3 = 0.f;
  }
  else {
    n3 = std::pow(t3, 4.f) * dot3D(GRAD3[gi3], x3, y3, z3);
  }

  // Add contributions from each corner to get the final noise value.
  // The result is scaled to stay just inside [-1,1]
  return 94.68493150681972f * (n0 + n1 + n2 + n3);
}

float FastSimplexNoise::raw4D(float x, float y, float z, float w) const
{
  float n0, n1, n2, n3, n4; // Noise contributions from the five corners

  // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
  float s
    = (x + y + z + w) * (std::sqrt(5.f) - 1.f) / 4.f; // Factor for 4D skewing
  auto i = static_cast<unsigned int>(std::floor(x + s));
  auto j = static_cast<unsigned int>(std::floor(y + s));
  auto k = static_cast<unsigned int>(std::floor(z + s));
  auto l = static_cast<unsigned int>(std::floor(w + s));
  // Factor for 4D unskewing
  float t = static_cast<float>(i + j + k + l) * G4;
  // Unskew the cell origin back to (x,y,z,w) space
  float X0 = static_cast<float>(i) - t;
  float Y0 = static_cast<float>(j) - t;
  float Z0 = static_cast<float>(k) - t;
  float W0 = static_cast<float>(l) - t;
  float x0 = x - X0; // The x,y,z,w distances from the cell origin
  float y0 = y - Y0;
  float z0 = z - Z0;
  float w0 = w - W0;

  // For the 4D case, the simplex is a 4D shape I won't even try to describe. To
  // find out which of the 24 possible simplices we're in, we need to determine
  // the magnitude ordering of x0, y0, z0 and w0. Six pair-wise comparisons are
  // performed between each possible pair of the four coordinates, and the
  // results are used to rank the numbers.
  unsigned int rankx = 0;
  unsigned int ranky = 0;
  unsigned int rankz = 0;
  unsigned int rankw = 0;
  if (x0 > y0) {
    ++rankx;
  }
  else {
    ++ranky;
  }
  if (x0 > z0) {
    ++rankx;
  }
  else {
    ++rankz;
  }
  if (x0 > w0) {
    ++rankx;
  }
  else {
    ++rankw;
  }
  if (y0 > z0) {
    ++ranky;
  }
  else {
    ++rankz;
  }
  if (y0 > w0) {
    ++ranky;
  }
  else {
    ++rankw;
  }
  if (z0 > w0) {
    ++rankz;
  }
  else {
    ++rankw;
  }
  // The integer offsets for the second simplex corner
  unsigned int i1, j1, k1, l1;
  // The integer offsets for the third simplex corner
  unsigned int i2, j2, k2, l2;
  // The integer offsets for the fourth simplex corner
  unsigned int i3, j3, k3, l3;

  // simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
  // Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and
  // x<w impossible. Only the 24 indices which have non-zero entries make any
  // sense. We use a thresholding to set the coordinates in turn from the
  // largest magnitude. Rank 3 denotes the largest coordinate.
  i1 = rankx >= 3 ? 1 : 0;
  j1 = ranky >= 3 ? 1 : 0;
  k1 = rankz >= 3 ? 1 : 0;
  l1 = rankw >= 3 ? 1 : 0;
  // Rank 2 denotes the second largest coordinate.
  i2 = rankx >= 2 ? 1 : 0;
  j2 = ranky >= 2 ? 1 : 0;
  k2 = rankz >= 2 ? 1 : 0;
  l2 = rankw >= 2 ? 1 : 0;
  // Rank 1 denotes the second smallest coordinate.
  i3 = rankx >= 1 ? 1 : 0;
  j3 = ranky >= 1 ? 1 : 0;
  k3 = rankz >= 1 ? 1 : 0;
  l3 = rankw >= 1 ? 1 : 0;

  // The fifth corner has all coordinate offsets = 1, so no need to compute
  // that.
  // Offsets for second corner in (x,y,z,w) coords
  float x1 = x0 - static_cast<float>(i1) + G4;
  float y1 = y0 - static_cast<float>(j1) + G4;
  float z1 = z0 - static_cast<float>(k1) + G4;
  float w1 = w0 - static_cast<float>(l1) + G4;
  // Offsets for third corner in (x,y,z,w) coords
  float x2 = x0 - static_cast<float>(i2) + 2.f * G4;
  float y2 = y0 - static_cast<float>(j2) + 2.f * G4;
  float z2 = z0 - static_cast<float>(k2) + 2.f * G4;
  float w2 = w0 - static_cast<float>(l2) + 2.f * G4;
  // Offsets for fourth corner in (x,y,z,w) coords
  float x3 = x0 - static_cast<float>(i3) + 3.f * G4;
  float y3 = y0 - static_cast<float>(j3) + 3.f * G4;
  float z3 = z0 - static_cast<float>(k3) + 3.f * G4;
  float w3 = w0 - static_cast<float>(l3) + 3.f * G4;
  float x4 = x0 - 1.f + 4.f * G4; // Offsets for last corner in (x,y,z,w) coords
  float y4 = y0 - 1.f + 4.f * G4;
  float z4 = z0 - 1.f + 4.f * G4;
  float w4 = w0 - 1.f + 4.f * G4;

  // Work out the hashed gradient indices of the five simplex corners
  unsigned int ii  = i & 255;
  unsigned int jj  = j & 255;
  unsigned int kk  = k & 255;
  unsigned int ll  = l & 255;
  std::uint8_t gi0 = _perm[ii + _perm[jj + _perm[kk + _perm[ll]]]] % 32;
  std::uint8_t gi1
    = _perm[ii + i1 + _perm[jj + j1 + _perm[kk + k1 + _perm[ll + l1]]]] % 32;
  std::uint8_t gi2
    = _perm[ii + i2 + _perm[jj + j2 + _perm[kk + k2 + _perm[ll + l2]]]] % 32;
  std::uint8_t gi3
    = _perm[ii + i3 + _perm[jj + j3 + _perm[kk + k3 + _perm[ll + l3]]]] % 32;
  std::uint8_t gi4
    = _perm[ii + 1 + _perm[jj + 1 + _perm[kk + 1 + _perm[ll + 1]]]] % 32;

  // Calculate the contribution from the five corners
  const float t0 = 0.5f - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
  if (t0 < 0.f) {
    n0 = 0.f;
  }
  else {
    n0 = std::pow(t0, 4.f) * dot4D(GRAD4[gi0], x0, y0, z0, w0);
  }
  const float t1 = 0.5f - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
  if (t1 < 0.f) {
    n1 = 0.f;
  }
  else {
    n1 = std::pow(t1, 4.f) * dot4D(GRAD4[gi1], x1, y1, z1, w1);
  }
  const float t2 = 0.5f - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
  if (t2 < 0.f) {
    n2 = 0.f;
  }
  else {
    n2 = std::pow(t2, 4.f) * dot4D(GRAD4[gi2], x2, y2, z2, w2);
  }
  const float t3 = 0.5f - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
  if (t3 < 0.f) {
    n3 = 0.f;
  }
  else {
    n3 = std::pow(t3, 4.f) * dot4D(GRAD4[gi3], x3, y3, z3, w3);
  }
  const float t4 = 0.5f - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
  if (t4 < 0.f) {
    n4 = 0.f;
  }
  else {
    n4 = std::pow(t4, 4.f) * dot4D(GRAD4[gi4], x4, y4, z4, w4);
  }

  // Sum up and scale the result to cover the range [-1,1]
  return 72.37857097679466f * (n0 + n1 + n2 + n3 + n4);
}

float FastSimplexNoise::scaled(const Float32Array& coords) const
{
  switch (coords.size()) {
    case 2:
      return scaled2D(coords[0], coords[1]);
    case 3:
      return scaled3D(coords[0], coords[1], coords[2]);
    case 4:
      return scaled4D(coords[0], coords[1], coords[2], coords[3]);
    default:
      return 0.f;
  }
}

float FastSimplexNoise::scaled2D(float x, float y) const
{
  float amplitude    = _amplitude;
  float frequency    = _frequency;
  float maxAmplitude = 0.f;
  float noise        = 0.f;

  for (unsigned int i = 0; i < _octaves; ++i) {
    noise += raw2D(x * frequency, y * frequency) * amplitude;
    maxAmplitude += amplitude;
    amplitude *= _persistence;
    frequency *= 2.f;
  }

  return _scale(noise / maxAmplitude);
}

float FastSimplexNoise::scaled3D(float x, float y, float z) const
{
  float amplitude    = _amplitude;
  float frequency    = _frequency;
  float maxAmplitude = 0.f;
  float noise        = 0.f;

  for (unsigned int i = 0; i < _octaves; ++i) {
    noise += raw3D(x * frequency, y * frequency, z * frequency) * amplitude;
    maxAmplitude += amplitude;
    amplitude *= _persistence;
    frequency *= 2.f;
  }

  return _scale(noise / maxAmplitude);
}

float FastSimplexNoise::scaled4D(float x, float y, float z, float w) const
{
  float amplitude    = _amplitude;
  float frequency    = _frequency;
  float maxAmplitude = 0.f;
  float noise        = 0.f;

  for (unsigned int i = 0; i < _octaves; ++i) {
    noise += raw4D(x * frequency, y * frequency, z * frequency, w * frequency)
             * amplitude;
    maxAmplitude += amplitude;
    amplitude *= _persistence;
    frequency *= 2.f;
  }

  return _scale(noise / maxAmplitude);
}

float FastSimplexNoise::spherical(float c, const Float32Array& coords) const
{
  switch (coords.size()) {
    case 3:
      return spherical3D(c, coords[0], coords[1], coords[2]);
    case 2:
      return spherical2D(c, coords[0], coords[1]);
    default:
      return 0.f;
  }
}

float FastSimplexNoise::spherical2D(float c, float x, float y) const
{
  const float nx     = x / c;
  const float ny     = y / c;
  const float rdx    = nx * 2.f * Math::PI;
  const float rdy    = ny * Math::PI;
  const float sinY   = std::sin(rdy + Math::PI);
  const float sinRds = 2.f * Math::PI;
  const float a      = sinRds * std::sin(rdx) * sinY;
  const float b      = sinRds * std::cos(rdx) * sinY;
  const float d      = sinRds * std::cos(rdy);

  return scaled3D(a, b, d);
}

float FastSimplexNoise::spherical3D(float c, float x, float y, float z) const
{
  const float nx     = x / c;
  const float ny     = y / c;
  const float rdx    = nx * 2.f * Math::PI;
  const float rdy    = ny * Math::PI;
  const float sinY   = std::sin(rdy + Math::PI);
  const float sinRds = 2.f * Math::PI;
  const float a      = sinRds * std::sin(rdx) * sinY;
  const float b      = sinRds * std::cos(rdx) * sinY;
  const float d      = sinRds * std::cos(rdy);

  return scaled4D(a, b, d, z);
}

} // end of namespace Extensions
} // end of namespace BABYLON
