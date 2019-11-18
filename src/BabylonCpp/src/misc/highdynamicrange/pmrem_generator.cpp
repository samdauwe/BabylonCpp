#include <babylon/misc/highdynamicrange/pmrem_generator.h>

#include <cmath>

namespace BABYLON {

template <typename ArrayBufferView>
const std::vector<std::vector<Float32Array>>
  PMREMGenerator<ArrayBufferView>::_sgFace2DMapping = {
    // XPOS face
    {{0, 0, -1}, // u towards negative Z
     {0, -1, 0}, // v towards negative Y
     {1, 0, 0}}, // pos X axis
    // XNEG face
    {{0, 0, 1},   // u towards positive Z
     {0, -1, 0},  // v towards negative Y
     {-1, 0, 0}}, // neg X axis
    // YPOS face
    {{1, 0, 0},  // u towards positive X
     {0, 0, 1},  // v towards positive Z
     {0, 1, 0}}, // pos Y axis
    // YNEG face
    {{1, 0, 0},   // u towards positive X
     {0, 0, -1},  // v towards negative Z
     {0, -1, 0}}, // neg Y axis
    // ZPOS face
    {{1, 0, 0},  // u towards positive X
     {0, -1, 0}, // v towards negative Y
     {0, 0, 1}}, // pos Z axis
    // ZNEG face
    {{-1, 0, 0},  // u towards negative X
     {0, -1, 0},  // v towards negative Y
     {0, 0, -1}}, // neg Z axis
};

template <typename ArrayBufferView>
const std::vector<std::vector<Uint32Array>>
  PMREMGenerator<ArrayBufferView>::_sgCubeNgh = {
    // XPOS face
    {{PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_Y_POS, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_Y_NEG, PMREMGenerator::CP_EDGE_RIGHT}},
    // XNEG face
    {{PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_Y_POS, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_Y_NEG, PMREMGenerator::CP_EDGE_LEFT}},
    // YPOS face
    {{PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_TOP}},
    // YNEG face
    {{PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_BOTTOM},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_BOTTOM},
     {PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_BOTTOM},
     {PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_BOTTOM}},
    // ZPOS face
    {{PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_Y_POS, PMREMGenerator::CP_EDGE_BOTTOM},
     {PMREMGenerator::CP_FACE_Y_NEG, PMREMGenerator::CP_EDGE_TOP}},
    // ZNEG face
    {{PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_Y_POS, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_Y_NEG, PMREMGenerator::CP_EDGE_BOTTOM}}};

template <typename ArrayBufferView>
const std::vector<Uint32Array> PMREMGenerator<ArrayBufferView>::_sgCubeEdgeList
  = {{PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_X_POS, PMREMGenerator::CP_EDGE_BOTTOM},

     {PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_LEFT},
     {PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_RIGHT},
     {PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_X_NEG, PMREMGenerator::CP_EDGE_BOTTOM},

     {PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_Z_POS, PMREMGenerator::CP_EDGE_BOTTOM},
     {PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_TOP},
     {PMREMGenerator::CP_FACE_Z_NEG, PMREMGenerator::CP_EDGE_BOTTOM}};

template <typename ArrayBufferView>
const std::vector<Uint32Array>
  PMREMGenerator<ArrayBufferView>::_sgCubeCornerList = {
    // XPOS face
    {PMREMGenerator::CP_CORNER_PPP, PMREMGenerator::CP_CORNER_PPN,
     PMREMGenerator::CP_CORNER_PNP, PMREMGenerator::CP_CORNER_PNN},
    // XNEG face
    {PMREMGenerator::CP_CORNER_NPN, PMREMGenerator::CP_CORNER_NPP,
     PMREMGenerator::CP_CORNER_NNN, PMREMGenerator::CP_CORNER_NNP},
    // YPOS face
    {PMREMGenerator::CP_CORNER_NPN, PMREMGenerator::CP_CORNER_PPN,
     PMREMGenerator::CP_CORNER_NPP, PMREMGenerator::CP_CORNER_PPP},
    // YNEG face
    {PMREMGenerator::CP_CORNER_NNP, PMREMGenerator::CP_CORNER_PNP,
     PMREMGenerator::CP_CORNER_NNN, PMREMGenerator::CP_CORNER_PNN},
    // ZPOS face
    {PMREMGenerator::CP_CORNER_NPP, PMREMGenerator::CP_CORNER_PPP,
     PMREMGenerator::CP_CORNER_NNP, PMREMGenerator::CP_CORNER_PNP},
    // ZNEG face
    {PMREMGenerator::CP_CORNER_PPN, PMREMGenerator::CP_CORNER_NPN,
     PMREMGenerator::CP_CORNER_PNN, PMREMGenerator::CP_CORNER_NNN}};

template <typename ArrayBufferView>
PMREMGenerator<ArrayBufferView>::PMREMGenerator(
  const std::vector<ArrayBufferView>& _input, int _inputSize, int _outputSize,
  size_t _maxNumMipLevels, size_t _numChannels, bool _isFloat,
  float _specularPower, float _cosinePowerDropPerMip, bool _excludeBase,
  bool _fixup)
    : input{_input}
    , inputSize{_inputSize}
    , outputSize{_outputSize}
    , maxNumMipLevels{_maxNumMipLevels}
    , numChannels{_numChannels}
    , isFloat{_isFloat}
    , specularPower{_specularPower}
    , cosinePowerDropPerMip{_cosinePowerDropPerMip}
    , excludeBase{_excludeBase}
    , fixup{_fixup}
{
}

