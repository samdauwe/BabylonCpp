//_______________________________________________________________
// Extracted from CubeMapGen:
// https://code.google.com/archive/p/cubemapgen/
//
// Following
// https://seblagarde.wordpress.com/2012/06/10/amd-cubemapgen-for-physically-based-rendering/
//_______________________________________________________________

#ifndef BABYLON_MISC_HIGH_DYNAMIC_RANGE_PMREM_GENERATOR_H
#define BABYLON_MISC_HIGH_DYNAMIC_RANGE_PMREM_GENERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector4.h>
#include <babylon/misc/highdynamicrange/cmg_bounding_box.h>

namespace BABYLON {

/**
 * Helper class to PreProcess a cubemap in order to generate mipmap according
 * to the level of blur required by the glossinees of a material.
 *
 * This only supports the cosine drop power as well as Warp fixup generation
 * method.
 *
 * This is using the process from CubeMapGen described here:
 * https://seblagarde.wordpress.com/2012/06/10/amd-cubemapgen-for-physically-based-rendering/
 */
template <typename ArrayBufferView>
class BABYLON_SHARED_EXPORT PMREMGenerator {

private:
  static constexpr unsigned int CP_MAX_MIPLEVELS = 16;

  static constexpr unsigned int CP_UDIR     = 0;
  static constexpr unsigned int CP_VDIR     = 1;
  static constexpr unsigned int CP_FACEAXIS = 2;

  // used to index cube faces
  static constexpr unsigned int CP_FACE_X_POS = 0;
  static constexpr unsigned int CP_FACE_X_NEG = 1;
  static constexpr unsigned int CP_FACE_Y_POS = 2;
  static constexpr unsigned int CP_FACE_Y_NEG = 3;
  static constexpr unsigned int CP_FACE_Z_POS = 4;
  static constexpr unsigned int CP_FACE_Z_NEG = 5;

  // used to index image edges
  // NOTE.. the actual number corresponding to the edge is important
  //  do not change these, or the code will break
  //
  // CP_EDGE_LEFT   is u = 0
  // CP_EDGE_RIGHT  is u = width-1
  // CP_EDGE_TOP    is v = 0
  // CP_EDGE_BOTTOM is v = height-1
  static constexpr unsigned int CP_EDGE_LEFT   = 0;
  static constexpr unsigned int CP_EDGE_RIGHT  = 1;
  static constexpr unsigned int CP_EDGE_TOP    = 2;
  static constexpr unsigned int CP_EDGE_BOTTOM = 3;

  // corners of CUBE map (P or N specifys if it corresponds to the
  //  positive or negative direction each of X, Y, and Z
  static constexpr unsigned int CP_CORNER_NNN = 0;
  static constexpr unsigned int CP_CORNER_NNP = 1;
  static constexpr unsigned int CP_CORNER_NPN = 2;
  static constexpr unsigned int CP_CORNER_NPP = 3;
  static constexpr unsigned int CP_CORNER_PNN = 4;
  static constexpr unsigned int CP_CORNER_PNP = 5;
  static constexpr unsigned int CP_CORNER_PPN = 6;
  static constexpr unsigned int CP_CORNER_PPP = 7;

  // 3x2 matrices that map cube map indexing vectors in 3d
  // (after face selection and divide through by the
  //  _ABSOLUTE VALUE_ of the max coord)
  // into NVC space
  // Note this currently assumes the D3D cube face ordering and orientation
  static const std::vector<std::vector<Float32Array>> _sgFace2DMapping;

  //----------------------------------------------------------------------------
  // D3D cube map face specification
  //   mapping from 3D x,y,z cube map lookup coordinates
  //   to 2D within face u,v coordinates
  //
  //   --------------------> U direction
  //   |                   (within-face texture space)
  //   |         _____
  //   |        |     |
  //   |        | +Y  |
  //   |   _____|_____|_____ _____
  //   |  |     |     |     |     |
  //   |  | -X  | +Z  | +X  | -Z  |
  //   |  |_____|_____|_____|_____|
  //   |        |     |
  //   |        | -Y  |
  //   |        |_____|
  //   |
  //   v   V direction
  //      (within-face texture space)
  //----------------------------------------------------------------------------

  // Information about neighbors and how texture coorrdinates change across
  // faces
  //  in ORDER of left, right, top, bottom (e.g. edges corresponding to u=0,
  //  u=1, v=0, v=1 in the 2D coordinate system of the particular face.
  // Note this currently assumes the D3D cube face ordering and orientation
  static const std::vector<std::vector<Uint32Array>> _sgCubeNgh;

  // The 12 edges of the cubemap, (entries are used to index into the neighbor
  // table) this table is used to average over the edges.
  static const std::vector<Uint32Array> _sgCubeEdgeList;

