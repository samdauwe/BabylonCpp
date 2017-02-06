#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_ENUMS_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_ENUMS_H

namespace BABYLON {

/**
* Enums
*/
enum class EComponentType {
  BYTE           = 5120,
  UNSIGNED_BYTE  = 5121,
  SHORT          = 5122,
  UNSIGNED_SHORT = 5123,
  FLOAT          = 5126
};

enum class EShaderType { FRAGMENT = 35632, VERTEX = 35633 };

enum class EParameterType {
  BYTE           = 5120,
  UNSIGNED_BYTE  = 5121,
  SHORT          = 5122,
  UNSIGNED_SHORT = 5123,
  INT            = 5124,
  UNSIGNED_INT   = 5125,
  FLOAT          = 5126,
  FLOAT_VEC2     = 35664,
  FLOAT_VEC3     = 35665,
  FLOAT_VEC4     = 35666,
  INT_VEC2       = 35667,
  INT_VEC3       = 35668,
  INT_VEC4       = 35669,
  BOOL           = 35670,
  BOOL_VEC2      = 35671,
  BOOL_VEC3      = 35672,
  BOOL_VEC4      = 35673,
  FLOAT_MAT2     = 35674,
  FLOAT_MAT3     = 35675,
  FLOAT_MAT4     = 35676,
  SAMPLER_2D     = 35678
};

enum class ETextureWrapMode {
  CLAMP_TO_EDGE   = 33071,
  MIRRORED_REPEAT = 33648,
  REPEAT          = 10497
};

enum class ETextureFilterType {
  NEAREST                = 9728,
  LINEAR                 = 9728,
  NEAREST_MIPMAP_NEAREST = 9984,
  LINEAR_MIPMAP_NEAREST  = 9985,
  NEAREST_MIPMAP_LINEAR  = 9986,
  LINEAR_MIPMAP_LINEAR   = 9987
};

enum class ETextureFormat {
  ALPHA           = 6406,
  RGB             = 6407,
  RGBA            = 6408,
  LUMINANCE       = 6409,
  LUMINANCE_ALPHA = 6410
};

enum class ECullingType { FRONT = 1028, BACK = 1029, FRONT_AND_BACK = 1032 };

enum class EBlendingFunction {
  ZERO                     = 0,
  ONE                      = 1,
  SRC_COLOR                = 768,
  ONE_MINUS_SRC_COLOR      = 769,
  DST_COLOR                = 774,
  ONE_MINUS_DST_COLOR      = 775,
  SRC_ALPHA                = 770,
  ONE_MINUS_SRC_ALPHA      = 771,
  DST_ALPHA                = 772,
  ONE_MINUS_DST_ALPHA      = 773,
  CONSTANT_COLOR           = 32769,
  ONE_MINUS_CONSTANT_COLOR = 32770,
  CONSTANT_ALPHA           = 32771,
  ONE_MINUS_CONSTANT_ALPHA = 32772,
  SRC_ALPHA_SATURATE       = 776
};

enum class ETokenType { IDENTIFIER = 1, UNKNOWN = 2, END_OF_INPUT = 3 };

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_ENUMS_H