template <typename ArrayBufferView>
PMREMGenerator<ArrayBufferView>::~PMREMGenerator() = default;

template <typename ArrayBufferView>
std::vector<std::vector<ArrayBufferView>>&
PMREMGenerator<ArrayBufferView>::filterCubeMap()
{
  // Init cubemap processor
  init();

  // Filters the cubemap
  filterCubeMapMipChain();

  // Returns the filtered mips.
  return _outputSurface;
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::init()
{
  unsigned int mipLevelSize;

  // if nax num mip levels is set to 0, set it to generate the entire mip chain
  if (maxNumMipLevels == 0) {
    maxNumMipLevels = PMREMGenerator::CP_MAX_MIPLEVELS;
  }

  // first miplevel size
  mipLevelSize = outputSize;

  // Iterate over mip chain, and init ArrayBufferView for mip-chain
  _outputSurface.resize(maxNumMipLevels);
  for (unsigned int j = 0; j < maxNumMipLevels; ++j) {
    _outputSurface[j].resize(6);
    // Iterate over faces for output images
    for (unsigned i = 0; i < 6; i++) {
      // Initializes a new array for the output.
      _outputSurface[j][i].resize(mipLevelSize * mipLevelSize * numChannels);
    }

    // next mip level is half size
    mipLevelSize >>= 1;

    ++_numMipLevels;

    // terminate if mip chain becomes too small
    if (mipLevelSize == 0) {
      maxNumMipLevels = j;
      return;
    }
  }
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::filterCubeMapMipChain()
{
  // First, take count of the lighting model to modify SpecularPower
  // var refSpecularPower = (a_MCO.LightingModel == CP_LIGHTINGMODEL_BLINN ||
  // a_MCO.LightingModel == CP_LIGHTINGMODEL_BLINN_BRDF) ? a_MCO.SpecularPower /
  // GetSpecularPowerFactorToMatchPhong(a_MCO.SpecularPower) :
  // a_MCO.SpecularPower;
  // var refSpecularPower = specularPower; // Only Phong BRDF yet. This explains
  // the line below using specularpower.

  // Cone angle start (for generating subsequent mip levels)
  float currentSpecularPower = specularPower;

  // Build filter lookup tables based on the source miplevel size
  precomputeFilterLookupTables(inputSize);

  // Note that we need to filter the first level before generating mipmap
  // So LevelIndex == 0 is base filtering hen LevelIndex > 0 is mipmap
  // generation
  for (size_t levelIndex = 0; levelIndex < _numMipLevels; ++levelIndex) {
    // TODO : Write a function to copy and scale the base mipmap in output
    // I am just lazy here and just put a high specular power value, and do some
    // if.
    if (excludeBase && (levelIndex == 0)) {
      // If we don't want to process the base mipmap, just put a very high
      // specular power (this allow to handle scale of the texture).
      currentSpecularPower = 100000.f;
    }

    // Special case for cosine power mipmap chain. For quality requirement, we
    // always process the current mipmap from the top mipmap
    std::vector<ArrayBufferView>& srcCubeImage = input;
    std::vector<std::vector<ArrayBufferView>>& dstCubeImage
      = _outputSurface[levelIndex];
    size_t dstSize = outputSize >> levelIndex;

    // Compute required angle.
    float angle = getBaseFilterAngle(currentSpecularPower);

    // filter cube surfaces
    filterCubeSurfaces(srcCubeImage, inputSize, dstCubeImage, dstSize, angle,
                       currentSpecularPower);

    // fix seams
    if (fixup) {
      fixupCubeEdges(dstCubeImage, dstSize);
    }

    // Decrease the specular power to generate the mipmap chain
    // TODO : Use another method for Exclude (see first comment at start of the
    // function
    if (excludeBase && (levelIndex == 0)) {
      currentSpecularPower = specularPower;
    }

    currentSpecularPower *= cosinePowerDropPerMip;
  }
}

template <typename ArrayBufferView>
float PMREMGenerator<ArrayBufferView>::getBaseFilterAngle(
  float cosinePower) const
{
  // We want to find the alpha such that:
  // cos(alpha)^cosinePower = epsilon
  // That's: acos(epsilon^(1/cosinePower))
  const float threshold = 0.000001f; // Empirical threshold (Work perfectly,
                                     // didn't check for a more big number, may
                                     // get some performance and still god
                                     // approximation)
  float angle = 180.f;
  angle       = std::acos(std::pow(threshold, 1.f / cosinePower));
  angle *= 180.f / Math::PI; // Convert to degree
  angle *= 2.f;              // * 2.0f because PMREMGenerator divide by 2 later

  return angle;
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::precomputeFilterLookupTables(
  size_t srcCubeMapWidth)
{
  // clear pre-existing normalizer cube map
  _normCubeMap.clear();

  // Normalized vectors per cubeface and per-texel solid angle
  buildNormalizerSolidAngleCubemap(srcCubeMapWidth);
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::buildNormalizerSolidAngleCubemap(
  size_t size)
{
  // iterate over cube faces
  for (unsigned int iCubeFace = 0; iCubeFace < 6; ++iCubeFace) {
    // First three channels for norm cube, and last channel for solid angle
    _normCubeMap.emplace_back(Float32Array(size * size * 4));

    // fast texture walk, build normalizer cube map
    // ArrayBufferView& facesData = input[iCubeFace];

    for (size_t v = 0; v < size; v++) {
      for (size_t u = 0; u < size; u++) {
        Vector3 vect = texelCoordToVect(iCubeFace, u, v, size, fixup);
        _normCubeMap[iCubeFace][(v * size + u) * 4 + 0] = vect.x;
        _normCubeMap[iCubeFace][(v * size + u) * 4 + 1] = vect.y;
        _normCubeMap[iCubeFace][(v * size + u) * 4 + 2] = vect.z;

        float solidAngle = texelCoordSolidAngle(iCubeFace, u, v, size);
        _normCubeMap[iCubeFace][(v * size + u) * 4 + 4] = solidAngle;
      }
    }
  }
}

template <typename ArrayBufferView>
Vector4 PMREMGenerator<ArrayBufferView>::texelCoordToVect(unsigned int faceIdx,
                                                          float u, float v,
                                                          size_t size,
                                                          bool _fixup) const
{
  Vector4 _vectorTemp{0.f, 0.f, 0.f, 0.f};
  float nvcU;
  float nvcV;

  // Change from original AMD code
  // transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
  // + 0.5f is for texel center addressing
  nvcU = (2.f * (u + 0.5f) / size) - 1.f;
  nvcV = (2.f * (v + 0.5f) / size) - 1.f;

  // warp fixup
  if (_fixup && size > 1) {
    // Code from Nvtt :
    // http://code.google.com/p/nvidia-texture-tools/source/browse/trunk/src/nvtt/CubeSurface.cpp
    auto sizef  = static_cast<float>(size);
    float a     = std::pow(sizef, 2.f) / std::pow(sizef - 1.f, 3.f);
    nvcU        = a * std::pow(nvcU, 3.f) + nvcU;
    nvcV        = a * std::pow(nvcV, 3.f) + nvcV;
  }

  // Get current vector
  // generate x,y,z vector (xform 2d NVC coord to 3D vector)
  // U contribution
  const Float32Array& UVec
    = PMREMGenerator::_sgFace2DMapping[faceIdx][PMREMGenerator::CP_UDIR];
  _vectorTemp.x = UVec[0] * nvcU;
  _vectorTemp.y = UVec[1] * nvcU;
  _vectorTemp.z = UVec[2] * nvcU;

  // V contribution and Sum
  const Float32Array& VVec
    = PMREMGenerator::_sgFace2DMapping[faceIdx][PMREMGenerator::CP_VDIR];
  _vectorTemp.x += VVec[0] * nvcV;
  _vectorTemp.y += VVec[1] * nvcV;
  _vectorTemp.z += VVec[2] * nvcV;

  // add face axis
  const Float32Array& faceAxis
    = PMREMGenerator::_sgFace2DMapping[faceIdx][PMREMGenerator::CP_FACEAXIS];
  _vectorTemp.x += faceAxis[0];
  _vectorTemp.y += faceAxis[1];
  _vectorTemp.z += faceAxis[2];

  // normalize vector
  _vectorTemp.normalize();

  return _vectorTemp;
}

template <typename ArrayBufferView>
Vector4 PMREMGenerator<ArrayBufferView>::vectToTexelCoord(float x, float y,
                                                          float z,
                                                          size_t size) const
{
  Vector4 _vectorTemp{0.f, 0.f, 0.f, 0.f};
  float maxCoord;
  unsigned int faceIdx;

  // absolute value 3
  float absX = std::abs(x);
  float absY = std::abs(y);
  float absZ = std::abs(z);

  if (absX >= absY && absX >= absZ) {
    maxCoord = absX;

    if (x >= 0.f) // face = XPOS
    {
      faceIdx = PMREMGenerator::CP_FACE_X_POS;
    }
    else {
      faceIdx = PMREMGenerator::CP_FACE_X_NEG;
    }
  }
  else if (absY >= absX && absY >= absZ) {
    maxCoord = absY;

    if (y >= 0.f) // face = XPOS
    {
      faceIdx = PMREMGenerator::CP_FACE_Y_POS;
    }
    else {
      faceIdx = PMREMGenerator::CP_FACE_Y_NEG;
    }
  }
  else {
    maxCoord = absZ;

    if (z >= 0.f) // face = XPOS
    {
      faceIdx = PMREMGenerator::CP_FACE_Z_POS;
    }
    else {
      faceIdx = PMREMGenerator::CP_FACE_Z_NEG;
    }
  }

  // divide through by max coord so face vector lies on cube face
  float scale = 1.f / maxCoord;
  x *= scale;
  y *= scale;
  z *= scale;

  const Float32Array& temp1
    = PMREMGenerator::_sgFace2DMapping[faceIdx][PMREMGenerator::CP_UDIR];
  float nvcU = temp1[0] * x + temp1[1] * y + temp1[2] * z;

  const Float32Array& temp2
    = PMREMGenerator::_sgFace2DMapping[faceIdx][PMREMGenerator::CP_VDIR];
  float nvcV = temp2[0] * x + temp2[1] * y + temp2[2] * z;

  // Modify original AMD code to return value from 0 to Size - 1
  float u = std::floor(static_cast<float>(size - 1) * 0.5f * (nvcU + 1.f));
  float v = std::floor(static_cast<float>(size - 1) * 0.5f * (nvcV + 1.f));

  _vectorTemp.x = static_cast<float>(faceIdx);
  _vectorTemp.y = u;
  _vectorTemp.z = v;

  return _vectorTemp;
}

template <typename ArrayBufferView>
float PMREMGenerator<ArrayBufferView>::areaElement(float x, float y) const
{
  return std::atan2(x * y, std::sqrt(x * x + y * y + 1.f));
}

template <typename ArrayBufferView>
float PMREMGenerator<ArrayBufferView>::texelCoordSolidAngle(
  unsigned int /*faceIdx*/, float u, float v, size_t size) const
{
  // transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
  // (+ 0.5f is for texel center addressing)
  float _u = (2.f * (u + 0.5f) / size) - 1.f;
  float _v = (2.f * (v + 0.5f) / size) - 1.f;

  // Shift from a demi texel, mean 1.0f / a_Size with U and V in [-1..1]
  float invResolution = 1.f / size;

  // U and V are the -1..1 texture coordinate on the current face.
  // Get projected area for this texel
  float x0 = _u - invResolution;
  float y0 = _v - invResolution;
  float x1 = _u + invResolution;
  float y1 = _v + invResolution;

  // return solid angle
  return areaElement(x0, y0) - areaElement(x0, y1) - areaElement(x1, y0)
         + areaElement(x1, y1);
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::filterCubeSurfaces(
  const std::vector<ArrayBufferView>& srcCubeMap, float srcSize,
  std::vector<ArrayBufferView>& dstCubeMap, size_t dstSize,
  float filterConeAngle, float _specularPower)
{
  // bounding box per face to specify region to process
  std::array<CMGBoundinBox, 6> filterExtents;

  // min angle a src texel can cover (in degrees)
  float srcTexelAngle = (180.f / (Math::PI)*std::atan2(1.f, srcSize));

  // angle about center tap to define filter cone
  // filter angle is 1/2 the cone angle
  float filterAngle = filterConeAngle / 2.f;

  // ensure filter angle is larger than a texel
  if (filterAngle < srcTexelAngle) {
    filterAngle = srcTexelAngle;
  }

  // ensure filter cone is always smaller than the hemisphere
  if (filterAngle > 90.f) {
    filterAngle = 90.f;
  }

  // the maximum number of texels in 1D the filter cone angle will cover
  //  used to determine bounding box size for filter extents
  float filterSize = std::ceil(filterAngle / srcTexelAngle);

  // ensure conservative region always covers at least one texel
  if (filterSize < 1) {
    filterSize = 1;
  }

  // dotProdThresh threshold based on cone angle to determine whether or not
  // taps
  //  reside within the cone angle
  float dotProdThresh = std::cos((Math::PI / 180.f) * filterAngle);

  // process required faces
  for (unsigned int iCubeFace = 0; iCubeFace < 6; ++iCubeFace) {
    // iterate over dst cube map face texel
    for (unsigned int v = 0; v < dstSize; ++v) {
      for (unsigned int u = 0; u < dstSize; ++u) {
        // get center tap direction
        Vector4 centerTapDir
          = texelCoordToVect(iCubeFace, u, v, dstSize, fixup).clone();

        // clear old per-face filter extents
        clearFilterExtents(filterExtents);

        // define per-face filter extents
        determineFilterExtents(centerTapDir, srcSize, filterSize,
                               filterExtents);

        // perform filtering of src faces using filter extents
        Vector3 vect
          = processFilterExtents(centerTapDir, dotProdThresh, filterExtents,
                                 srcCubeMap, srcSize, _specularPower);

        dstCubeMap[iCubeFace][(v * dstSize + u) * numChannels + 0] = vect.x;
        dstCubeMap[iCubeFace][(v * dstSize + u) * numChannels + 1] = vect.y;
        dstCubeMap[iCubeFace][(v * dstSize + u) * numChannels + 2] = vect.z;
      }
    }
  }
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::clearFilterExtents(
  std::array<CMGBoundinBox, 6>& filterExtents)
{
  for (auto& filterExtent : filterExtents) {
    filterExtent.clear();
  }
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::determineFilterExtents(
  const Vector4& centerTapDir, size_t srcSize, size_t bboxSize,
  std::array<CMGBoundinBox, 6>& filterExtents) const
{
  // neighboring face and bleed over amount, and width of BBOX for
  // left, right, top, and bottom edges of this face
  std::array<float, 4> bleedOverAmount{{0, 0, 0, 0}};
  std::array<float, 4> bleedOverBBoxMin{{0, 0, 0, 0}};
  std::array<float, 4> bleedOverBBoxMax{{0, 0, 0, 0}};

  unsigned int neighborFace    = 0;
  unsigned int neighborEdge    = 0;
  unsigned int oppositeFaceIdx = 0;

  // get face idx, and u, v info from center tap dir
  Vector3 result
    = vectToTexelCoord(centerTapDir.x, centerTapDir.y, centerTapDir.z, srcSize);
  auto faceIdx         = static_cast<unsigned>(result.x);
  float u              = result.y;
  float v              = result.z;

  // define bbox size within face
  filterExtents[faceIdx].augment(u - bboxSize, v - bboxSize, 0);
  filterExtents[faceIdx].augment(u + bboxSize, v + bboxSize, 0);

  filterExtents[faceIdx].clampMin(0, 0, 0);
  filterExtents[faceIdx].clampMax(srcSize - 1, srcSize - 1, 0);

  // u and v extent in face corresponding to center tap
  float minU = filterExtents[faceIdx].min.x;
  float minV = filterExtents[faceIdx].min.y;
  float maxU = filterExtents[faceIdx].max.x;
  float maxV = filterExtents[faceIdx].max.y;

  // bleed over amounts for face across u=0 edge (left)
  bleedOverAmount[0]  = (bboxSize - u);
  bleedOverBBoxMin[0] = minV;
  bleedOverBBoxMax[0] = maxV;

  // bleed over amounts for face across u=1 edge (right)
  bleedOverAmount[1]  = (u + bboxSize) - (srcSize - 1);
  bleedOverBBoxMin[1] = minV;
  bleedOverBBoxMax[1] = maxV;

  // bleed over to face across v=0 edge (up)
  bleedOverAmount[2]  = (bboxSize - v);
  bleedOverBBoxMin[2] = minU;
  bleedOverBBoxMax[2] = maxU;

  // bleed over to face across v=1 edge (down)
  bleedOverAmount[3]  = (v + bboxSize) - (srcSize - 1);
  bleedOverBBoxMin[3] = minU;
  bleedOverBBoxMax[3] = maxU;

  // compute bleed over regions in neighboring faces
  for (unsigned int i = 0; i < 4; i++) {
    if (bleedOverAmount[i] > 0) {
      neighborFace = PMREMGenerator::_sgCubeNgh[faceIdx][i][0];
      neighborEdge = PMREMGenerator::_sgCubeNgh[faceIdx][i][1];

      // For certain types of edge abutments, the bleedOverBBoxMin, and
      // bleedOverBBoxMax need to
      //  be flipped: the cases are
      // if a left   edge mates with a left or bottom  edge on the neighbor
      // if a top    edge mates with a top or right edge on the neighbor
      // if a right  edge mates with a right or top edge on the neighbor
      // if a bottom edge mates with a bottom or left  edge on the neighbor
      // Seeing as the edges are enumerated as follows
      // left   =0
      // right  =1
      // top    =2
      // bottom =3
      //
      // so if the edge enums are the same, or the sum of the enums == 3,
      //  the bbox needs to be flipped
      if ((i == neighborEdge) || ((i + neighborEdge) == 3)) {
        bleedOverBBoxMin[i] = (srcSize - 1) - bleedOverBBoxMin[i];
        bleedOverBBoxMax[i] = (srcSize - 1) - bleedOverBBoxMax[i];
      }

      // The way the bounding box is extended onto the neighboring face
      // depends on which edge of neighboring face abuts with this one
      switch (PMREMGenerator::_sgCubeNgh[faceIdx][i][1]) {
        case PMREMGenerator::CP_EDGE_LEFT:
          filterExtents[neighborFace].augment(0, bleedOverBBoxMin[i], 0);
          filterExtents[neighborFace].augment(bleedOverAmount[i],
                                              bleedOverBBoxMax[i], 0);
          break;
        case PMREMGenerator::CP_EDGE_RIGHT:
          filterExtents[neighborFace].augment((srcSize - 1),
                                              bleedOverBBoxMin[i], 0);
          filterExtents[neighborFace].augment(
            (srcSize - 1) - bleedOverAmount[i], bleedOverBBoxMax[i], 0);
          break;
        case PMREMGenerator::CP_EDGE_TOP:
          filterExtents[neighborFace].augment(bleedOverBBoxMin[i], 0, 0);
          filterExtents[neighborFace].augment(bleedOverBBoxMax[i],
                                              bleedOverAmount[i], 0);
          break;
        case PMREMGenerator::CP_EDGE_BOTTOM:
          filterExtents[neighborFace].augment(bleedOverBBoxMin[i],
                                              (srcSize - 1), 0);
          filterExtents[neighborFace].augment(
            bleedOverBBoxMax[i], (srcSize - 1) - bleedOverAmount[i], 0);
          break;
      }

      // clamp filter extents in non-center tap faces to remain within surface
      filterExtents[neighborFace].clampMin(0, 0, 0);
      filterExtents[neighborFace].clampMax(srcSize - 1, srcSize - 1, 0);
    }

    // If the bleed over amount bleeds past the adjacent face onto the opposite
    // face
    // from the center tap face, then process the opposite face entirely for
    // now.
    // Note that the cases in which this happens, what usually happens is that
    // more than one edge bleeds onto the opposite face, and the bounding box
    // encompasses the entire cube map face.
    if (bleedOverAmount[i] > srcSize) {
      // determine opposite face
      switch (faceIdx) {
        case PMREMGenerator::CP_FACE_X_POS:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_X_NEG;
          break;
        case PMREMGenerator::CP_FACE_X_NEG:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_X_POS;
          break;
        case PMREMGenerator::CP_FACE_Y_POS:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_Y_NEG;
          break;
        case PMREMGenerator::CP_FACE_Y_NEG:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_Y_POS;
          break;
        case PMREMGenerator::CP_FACE_Z_POS:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_Z_NEG;
          break;
        case PMREMGenerator::CP_FACE_Z_NEG:
          oppositeFaceIdx = PMREMGenerator::CP_FACE_Z_POS;
          break;
        default:
          break;
      }

      // just encompass entire face for now
      filterExtents[oppositeFaceIdx].augment(0, 0, 0);
      filterExtents[oppositeFaceIdx].augment((srcSize - 1), (srcSize - 1), 0);
    }
  }
}

template <typename ArrayBufferView>
Vector4 PMREMGenerator<ArrayBufferView>::processFilterExtents(
  const Vector4& centerTapDir, float dotProdThresh,
  const std::array<CMGBoundinBox, 6>& filterExtents,
  const std::vector<ArrayBufferView>& srcCubeMap, size_t srcSize,
  size_t _specularPower) const
{
  Vector4 _vectorTemp{0.f, 0.f, 0.f, 0.f};

  // accumulators are 64-bit floats in order to have the precision needed
  // over a summation of a large number of pixels
  std::array<float, 4> dstAccum{{0, 0, 0, 0}};
  float weightAccum   = 0.f;
  size_t nSrcChannels = numChannels;

  // norm cube map and srcCubeMap have same face width
  size_t faceWidth = srcSize;

  // amount to add to pointer to move to next scanline in images
  size_t normCubePitch = faceWidth * 4; // 4 channels in normCubeMap.
  size_t srcCubePitch
    = faceWidth
      * numChannels; // numChannels correponds to the cubemap number of channel

  unsigned int IsPhongBRDF = 1; // Only works in Phong BRDF yet.
  //(a_LightingModel == CP_LIGHTINGMODEL_PHONG_BRDF || a_LightingModel ==
  // CP_LIGHTINGMODEL_BLINN_BRDF) ? 1 : 0; // This value will be added to the
  // specular power

  // iterate over cubefaces
  for (unsigned int iFaceIdx = 0; iFaceIdx < 6; iFaceIdx++) {

    // if bbox is non empty
    if (!filterExtents[iFaceIdx].empty()) {
      float uStart = filterExtents[iFaceIdx].min.x;
      float vStart = filterExtents[iFaceIdx].min.y;
      float uEnd   = filterExtents[iFaceIdx].max.x;
      float vEnd   = filterExtents[iFaceIdx].max.y;

      auto startIndexNormCubeMap = static_cast<size_t>(4.f * ((vStart * faceWidth) + uStart));
      auto startIndexSrcCubeMap
        = static_cast<size_t>(numChannels * ((vStart * faceWidth) + uStart));

      // note that <= is used to ensure filter extents always encompass at least
      // one pixel if bbox is non empty
      for (float v = vStart; v <= vEnd; v++) {
        float normCubeRowWalk = 0;
        float srcCubeRowWalk  = 0;

        for (float u = uStart; u <= uEnd; u++) {
          // pointer to direction in cube map associated with texel
          float texelVectX = _normCubeMap[iFaceIdx][startIndexNormCubeMap
                                                    + normCubeRowWalk + 0];
          float texelVectY = _normCubeMap[iFaceIdx][startIndexNormCubeMap
                                                    + normCubeRowWalk + 1];
          float texelVectZ = _normCubeMap[iFaceIdx][startIndexNormCubeMap
                                                    + normCubeRowWalk + 2];

          // check dot product to see if texel is within cone
          float tapDotProd = texelVectX * centerTapDir.x
                             + texelVectY * centerTapDir.y
                             + texelVectZ * centerTapDir.z;

          if (tapDotProd >= dotProdThresh && tapDotProd > 0.f) {
            // solid angle stored in 4th channel of normalizer/solid angle cube
            // map
            float weight = _normCubeMap[iFaceIdx][startIndexNormCubeMap
                                                  + normCubeRowWalk + 3];

            // Here we decide if we use a Phong/Blinn or a Phong/Blinn BRDF.
            // Phong/Blinn BRDF is just the Phong/Blinn model multiply by the
            // cosine of the lambert law
            // so just adding one to specularpower do the trick.
            weight *= std::pow(tapDotProd, (_specularPower + IsPhongBRDF));

            // iterate over channels
            for (size_t k = 0; k < nSrcChannels;
                 k++) //(aSrcCubeMap[iFaceIdx].m_NumChannels) //up to 4 channels
            {
              dstAccum[k] += weight
                             * srcCubeMap[iFaceIdx][startIndexSrcCubeMap
                                                    + srcCubeRowWalk];
              srcCubeRowWalk++;
            }

            weightAccum += weight; // accumulate weight
          }
          else {
            // step across source pixel
            srcCubeRowWalk += nSrcChannels;
          }

          normCubeRowWalk += 4; // 4 channels per norm cube map.
        }

        startIndexNormCubeMap += normCubePitch;
        startIndexSrcCubeMap += srcCubePitch;
      }
    }
  }

  // divide through by weights if weight is non zero
  if (weightAccum != 0.f) {
    _vectorTemp.x = (dstAccum[0] / weightAccum);
    _vectorTemp.y = (dstAccum[1] / weightAccum);
    _vectorTemp.z = (dstAccum[2] / weightAccum);
    if (numChannels > 3) {
      _vectorTemp.w = (dstAccum[3] / weightAccum);
    }
  }
  else {
    // otherwise sample nearest
    // get face idx and u, v texel coordinate in face
    Vector3 coord = vectToTexelCoord(centerTapDir.x, centerTapDir.y,
                                     centerTapDir.z, srcSize)
                      .clone();

    _vectorTemp.x
      = srcCubeMap[coord.x][numChannels * (coord.z * srcSize + coord.y) + 0];
    _vectorTemp.y
      = srcCubeMap[coord.x][numChannels * (coord.z * srcSize + coord.y) + 1];
    _vectorTemp.z
      = srcCubeMap[coord.x][numChannels * (coord.z * srcSize + coord.y) + 2];
    if (numChannels > 3) {
      _vectorTemp.z
        = srcCubeMap[coord.x][numChannels * (coord.z * srcSize + coord.y) + 3];
    }
  }

  return _vectorTemp;
}

template <typename ArrayBufferView>
void PMREMGenerator<ArrayBufferView>::fixupCubeEdges(
  std::vector<ArrayBufferView>& cubeMap, size_t cubeMapSize)
{
  std::array<unsigned int, 8> cornerNumPtrs{
    {0, 0, 0, 0, 0, 0, 0, 0}}; // indexed by corner and face idx
  std::array<Uint32Array, 4> faceCornerStartIndicies;

  // note that if functionality to filter across the three texels for each
  // corner, then
  // indexed by corner and face idx. the array contains the face the start
  // points belongs to.
  std::array<std::array<Uint32Array, 3>, 8> cornerPtr;

  // if there is no fixup, or fixup width = 0, do nothing
  if (cubeMapSize < 1) {
    return;
  }

  // special case 1x1 cubemap, average face colors
  if (cubeMapSize == 1) {
    // iterate over channels
    for (unsigned int k = 0; k < numChannels; ++k) {
      size_t accum = 0.f;

      // iterate over faces to accumulate face colors
      for (unsigned int iFace = 0; iFace < 6; ++iFace) {
        accum += cubeMap[iFace][k];
      }

      // compute average over 6 face colors
      accum /= 6.f;

      // iterate over faces to distribute face colors
      for (unsigned int iFace = 0; iFace < 6; iFace++) {
        cubeMap[iFace][k] = accum;
      }
    }

    return;
  }

  // iterate over faces to collect list of corner texel pointers
  for (unsigned int iFace = 0; iFace < 6; ++iFace) {
    // the 4 corner pointers for this face
    faceCornerStartIndicies[0] = {iFace, 0};
    faceCornerStartIndicies[1] = {iFace, ((cubeMapSize - 1) * numChannels)};
    faceCornerStartIndicies[2]
      = {iFace, ((cubeMapSize) * (cubeMapSize - 1) * numChannels)};
    faceCornerStartIndicies[3]
      = {iFace, ((((cubeMapSize) * (cubeMapSize - 1)) + (cubeMapSize - 1))
                 * numChannels)};

    // iterate over face corners to collect cube corner pointers
    for (unsigned int iCorner = 0; iCorner < 4; ++iCorner) {
      unsigned int corner = _sgCubeCornerList[iFace][iCorner];
      cornerPtr[corner][cornerNumPtrs[corner]]
        = faceCornerStartIndicies[iCorner];
      cornerNumPtrs[corner]++;
    }
  }

  // iterate over corners to average across corner tap values
  for (unsigned int iCorner = 0; iCorner < 8; iCorner++) {
    for (unsigned int k = 0; k < numChannels; ++k) {
      float cornerTapAccum = 0.f;

      // iterate over corner texels and average results
      for (unsigned int i = 0; i < 3; i++) {
        cornerTapAccum
          += cubeMap[cornerPtr[iCorner][i][0]]
                    [cornerPtr[iCorner][i][1] + k]; // Get in the cube map face
                                                    // the start point +
                                                    // channel.
      }

      // divide by 3 to compute average of corner tap values
      cornerTapAccum *= (1.f / 3.f);

      // iterate over corner texels and average results
      for (unsigned int i = 0; i < 3; i++) {
        cubeMap[cornerPtr[iCorner][i][0]][cornerPtr[iCorner][i][1] + k]
          = cornerTapAccum;
      }
    }
  }

  // iterate over the twelve edges of the cube to average across edges
  for (unsigned int i = 0; i < 12; i++) {
    unsigned int face = _sgCubeEdgeList[i][0];
    unsigned int edge = _sgCubeEdgeList[i][1];

    unsigned int neighborFace = PMREMGenerator::_sgCubeNgh[face][edge][0];
    unsigned int neighborEdge = PMREMGenerator::_sgCubeNgh[face][edge][1];

    unsigned int edgeStartIndex = 0; // a_CubeMap[face].m_ImgData;
    unsigned int neighborEdgeStartIndex
      = 0; // a_CubeMap[neighborFace].m_ImgData;
    unsigned int edgeWalk         = 0;
    unsigned int neighborEdgeWalk = 0;

    // Determine walking pointers based on edge type
    // e.g. CP_EDGE_LEFT, CP_EDGE_RIGHT, CP_EDGE_TOP, CP_EDGE_BOTTOM
    switch (edge) {
      case PMREMGenerator::CP_EDGE_LEFT:
        // no change to faceEdgeStartPtr
        edgeWalk = numChannels * cubeMapSize;
        break;
      case PMREMGenerator::CP_EDGE_RIGHT:
        edgeStartIndex += (cubeMapSize - 1) * numChannels;
        edgeWalk = numChannels * cubeMapSize;
        break;
      case PMREMGenerator::CP_EDGE_TOP:
        // no change to faceEdgeStartPtr
        edgeWalk = numChannels;
        break;
      case PMREMGenerator::CP_EDGE_BOTTOM:
        edgeStartIndex += (cubeMapSize) * (cubeMapSize - 1) * numChannels;
        edgeWalk = numChannels;
        break;
    }

    // For certain types of edge abutments, the neighbor edge walk needs to
    //  be flipped: the cases are
    // if a left   edge mates with a left or bottom  edge on the neighbor
    // if a top    edge mates with a top or right edge on the neighbor
    // if a right  edge mates with a right or top edge on the neighbor
    // if a bottom edge mates with a bottom or left  edge on the neighbor
    // Seeing as the edges are enumerated as follows
    // left   =0
    // right  =1
    // top    =2
    // bottom =3
    //
    // If the edge enums are the same, or the sum of the enums == 3,
    //  the neighbor edge walk needs to be flipped
    if ((edge == neighborEdge)
        || ((edge + neighborEdge)
            == 3)) { // swapped direction neighbor edge walk
      switch (neighborEdge) {
        case PMREMGenerator::CP_EDGE_LEFT: // start at lower left and walk up
          neighborEdgeStartIndex
            += (cubeMapSize - 1) * (cubeMapSize)*numChannels;
          neighborEdgeWalk = -(numChannels * cubeMapSize);
          break;
        case PMREMGenerator::CP_EDGE_RIGHT: // start at lower right and walk up
          neighborEdgeStartIndex
            += ((cubeMapSize - 1) * (cubeMapSize) + (cubeMapSize - 1))
               * numChannels;
          neighborEdgeWalk = -(numChannels * cubeMapSize);
          break;
        case PMREMGenerator::CP_EDGE_TOP: // start at upper right and walk left
          neighborEdgeStartIndex += (cubeMapSize - 1) * numChannels;
          neighborEdgeWalk = -numChannels;
          break;
        case PMREMGenerator::CP_EDGE_BOTTOM: // start at lower right and walk
                                             // left
          neighborEdgeStartIndex
            += ((cubeMapSize - 1) * (cubeMapSize) + (cubeMapSize - 1))
               * numChannels;
          neighborEdgeWalk = -numChannels;
          break;
      }
    }
    else {
      // swapped direction neighbor edge walk
      switch (neighborEdge) {
        case PMREMGenerator::CP_EDGE_LEFT: // start at upper left and walk down
          // no change to neighborEdgeStartPtr for this case since it points
          // to the upper left corner already
          neighborEdgeWalk = numChannels * cubeMapSize;
          break;
        case PMREMGenerator::CP_EDGE_RIGHT: // start at upper right and walk
                                            // down
          neighborEdgeStartIndex += (cubeMapSize - 1) * numChannels;
          neighborEdgeWalk = numChannels * cubeMapSize;
          break;
        case PMREMGenerator::CP_EDGE_TOP: // start at upper left and walk left
          // no change to neighborEdgeStartPtr for this case since it points
          // to the upper left corner already
          neighborEdgeWalk = numChannels;
          break;
        case PMREMGenerator::CP_EDGE_BOTTOM: // start at lower left and walk
                                             // left
          neighborEdgeStartIndex
            += (cubeMapSize) * (cubeMapSize - 1) * numChannels;
          neighborEdgeWalk = numChannels;
          break;
      }
    }

    // Perform edge walk, to average across the 12 edges and smoothly propagate
    // change to
    // nearby neighborhood

    // step ahead one texel on edge
    edgeStartIndex += edgeWalk;
    neighborEdgeStartIndex += neighborEdgeWalk;

    // note that this loop does not process the corner texels, since they have
    // already been
    //  averaged across faces across earlier
    for (unsigned int j = 1; j < (cubeMapSize - 1); j++) {
      // for each set of taps along edge, average them
      // and rewrite the results into the edges
      for (unsigned int k = 0; k < numChannels; k++) {
        ArrayBufferView& edgeTap = cubeMap[face][edgeStartIndex + k];
        ArrayBufferView& neighborEdgeTap
          = cubeMap[neighborFace][neighborEdgeStartIndex + k];

        // compute average of tap intensity values
        float avgTap = 0.5f * (edgeTap + neighborEdgeTap);

        // propagate average of taps to edge taps
        cubeMap[face][edgeStartIndex + k]                 = avgTap;
        cubeMap[neighborFace][neighborEdgeStartIndex + k] = avgTap;
      }

      edgeStartIndex += edgeWalk;
      neighborEdgeStartIndex += neighborEdgeWalk;
    }
  }
}

} // end of namespace BABYLON