  // Information about which of the 8 cube corners are correspond to the
  //  the 4 corners in each cube face
  //  the order is upper left, upper right, lower left, lower right
  static const std::vector<Uint32Array> _sgCubeCornerList;

public:
  /**
   * Constructor of the generator.
   *
   * @param input The different faces data from the original cubemap in the
   * order X+ X- Y+ Y- Z+ Z-
   * @param inputSize The size of the cubemap faces
   * @param outputSize The size of the output cubemap faces
   * @param maxNumMipLevels The max number of mip map to generate (0 means all)
   * @param numChannels The number of channels stored in the cubemap (3 for RBGE
   * for instance)
   * @param isFloat Specifies if the input texture is in float or int (hdr is
   * usually in float)
   * @param specularPower The max specular level of the desired cubemap
   * @param cosinePowerDropPerMip The amount of drop the specular power will
   * follow on each mip
   * @param excludeBase Specifies wether to process the level 0 (original level)
   * or not
   * @param fixup Specifies wether to apply the edge fixup algorythm or not
   */
  PMREMGenerator(const std::vector<ArrayBufferView>& input, int inputSize, int outputSize,
                 size_t maxNumMipLevels, size_t numChannels, bool isFloat, float specularPower,
                 float cosinePowerDropPerMip, bool excludeBase, bool fixup);
  ~PMREMGenerator(); // = default

  /**
   * Launches the filter process and return the result.
   *
   * @return the filter cubemap in the form mip0 [faces1..6] .. mipN [faces1..6]
   */
  std::vector<std::vector<ArrayBufferView>>& filterCubeMap();

private:
  void init();

  //----------------------------------------------------------------------------
  // Cube map filtering and mip chain generation.
  // the cube map filtereing is specified using a number of parameters:
  // Filtering per miplevel is specified using 2D cone angle (in degrees) that
  //  indicates the region of the hemisphere to filter over for each tap.
  //
  // Note that the top mip level is also a filtered version of the original
  // input images
  //  as well in order to create mip chains for diffuse environment
  //  illumination.
  // The cone angle for the top level is specified by a_BaseAngle.  This can be
  // used to
  //  generate mipchains used to store the resutls of preintegration across the
  //  hemisphere.
  //
  // Then the mip angle used to genreate the next level of the mip chain from
  // the first level
  //  is a_InitialMipAngle
  //
  // The angle for the subsequent levels of the mip chain are specified by their
  // parents
  //  filtering angle and a per-level scale and bias
  //   newAngle = oldAngle * a_MipAnglePerLevelScale;
  //
  //----------------------------------------------------------------------------
  void filterCubeMapMipChain();

  //----------------------------------------------------------------------------
  // This function return the BaseFilterAngle require by PMREMGenerator to its
  // FilterExtends
  // It allow to optimize the texel to access base on the specular power.
  //----------------------------------------------------------------------------
  [[nodiscard]] float getBaseFilterAngle(float cosinePower) const;

  //----------------------------------------------------------------------------
  // Builds the following lookup tables prior to filtering:
  //  -normalizer cube map
  //  -tap weight lookup table
  //
  //----------------------------------------------------------------------------
  void precomputeFilterLookupTables(size_t srcCubeMapWidth);

  //----------------------------------------------------------------------------
  // Builds a normalizer cubemap, with the texels solid angle stored in the
  // fourth component
  //
  // Takes in a cube face size, and an array of 6 surfaces to write the cube
  // faces into
  //
  // Note that this normalizer cube map stores the vectors in unbiased -1 to 1
  // range.
  // if _bx2 style scaled and biased vectors are needed, uncomment the SCALE and
  // BIAS below
  //----------------------------------------------------------------------------
  void buildNormalizerSolidAngleCubemap(size_t size);

  //----------------------------------------------------------------------------
  // Convert cubemap face texel coordinates and face idx to 3D vector
  // note the U and V coords are integer coords and range from 0 to size-1
  //  this routine can be used to generate a normalizer cube map
  //----------------------------------------------------------------------------
  // SL BEGIN
  [[nodiscard]] Vector4 texelCoordToVect(unsigned int faceIdx, float u, float v, size_t size,
                                         bool fixup) const;

  //----------------------------------------------------------------------------
  // Convert 3D vector to cubemap face texel coordinates and face idx
  // note the U and V coords are integer coords and range from 0 to size-1
  //  this routine can be used to generate a normalizer cube map
  //
  // returns face IDX and texel coords
  //----------------------------------------------------------------------------
  // SL BEGIN
  /*
  Mapping Texture Coordinates to Cube Map Faces
  Because there are multiple faces, the mapping of texture coordinates to
  positions on cube map faces
  is more complicated than the other texturing targets.  The
  EXT_texture_cube_map extension is purposefully
  designed to be consistent with DirectX 7's cube map arrangement.  This is also
  consistent with the cube
  map arrangement in Pixar's RenderMan package.
  For cube map texturing, the (s,t,r) texture coordinates are treated as a
  direction vector (rx,ry,rz)
  emanating from the center of a cube.  (The q coordinate can be ignored since
  it merely scales the vector
  without affecting the direction.) At texture application time, the
  interpolated per-fragment (s,t,r)
  selects one of the cube map face's 2D mipmap sets based on the largest
  magnitude coordinate direction
  the major axis direction). The target column in the table below explains how
  the major axis direction
  maps to the 2D image of a particular cube map target.

  major axis
  direction     target                              sc     tc    ma
  ----------    ---------------------------------   ---    ---   ---
  +rx          GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT   -rz    -ry   rx
  -rx          GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT   +rz    -ry   rx
  +ry          GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT   +rx    +rz   ry
  -ry          GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT   +rx    -rz   ry
  +rz          GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT   +rx    -ry   rz
  -rz          GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT   -rx    -ry   rz

  Using the sc, tc, and ma determined by the major axis direction as specified
  in the table above,
  an updated (s,t) is calculated as follows
  s   =   ( sc/|ma| + 1 ) / 2
  t   =   ( tc/|ma| + 1 ) / 2
  If |ma| is zero or very nearly zero, the results of the above two equations
  need not be defined
  (though the result may not lead to GL interruption or termination).  Once the
  cube map face's 2D mipmap
  set and (s,t) is determined, texture fetching and filtering proceeds like
  standard OpenGL 2D texturing.
  */
  // Note this method return U and V in range from 0 to size-1
  // SL END
  // Store the information in vector3 for convenience (faceindex, u, v)
  [[nodiscard]] Vector4 vectToTexelCoord(float x, float y, float z, size_t size) const;

  //----------------------------------------------------------------------------
  // Original code from Ignacio CastaÒo
  // This formula is from Manne ÷hrstrˆm's thesis.
  // Take two coordiantes in the range [-1, 1] that define a portion of a
  // cube face and return the area of the projection of that portion on the
  // surface of the sphere.
  //----------------------------------------------------------------------------
  [[nodiscard]] float areaElement(float x, float y) const;

  [[nodiscard]] float texelCoordSolidAngle(unsigned int faceIdx, float u, float v,
                                           size_t size) const;

  void filterCubeSurfaces(const std::vector<ArrayBufferView>& srcCubeMap, float srcSize,
                          std::vector<ArrayBufferView>& dstCubeMap, size_t dstSize,
                          float filterConeAngle, float specularPower);

  //----------------------------------------------------------------------------
  // Clear filter extents for the 6 cube map faces
  //----------------------------------------------------------------------------
  void clearFilterExtents(std::array<CMGBoundinBox, 6>& filterExtents);

  //----------------------------------------------------------------------------
  // Define per-face bounding box filter extents
  //
  // These define conservative texel regions in each of the faces the filter can
  // possibly process.  When the pixels in the regions are actually processed,
  // the dot product between the tap vector and the center tap vector is used to
  // determine the weight of the tap and whether or not the tap is within the
  // cone.
  //
  //----------------------------------------------------------------------------
  void determineFilterExtents(const Vector4& centerTapDir, size_t srcSize, size_t bboxSize,
                              std::array<CMGBoundinBox, 6>& filterExtents) const;

  //----------------------------------------------------------------------------
  // ProcessFilterExtents
  //  Process bounding box in each cube face
  //
  //----------------------------------------------------------------------------
  Vector4 processFilterExtents(const Vector4& centerTapDir, float dotProdThresh,
                               const std::array<CMGBoundinBox, 6>& filterExtents,
                               const std::vector<ArrayBufferView>& srcCubeMap, size_t srcSize,
                               size_t specularPower) const;

  //----------------------------------------------------------------------------
  // Fixup cube edges
  //
  // average texels on cube map faces across the edges
  // WARP/BENT Method Only.
  //----------------------------------------------------------------------------
  void fixupCubeEdges(std::vector<ArrayBufferView>& cubeMap, size_t cubeMapSize);

public:
  std::vector<ArrayBufferView> input;
  int inputSize;
  int outputSize;
  size_t maxNumMipLevels;
  size_t numChannels;
  bool isFloat;
  float specularPower;
  float cosinePowerDropPerMip;
  bool excludeBase;
  bool fixup;

private:
  std::vector<std::vector<ArrayBufferView>> _outputSurface;
  std::vector<ArrayBufferView> _normCubeMap;
  std::vector<std::vector<ArrayBufferView>> _filterLUT;
  int _numMipLevels;

}; // end of class PMREMGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_HIGH_DYNAMIC_RANGE_PMREM_GENERATOR_H
