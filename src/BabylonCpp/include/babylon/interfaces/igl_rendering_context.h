#ifndef BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H
#define BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class ICanvas;

namespace GL {

class BaseTexture;

using GLenum     = unsigned int;
using GLboolean  = bool;
using GLbitfield = unsigned int;
using GLbyte     = char;
using GLshort    = short;
using GLint      = int;
using GLsizei    = int;
using GLintptr   = long long;
using GLsizeiptr = long long;
using GLubyte    = unsigned char;
using GLushort   = unsigned short;
using GLuint     = unsigned int;
using GLfloat    = float;
using GLclampf   = float;
using GLint64    = long long;
using GLuint64   = unsigned long long;
using any        = void*;

// Structs
struct BABYLON_SHARED_EXPORT GLInfo {
  std::string vendor;
  std::string renderer;
  std::string version;
};

// Enums
enum GLEnums : GLenum {
  // FALSE = 0x0000,
  // TRUE  = 0x0001,
  // NO_ERROR = 0x0000,
  /* ClearBufferMask */
  DEPTH_BUFFER_BIT   = 0x00000100,
  STENCIL_BUFFER_BIT = 0x00000400,
  COLOR_BUFFER_BIT   = 0x00004000,
  /* BeginMode */
  POINTS         = 0x0000,
  LINES          = 0x0001,
  LINE_LOOP      = 0x0002,
  LINE_STRIP     = 0x0003,
  TRIANGLES      = 0x0004,
  TRIANGLE_STRIP = 0x0005,
  TRIANGLE_FAN   = 0x0006,
  /* BlendingFactorDest */
  ZERO                = 0x0000,
  ONE                 = 0x0001,
  SRC_COLOR           = 0x0300,
  ONE_MINUS_SRC_COLOR = 0x0301,
  SRC_ALPHA           = 0x0302,
  ONE_MINUS_SRC_ALPHA = 0x0303,
  DST_ALPHA           = 0x0304,
  ONE_MINUS_DST_ALPHA = 0x0305,
  /* BlendingFactorSrc */
  DST_COLOR           = 0x0306,
  ONE_MINUS_DST_COLOR = 0x0307,
  SRC_ALPHA_SATURATE  = 0x0308,
  /* BlendEquationSeparate  */
  FUNC_ADD       = 0x8006,
  BLEND_EQUATION = 0x8009,
  /* MIN / MAX */
  MIN = 0x8007,
  MAX = 0x8008,
  /* same as BLEND_EQUATION */
  BLEND_EQUATION_RGB   = 0x8009,
  BLEND_EQUATION_ALPHA = 0x883D,
  /* BlendSubtract */
  FUNC_SUBTRACT         = 0x800A,
  FUNC_REVERSE_SUBTRACT = 0x800B,
  /* Separate Blend Functions */
  BLEND_DST_RGB            = 0x80C8,
  BLEND_SRC_RGB            = 0x80C9,
  BLEND_DST_ALPHA          = 0x80CA,
  BLEND_SRC_ALPHA          = 0x80CB,
  CONSTANT_COLOR           = 0x8001,
  ONE_MINUS_CONSTANT_COLOR = 0x8002,
  CONSTANT_ALPHA           = 0x8003,
  ONE_MINUS_CONSTANT_ALPHA = 0x8004,
  BLEND_COLOR              = 0x8005,
  /* Buffer Objects */
  ARRAY_BUFFER                 = 0x8892,
  ELEMENT_ARRAY_BUFFER         = 0x8893,
  ARRAY_BUFFER_BINDING         = 0x8894,
  ELEMENT_ARRAY_BUFFER_BINDING = 0x8895,
  STREAM_DRAW                  = 0x88E0,
  STATIC_DRAW                  = 0x88E4,
  DYNAMIC_DRAW                 = 0x88E8,
  BUFFER_SIZE                  = 0x8764,
  BUFFER_USAGE                 = 0x8765,
  QUERY_RESULT                 = 0x8866,
  QUERY_RESULT_AVAILABLE       = 0x8867,
  /* CullFaceMode */
  FRONT          = 0x0404,
  BACK           = 0x0405,
  FRONT_AND_BACK = 0x0408,
  /* DepthFunction */
  /* EnableCap */
  CULL_FACE                = 0x0B44,
  BLEND                    = 0x0BE2,
  DITHER                   = 0x0BD0,
  STENCIL_TEST             = 0x0B90,
  DEPTH_TEST               = 0x0B71,
  SCISSOR_TEST             = 0x0C11,
  POLYGON_OFFSET_FILL      = 0x8037,
  SAMPLE_ALPHA_TO_COVERAGE = 0x809E,
  SAMPLE_COVERAGE          = 0x80A0,
  /* ErrorCode */
  // NO_ERROR          = 0x0000,
  INVALID_ENUM      = 0x0500,
  INVALID_VALUE     = 0x0501,
  INVALID_OPERATION = 0x0502,
  OUT_OF_MEMORY     = 0x0505,
  /* FrontFaceDirection */
  CW  = 0x0900,
  CCW = 0x0901,
  /* GetPName */
  LINE_WIDTH                   = 0x0B21,
  ALIASED_POINT_SIZE_RANGE     = 0x846D,
  ALIASED_LINE_WIDTH_RANGE     = 0x846E,
  CULL_FACE_MODE               = 0x0B45,
  FRONT_FACE                   = 0x0B46,
  DEPTH_RANGE                  = 0x0B70,
  DEPTH_WRITEMASK              = 0x0B72,
  DEPTH_CLEAR_VALUE            = 0x0B73,
  DEPTH_FUNC                   = 0x0B74,
  STENCIL_CLEAR_VALUE          = 0x0B91,
  STENCIL_FUNC                 = 0x0B92,
  STENCIL_FAIL                 = 0x0B94,
  STENCIL_PASS_DEPTH_FAIL      = 0x0B95,
  STENCIL_PASS_DEPTH_PASS      = 0x0B96,
  STENCIL_REF                  = 0x0B97,
  STENCIL_VALUE_MASK           = 0x0B93,
  STENCIL_WRITEMASK            = 0x0B98,
  STENCIL_BACK_FUNC            = 0x8800,
  STENCIL_BACK_FAIL            = 0x8801,
  STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802,
  STENCIL_BACK_PASS_DEPTH_PASS = 0x8803,
  STENCIL_BACK_REF             = 0x8CA3,
  STENCIL_BACK_VALUE_MASK      = 0x8CA4,
  STENCIL_BACK_WRITEMASK       = 0x8CA5,
  VIEWPORT                     = 0x0BA2,
  SCISSOR_BOX                  = 0x0C10,
  /* SCISSOR_TEST */
  COLOR_CLEAR_VALUE    = 0x0C22,
  COLOR_WRITEMASK      = 0x0C23,
  UNPACK_ALIGNMENT     = 0x0CF5,
  PACK_ALIGNMENT       = 0x0D05,
  MAX_TEXTURE_SIZE     = 0x0D33,
  MAX_VIEWPORT_DIMS    = 0x0D3A,
  SUBPIXEL_BITS        = 0x0D50,
  RED_BITS             = 0x0D52,
  GREEN_BITS           = 0x0D53,
  BLUE_BITS            = 0x0D54,
  ALPHA_BITS           = 0x0D55,
  DEPTH_BITS           = 0x0D56,
  STENCIL_BITS         = 0x0D57,
  POLYGON_OFFSET_UNITS = 0x2A00,
  /* TRANSFORM FEEDBACK */
  INTERLEAVED_ATTRIBS       = 0x8C8C,
  TRANSFORM_FEEDBACK_BUFFER = 0x8C8E,
  TRANSFORM_FEEDBACK        = 0x8E22,
  /*  POLYGON_OFFSET_FILL*/
  POLYGON_OFFSET_FACTOR  = 0x8038,
  TEXTURE_BINDING_2D     = 0x8069,
  SAMPLE_BUFFERS         = 0x80A8,
  SAMPLES                = 0x80A9,
  SAMPLE_COVERAGE_VALUE  = 0x80AA,
  SAMPLE_COVERAGE_INVERT = 0x80AB,
  /* GetTextureParameter */
  COMPRESSED_TEXTURE_FORMATS = 0x86A3,
  /* HintMode */
  DONT_CARE = 0x1100,
  FASTEST   = 0x1101,
  NICEST    = 0x1102,
  /* HintTarget */
  GENERATE_MIPMAP_HINT = 0x8192,
  /* DataType */
  BYTE           = 0x1400,
  UNSIGNED_BYTE  = 0x1401,
  SHORT          = 0x1402,
  UNSIGNED_SHORT = 0x1403,
  INT            = 0x1404,
  UNSIGNED_INT   = 0x1405,
  FLOAT          = 0x1406,
  /* PixelFormat */
  DEPTH_COMPONENT = 0x1902,
  ALPHA           = 0x1906,
  RGB             = 0x1907,
  RGBA            = 0x1908,
  LUMINANCE       = 0x1909,
  LUMINANCE_ALPHA = 0x190A,
  /* PixelType */
  UNSIGNED_SHORT_4_4_4_4 = 0x8033,
  UNSIGNED_SHORT_5_5_5_1 = 0x8034,
  UNSIGNED_SHORT_5_6_5   = 0x8363,
  /* Color schemes */
  /* RGBA 32-bit floating-point color-renderable internal sized format */
  RGBA32F = 0x8814,
  /* RGB 32-bit floating-point color-renderable internal sized format */
  RGB32F = 0x8815,
  /* RGBA 16-bit floating-point color-renderable internal sized format */
  RGBA16F = 0x881A,
  /* RGB 16-bit floating-point color-renderable internal sized format */
  RGB16F = 0x881B,
  /* Uniform Buffers */
  UNIFORM_BUFFER = 0x8A11,
  /* Shaders */
  FRAGMENT_SHADER                  = 0x8B30,
  VERTEX_SHADER                    = 0x8B31,
  MAX_VERTEX_ATTRIBS               = 0x8869,
  MAX_VERTEX_UNIFORM_VECTORS       = 0x8DFB,
  MAX_VARYING_VECTORS              = 0x8DFC,
  MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
  MAX_VERTEX_TEXTURE_IMAGE_UNITS   = 0x8B4C,
  MAX_TEXTURE_IMAGE_UNITS          = 0x8872,
  MAX_FRAGMENT_UNIFORM_VECTORS     = 0x8DFD,
  SHADER_TYPE                      = 0x8B4F,
  DELETE_STATUS                    = 0x8B80,
  LINK_STATUS                      = 0x8B82,
  VALIDATE_STATUS                  = 0x8B83,
  ATTACHED_SHADERS                 = 0x8B85,
  ACTIVE_UNIFORMS                  = 0x8B86,
  ACTIVE_ATTRIBUTES                = 0x8B89,
  SHADING_LANGUAGE_VERSION         = 0x8B8C,
  CURRENT_PROGRAM                  = 0x8B8D,
  /* Algorithm types */
  ANY_SAMPLES_PASSED              = 0x8C2F,
  ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A,
  /* Half floating-point type (16-bit) */
  HALF_FLOAT     = 0x140B,
  HALF_FLOAT_OES = 0x8D61,
  /* Other types */
  RGB8                           = 0x8051,
  RED_INTEGER                    = 0x8D94,
  RG_INTEGER                     = 0x8228,
  RGB_INTEGER                    = 0x8D98,
  RGBA_INTEGER                   = 0x8D99,
  R8_SNORM                       = 0x8F94,
  RG8_SNORM                      = 0x8F95,
  RGB8_SNORM                     = 0x8F96,
  RGBA8_SNORM                    = 0x8F97,
  R8I                            = 0x8231,
  RG8I                           = 0x8237,
  RGB8I                          = 0x8D8F,
  RGBA8I                         = 0x8D8E,
  R8UI                           = 0x8232,
  RG8UI                          = 0x8238,
  RGB8UI                         = 0x8D7D,
  RGBA8UI                        = 0x8D7C,
  R16I                           = 0x8233,
  RG16I                          = 0x8239,
  RGB16I                         = 0x8D89,
  RGBA16I                        = 0x8D88,
  R16UI                          = 0x8234,
  RG16UI                         = 0x823A,
  RGB16UI                        = 0x8D77,
  RGBA16UI                       = 0x8D76,
  R32I                           = 0x8235,
  RG32I                          = 0x823B,
  RGB32I                         = 0x8D83,
  RGBA32I                        = 0x8D82,
  R32UI                          = 0x8236,
  RG32UI                         = 0x823C,
  RGB32UI                        = 0x8D71,
  RGBA32UI                       = 0x8D70,
  RGB10_A2UI                     = 0x906F,
  R11F_G11F_B10F                 = 0x8C3A,
  RGB9_E5                        = 0x8C3D,
  RGB10_A2                       = 0x8059,
  UNSIGNED_INT_2_10_10_10_REV    = 0x8368,
  UNSIGNED_INT_10F_11F_11F_REV   = 0x8C3B,
  UNSIGNED_INT_5_9_9_9_REV       = 0x8C3E,
  FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD,
  /* StencilFunction */
  NEVER    = 0x0200,
  LESS     = 0x0201,
  EQUAL    = 0x0202,
  LEQUAL   = 0x0203,
  GREATER  = 0x0204,
  NOTEQUAL = 0x0205,
  GEQUAL   = 0x0206,
  ALWAYS   = 0x0207,
  /* StencilOp */
  KEEP      = 0x1E00,
  REPLACE   = 0x1E01,
  INCR      = 0x1E02,
  DECR      = 0x1E03,
  INVERT    = 0x150A,
  INCR_WRAP = 0x8507,
  DECR_WRAP = 0x8508,
  /* StringName */
  VENDOR     = 0x1F00,
  RENDERER   = 0x1F01,
  VERSION    = 0x1F02,
  EXTENSIONS = 0x1F03,
  /* Textures */
  TEXTURE_3D             = 0x806F,
  TEXTURE_2D_ARRAY       = 0x8C1A,
  TEXTURE_COMPARE_MODE   = 0x884C,
  TEXTURE_COMPARE_FUNC   = 0x884D,
  COMPARE_REF_TO_TEXTURE = 0x884E,
  /* TextureMagFilter */
  NEAREST = 0x2600,
  LINEAR  = 0x2601,
  /* TextureMinFilter */
  NEAREST_MIPMAP_NEAREST = 0x2700,
  LINEAR_MIPMAP_NEAREST  = 0x2701,
  NEAREST_MIPMAP_LINEAR  = 0x2702,
  LINEAR_MIPMAP_LINEAR   = 0x2703,
  /* TextureParameterName */
  TEXTURE_MAG_FILTER = 0x2800,
  TEXTURE_MIN_FILTER = 0x2801,
  TEXTURE_WRAP_S     = 0x2802,
  TEXTURE_WRAP_T     = 0x2803,
  TEXTURE_WRAP_R     = 0x8072,
  /* TextureTarget */
  TEXTURE_2D                  = 0x0DE1,
  TEXTURE                     = 0x1702,
  TEXTURE_CUBE_MAP            = 0x8513,
  TEXTURE_BINDING_CUBE_MAP    = 0x8514,
  TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515,
  TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516,
  TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517,
  TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518,
  TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519,
  TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A,
  MAX_CUBE_MAP_TEXTURE_SIZE   = 0x851C,
  /* TextureUnit */
  TEXTURE0       = 0x84C0,
  TEXTURE1       = 0x84C1,
  TEXTURE2       = 0x84C2,
  TEXTURE3       = 0x84C3,
  TEXTURE4       = 0x84C4,
  TEXTURE5       = 0x84C5,
  TEXTURE6       = 0x84C6,
  TEXTURE7       = 0x84C7,
  TEXTURE8       = 0x84C8,
  TEXTURE9       = 0x84C9,
  TEXTURE10      = 0x84CA,
  TEXTURE11      = 0x84CB,
  TEXTURE12      = 0x84CC,
  TEXTURE13      = 0x84CD,
  TEXTURE14      = 0x84CE,
  TEXTURE15      = 0x84CF,
  TEXTURE16      = 0x84D0,
  TEXTURE17      = 0x84D1,
  TEXTURE18      = 0x84D2,
  TEXTURE19      = 0x84D3,
  TEXTURE20      = 0x84D4,
  TEXTURE21      = 0x84D5,
  TEXTURE22      = 0x84D6,
  TEXTURE23      = 0x84D7,
  TEXTURE24      = 0x84D8,
  TEXTURE25      = 0x84D9,
  TEXTURE26      = 0x84DA,
  TEXTURE27      = 0x84DB,
  TEXTURE28      = 0x84DC,
  TEXTURE29      = 0x84DD,
  TEXTURE30      = 0x84DE,
  TEXTURE31      = 0x84DF,
  ACTIVE_TEXTURE = 0x84E0,
  /* TextureWrapMode */
  REPEAT          = 0x2901,
  CLAMP_TO_EDGE   = 0x812F,
  MIRRORED_REPEAT = 0x8370,
  /* Uniform Types */
  FLOAT_VEC2   = 0x8B50,
  FLOAT_VEC3   = 0x8B51,
  FLOAT_VEC4   = 0x8B52,
  INT_VEC2     = 0x8B53,
  INT_VEC3     = 0x8B54,
  INT_VEC4     = 0x8B55,
  BOOL         = 0x8B56,
  BOOL_VEC2    = 0x8B57,
  BOOL_VEC3    = 0x8B58,
  BOOL_VEC4    = 0x8B59,
  FLOAT_MAT2   = 0x8B5A,
  FLOAT_MAT3   = 0x8B5B,
  FLOAT_MAT4   = 0x8B5C,
  SAMPLER_2D   = 0x8B5E,
  SAMPLER_CUBE = 0x8B60,
  /* Vertex Arrays */
  VERTEX_ATTRIB_ARRAY_ENABLED        = 0x8622,
  VERTEX_ATTRIB_ARRAY_SIZE           = 0x8623,
  VERTEX_ATTRIB_ARRAY_STRIDE         = 0x8624,
  VERTEX_ATTRIB_ARRAY_TYPE           = 0x8625,
  CURRENT_VERTEX_ATTRIB              = 0x8626,
  VERTEX_ATTRIB_ARRAY_NORMALIZED     = 0x886A,
  VERTEX_ATTRIB_ARRAY_POINTER        = 0x8645,
  VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F,
  /* Shader Source */
  COMPILE_STATUS = 0x8B81,
  /* Shader Precision-Specified Types */
  LOW_FLOAT    = 0x8DF0,
  MEDIUM_FLOAT = 0x8DF1,
  HIGH_FLOAT   = 0x8DF2,
  LOW_INT      = 0x8DF3,
  MEDIUM_INT   = 0x8DF4,
  HIGH_INT     = 0x8DF5,
  /* Framebuffer Object. */
  FRAMEBUFFER                                  = 0x8D40,
  RENDERBUFFER                                 = 0x8D41,
  RGBA4                                        = 0x8056,
  RGB5_A1                                      = 0x8057,
  RGBA8                                        = 0x8058,
  RGB565                                       = 0x8D62,
  DEPTH_COMPONENT16                            = 0x81A5,
  DEPTH_COMPONENT24                            = 0x81A6,
  STENCIL_INDEX                                = 0x1901,
  RED                                          = 0x1903,
  STENCIL_INDEX8                               = 0x8D48,
  DEPTH_STENCIL                                = 0x84F9,
  UNSIGNED_INT_24_8                            = 0x84FA,
  DEPTH24_STENCIL8                             = 0x88F0,
  RENDERBUFFER_WIDTH                           = 0x8D42,
  RENDERBUFFER_HEIGHT                          = 0x8D43,
  RENDERBUFFER_INTERNAL_FORMAT                 = 0x8D44,
  RENDERBUFFER_RED_SIZE                        = 0x8D50,
  RENDERBUFFER_GREEN_SIZE                      = 0x8D51,
  RENDERBUFFER_BLUE_SIZE                       = 0x8D52,
  RENDERBUFFER_ALPHA_SIZE                      = 0x8D53,
  RENDERBUFFER_DEPTH_SIZE                      = 0x8D54,
  RENDERBUFFER_STENCIL_SIZE                    = 0x8D55,
  MAX_SAMPLES                                  = 0x8D57,
  RASTERIZER_DISCARD                           = 0x8C89,
  FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE           = 0x8CD0,
  FRAMEBUFFER_ATTACHMENT_OBJECT_NAME           = 0x8CD1,
  FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL         = 0x8CD2,
  FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3,
  COLOR_ATTACHMENT0                            = 0x8CE0,
  COLOR_ATTACHMENT1                            = 0x8CE1,
  COLOR_ATTACHMENT2                            = 0x8CE2,
  COLOR_ATTACHMENT3                            = 0x8CE3,
  COLOR_ATTACHMENT4                            = 0x8CE4,
  COLOR_ATTACHMENT5                            = 0x8CE5,
  COLOR_ATTACHMENT6                            = 0x8CE6,
  COLOR_ATTACHMENT7                            = 0x8CE7,
  COLOR_ATTACHMENT8                            = 0x8CE8,
  COLOR_ATTACHMENT9                            = 0x8CE9,
  COLOR_ATTACHMENT10                           = 0x8CEA,
  COLOR_ATTACHMENT11                           = 0x8CEB,
  COLOR_ATTACHMENT12                           = 0x8CEC,
  COLOR_ATTACHMENT13                           = 0x8CED,
  COLOR_ATTACHMENT14                           = 0x8CEE,
  COLOR_ATTACHMENT15                           = 0x8CEF,
  COLOR_ATTACHMENT16                           = 0x8CF0,
  COLOR_ATTACHMENT17                           = 0x8CF1,
  COLOR_ATTACHMENT18                           = 0x8CF2,
  COLOR_ATTACHMENT19                           = 0x8CF3,
  COLOR_ATTACHMENT20                           = 0x8CF4,
  COLOR_ATTACHMENT21                           = 0x8CF5,
  COLOR_ATTACHMENT22                           = 0x8CF6,
  COLOR_ATTACHMENT23                           = 0x8CF7,
  COLOR_ATTACHMENT24                           = 0x8CF8,
  COLOR_ATTACHMENT25                           = 0x8CF9,
  COLOR_ATTACHMENT26                           = 0x8CFA,
  COLOR_ATTACHMENT27                           = 0x8CFB,
  COLOR_ATTACHMENT28                           = 0x8CFC,
  COLOR_ATTACHMENT29                           = 0x8CFD,
  COLOR_ATTACHMENT30                           = 0x8CFE,
  COLOR_ATTACHMENT31                           = 0x8CFF,
  DEPTH_ATTACHMENT                             = 0x8D00,
  STENCIL_ATTACHMENT                           = 0x8D20,
  DEPTH_STENCIL_ATTACHMENT                     = 0x821A,
  RG                                           = 0x8227,
  R8                                           = 0x8229,
  RG8                                          = 0x822B,
  R16F                                         = 0x822D,
  R32F                                         = 0x822E,
  RG16F                                        = 0x822F,
  RG32F                                        = 0x8230,
  NONE                                         = 0x0000,
  FRAMEBUFFER_COMPLETE                         = 0x8CD5,
  FRAMEBUFFER_INCOMPLETE_ATTACHMENT            = 0x8CD6,
  FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    = 0x8CD7,
  FRAMEBUFFER_INCOMPLETE_DIMENSIONS            = 0x8CD9,
  FRAMEBUFFER_UNSUPPORTED                      = 0x8CDD,
  FRAMEBUFFER_BINDING                          = 0x8CA6,
  RENDERBUFFER_BINDING                         = 0x8CA7,
  READ_FRAMEBUFFER                             = 0x8CA8,
  DRAW_FRAMEBUFFER                             = 0x8CA9,
  DEPTH_COMPONENT32F                           = 0x8CAC,
  DEPTH32F_STENCIL8                            = 0x8CAD,
  MAX_RENDERBUFFER_SIZE                        = 0x84E8,
  INVALID_FRAMEBUFFER_OPERATION                = 0x0506,
  /* WebGL-specific enums */
  UNPACK_FLIP_Y_WEBGL                = 0x9240,
  UNPACK_PREMULTIPLY_ALPHA_WEBGL     = 0x9241,
  CONTEXT_LOST_WEBGL                 = 0x9242,
  UNPACK_COLORSPACE_CONVERSION_WEBGL = 0x9243,
  BROWSER_DEFAULT_WEBGL              = 0x9244,
  /* KHR_parallel_shader_compile */
  COMPLETION_STATUS_KHR = 0x91B1,
  // IGL_EXT_texture_filter_anisotropic
  TEXTURE_MAX_ANISOTROPY_EXT     = 0x84FE,
  MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF,
  // Debugging
  DEBUG_TYPE_ERROR = 0x824C
}; // end of enum GLEnums

class IGLFramebuffer;
class IGLProgram;
class IGLRenderbuffer;
class IGLShader;
class IGLTexture;
class IGLTransformFeedback;
class IGLVertexArrayObject;
using IGLFramebufferPtr       = std::shared_ptr<IGLFramebuffer>;
using IGLProgramPtr           = std::shared_ptr<IGLProgram>;
using IGLRenderbufferPtr      = std::shared_ptr<IGLRenderbuffer>;
using IGLShaderPtr            = std::shared_ptr<IGLShader>;
using IGLTexturePtr           = std::shared_ptr<IGLTexture>;
using IGLTransformFeedbackPtr = std::shared_ptr<IGLTransformFeedback>;
using IGLVertexArrayObjectPtr = std::shared_ptr<IGLVertexArrayObject>;

class BABYLON_SHARED_EXPORT IGLBuffer {

public:
  IGLBuffer(GLuint _value) : references{1}, is32Bits{true}, capacity{1}, value{_value}
  {
  }

public:
  size_t references;
  bool is32Bits;
  unsigned int capacity;
  GLuint value;

}; // end of class IGLBuffer

class BABYLON_SHARED_EXPORT IGLFramebuffer {

public:
  IGLFramebuffer(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;

}; // end of class IGLFramebuffer

class BABYLON_SHARED_EXPORT IGLTransformFeedback {

public:
  IGLTransformFeedback(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;

}; // end of class IGLTransformFeedback

class BABYLON_SHARED_EXPORT IGLProgram {

public:
  IGLProgram(GLuint _value)
      : isParallelCompiled{false}
      , value{_value}
      , transformFeedback{nullptr}
      , __SPECTOR_rebuildProgram{nullptr}
  {
  }

public:
  bool isParallelCompiled;
  GLuint value;
  IGLTransformFeedbackPtr transformFeedback;
  std::function<void(const std::string& vertexSourceCode, const std::string& fragmentSourceCode,
                     const std::function<void(GL::IGLProgram* program)>& onCompiled,
                     const std::function<void(const std::string& message)>& onError)>
    __SPECTOR_rebuildProgram;

}; // end of class IGLProgram

class BABYLON_SHARED_EXPORT IGLQuery {

public:
  IGLQuery(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;

}; // end of class IGLQuery

struct BABYLON_SHARED_EXPORT EXT_disjoint_timer_query {
  int QUERY_COUNTER_BITS_EXT;
  int TIME_ELAPSED_EXT;
  int TIMESTAMP_EXT;
  int GPU_DISJOINT_EXT;
  int QUERY_RESULT_EXT;
  int QUERY_RESULT_AVAILABLE_EXT;
  void queryCounterEXT(IGLQuery* /*query*/, int /*target*/)
  {
  }
  std::unique_ptr<IGLQuery> createQueryEXT()
  {
    return nullptr;
  }
  void beginQueryEXT(int /*target*/, IGLQuery* /*query*/)
  {
  }
  void endQueryEXT(int /*target*/)
  {
  }
  int getQueryObjectEXT(IGLQuery* /*query*/, int /*target*/)
  {
    return -1;
  }
  void deleteQueryEXT(IGLQuery* /*query*/)
  {
  }
}; // end of struct EXT_disjoint_timer_query

struct BABYLON_SHARED_EXPORT EXT_texture_filter_anisotropic {

}; // end of struct EXT_texture_filter_anisotropic

struct BABYLON_SHARED_EXPORT WEBGL_compressed_texture_s3tc {

}; // end of struct WEBGL_compressed_texture_s3tc

class BABYLON_SHARED_EXPORT IGLRenderbuffer {

public:
  IGLRenderbuffer(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;

}; // end of class IGLRenderbuffer

class BABYLON_SHARED_EXPORT IGLShader {

public:
  IGLShader(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;
}; // end of class IGLShader

class BABYLON_SHARED_EXPORT IGLShaderPrecisionFormat {
public:
  GLint rangeMin;
  GLint rangeMax;
  GLint precision;
}; // end of class IGLShaderPrecisionFormat

/**
 * @brief The IGLTexture interface provides the GL rendering context
 */
class BABYLON_SHARED_EXPORT IGLTexture {

public:
  IGLTexture(GLuint _value)
      : value{_value}
      , _width{-1}
      , _height{-1}
      , _size{-1}
      , _baseWidth{-1}
      , _baseHeight{-1}
      , samplingMode{0}
      , samples{1}
      , references{0}
      , isCube{false}
      , isReady{false}
      , generateMipMaps{false}
      , noMipmap{false}
      , _cachedWrapU{0}
      , _cachedWrapV{0}
      , _cachedCoordinatesMode{0}
      , _generateDepthBuffer{false}
      , _generateStencilBuffer{false}
      , url{""}
      , _framebuffer{nullptr}
      , _depthBuffer{nullptr}
  {
  }

public:
  GLuint value;
  int _width;
  int _height;
  int _size;
  int _baseWidth;
  int _baseHeight;
  unsigned int samplingMode;
  unsigned int samples;
  int references;
  bool isCube;
  bool isReady;
  bool generateMipMaps;
  bool noMipmap;
  std::optional<unsigned int> type;
  std::optional<unsigned int> format;
  unsigned int _cachedWrapU;
  unsigned int _cachedWrapV;
  unsigned int _cachedCoordinatesMode;
  bool _generateDepthBuffer;
  bool _generateStencilBuffer;
  BaseTexture* _lodTextureHigh;
  BaseTexture* _lodTextureMid;
  BaseTexture* _lodTextureLow;
  std::string url;
  std::unique_ptr<IGLFramebuffer> _framebuffer;
  std::unique_ptr<IGLFramebuffer> _MSAAFramebuffer;
  std::unique_ptr<IGLRenderbuffer> _depthBuffer;
  std::unique_ptr<IGLRenderbuffer> _depthStencilBuffer;
  std::unique_ptr<IGLRenderbuffer> _MSAARenderBuffer;
  // std::unique_ptr<SphericalPolynomial> _sphericalPolynomial;
  std::vector<std::function<void()>> onLoadedCallbacks;
}; // end of class IGLTexture

class BABYLON_SHARED_EXPORT IGLUniformLocation {

public:
  IGLUniformLocation(GLint _value) : value{_value}
  {
  }

public:
  GLint value;
  int _currentState;

}; // end of class IGLUniformLocation

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT IGLRenderingContext {

public:
  // Half floating-point type (16-bit).
  unsigned int HALF_FLOAT_OES = GL::HALF_FLOAT_OES;
  // Drawing buffer width and height
  int drawingBufferWidth;
  int drawingBufferHeight;
  // Textures
  int RASTERIZER_DISCARD;
  int TEXTURE_3D;
  int TEXTURE_2D_ARRAY;
  int TEXTURE_WRAP_R;
  // Transform feedback
  int TRANSFORM_FEEDBACK;
  int INTERLEAVED_ATTRIBS;
  int TRANSFORM_FEEDBACK_BUFFER;

public:
  virtual ~IGLRenderingContext()                          = default;
  virtual bool initialize(bool enableGLDebugging = false) = 0;
  // virtual ICanvas* getCanvas() = 0;
  // virtual GLsizei getDrawingBufferWidth() = 0;
  // virtual GLsizei getDrawingBufferHeight() = 0;
  // virtual GLContextAttributes getContextAttributes() = 0;
  // virtual bool isContextLost() = 0;
  // virtual std::vector<std::string> getSupportedExtensions() = 0;
  // virtual any getExtension(const std::string& name) = 0;
  virtual GLenum operator[](const std::string& name)                = 0;
  virtual void activeTexture(GLenum texture)                        = 0;
  virtual void attachShader(IGLProgram* program, IGLShader* shader) = 0;

  /**
   * @brief Starts an asynchronous query. The target parameter indicates which
   * kind of query to begin.
   * @param target A GLenum specifying the target of the query. Possible values:
   *  - ANY_SAMPLES_PASSED: Specifies an occlusion query: these queries detect
   * whether an object is visible (whether the scoped drawing commands pass the
   * depth test and if so, how many samples pass).
   *  - ANY_SAMPLES_PASSED_CONSERVATIVE: Same as above above, but less accurate
   * and faster version.
   *  - TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN: Number of primitives that are
   * written to transform feedback buffers.
   * @param query An IGLQuery object for which to start the querying.
   */
  virtual void beginQuery(GLenum target, IGLQuery* query) = 0;

  /**
   * @brief Starts a transform feedback operation.
   * @param primitiveMode A GLenum specifying the output type of the primitives
   * that will be recorded into the buffer objects that are bound for transform
   * feedback.
   */
  virtual void beginTransformFeedback(GLenum primitiveMode) = 0;

  /**
   * @brief Binds a generic vertex index to a named attribute variable.
   * @param program A IGLProgram object to bind.
   * @param index A GLuint specifying the index of the generic vertex to bind.
   * @param name A string specifying the name of the variable to bind to the
   * generic vertex index.
   */
  virtual void bindAttribLocation(IGLProgram* program, GLuint index, const std::string& name) = 0;

  /**
   * @brief Binds a given IGLBuffer to a target.
   * @param target A GLenum specifying the binding point (target).
   * @param buffer A IGLBuffer to bind.
   */
  virtual void bindBuffer(GLenum target, IGLBuffer* buffer) = 0;

  /**
   * @brief Binds a given IGLFramebuffer to a target.
   * @param target A GLenum specifying the binding point (target).
   * @param framebuffer A IGLFramebuffer object to bind.
   */
  virtual void bindFramebuffer(GLenum target, IGLFramebuffer* framebuffer) = 0;

  /**
   * @brief bindBufferBase
   * @param target
   * @param index
   * @param buffer
   */
  virtual void bindBufferBase(GLenum target, GLuint index, IGLBuffer* buffer) = 0;

  /**
   * @brief Binds a IGLRenderbuffer object to a given target.
   * @param target A GLenum specifying the binding point (target).
   * @param renderbuffer A IGLRenderbuffer object to bind.
   */
  virtual void bindRenderbuffer(GLenum target, IGLRenderbuffer* renderbuffer) = 0;

  /**
   * @brief Binds a IGLTexture object to a given target.
   * @param target A GLenum specifying the binding point (target).
   * @param texture A IGLTexture object to bind.
   */
  virtual void bindTexture(GLenum target, IGLTexture* texture) = 0;

  /**
   * @brief Binds a passed IGLTransformFeedback object to the current GL state.
   * @param target A GLenum specifying the target (binding point). Must be
   * GL::TRANSFORM_FEEDBACK.
   * @param transformFeedback A IGLTransformFeedback object to bind.
   */
  virtual void bindTransformFeedback(GLenum target, IGLTransformFeedback* transformFeedback) = 0;

  /**
   * @brief Sets the source and destination blending factors.
   * @param red A GLclampf for the red component in the range of 0 to 1.
   * @param green A GLclampf for the green component in the range of 0 to 1.
   * @param blue A GLclampf for the blue component in the range of 0 to 1.
   * @param alpha A GLclampf for the alpha component (transparency) in the range
   * of 0 to 1.
   */
  virtual void blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;

  /**
   * @brief Sets both the RGB blend equation and alpha blend equation to a
   * single equation.
   * @param mode A GLenum specifying how source and destination colors are
   * combined.
   */
  virtual void blendEquation(GLenum mode) = 0;

  /**
   * @brief Sets the RGB blend equation and alpha blend equation separately.
   * @param modeRGB A GLenum specifying how the red, green and blue components
   * of source and destination colors are combined.
   * @param modeAlpha A GLenum specifying how the alpha component (transparency)
   * of source and destination colors are combined.
   */
  virtual void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) = 0;

  /**
   * @brief Defines which function is used for blending pixel arithmetic.
   * @param sfactor A GLenum specifying a multiplier for the source blending
   * factors.
   * @param dfactor A GLenum specifying a multiplier for the destination
   * blending factors.
   */
  virtual void blendFunc(GLenum sfactor, GLenum dfactor) = 0;

  /**
   * @brief defines which function is used for blending pixel arithmetic for RGB
   * and alpha components separately.
   * @param srcRGB A GLenum specifying a multiplier for the red, green and blue
   * (RGB) source blending factors.
   * @param dstRGB A GLenum specifying a multiplier for the red, green and blue
   * (RGB) destination blending factors.
   * @param srcAlpha A GLenum specifying a multiplier for the alpha source
   * blending factor.
   * @param dstAlpha A GLenum specifying a multiplier for the alpha destination
   * blending factor.
   */
  virtual void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
    = 0;
  virtual void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0,
                               GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask,
                               GLenum filter)
    = 0;

  /**
   * @brief Initializes and creates the buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param size A GLsizeiptr setting the size of the buffer object's data
   * store.
   * @param usage A GLenum specifying the usage pattern of the data store.
   */
  virtual void bufferData(GLenum target, GLsizeiptr size, GLenum usage) = 0;

  /**
   * @brief Initializes and creates the buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param data A Float32Array typed array that will be copied into the data
   * store.
   * @param usage A GLenum specifying the usage pattern of the data store.
   */
  virtual void bufferData(GLenum target, const Float32Array& data, GLenum usage) = 0;

  /**
   * @brief Initializes and creates the buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param data An Int32Array typed array that will be copied into the data
   * store.
   * @param usage A GLenum specifying the usage pattern of the data store.
   */
  virtual void bufferData(GLenum target, const Int32Array& data, GLenum usage) = 0;

  /**
   * @brief Initializes and creates the buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param data An Uint16Array typed array that will be copied into the data
   * store.
   * @param usage A GLenum specifying the usage pattern of the data store.
   */
  virtual void bufferData(GLenum target, const Uint16Array& data, GLenum usage) = 0;

  /**
   * @brief Initializes and creates the buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param data An Uint32Array typed array that will be copied into the data
   * store.
   * @param usage A GLenum specifying the usage pattern of the data store.
   */
  virtual void bufferData(GLenum target, const Uint32Array& data, GLenum usage) = 0;

  /**
   * @brief Updates a subset of a buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param offset A GLintptr specifying an offset in bytes where the data
   * replacement will start.
   * @param data A Uint8Array typed array that will be copied into the data
   * store.
   */
  virtual void bufferSubData(GLenum target, GLintptr offset, const Uint8Array& data) = 0;

  /**
   * @brief Updates a subset of a buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param offset A GLintptr specifying an offset in bytes where the data
   * replacement will start.
   * @param data A Float32Array typed array that will be copied into the data
   * store.
   */
  virtual void bufferSubData(GLenum target, GLintptr offset, const Float32Array& data) = 0;

  /**
   * @brief Updates a subset of a buffer object's data store.
   * @param target A GLenum specifying the binding point (target).
   * @param offset A GLintptr specifying an offset in bytes where the data
   * replacement will start.
   * @param data An Int32Array typed array that will be copied into the data
   * store.
   */
  virtual void bufferSubData(GLenum target, GLintptr offset, Int32Array& data) = 0;

  /**
   * @brief Binds a passed IGLVertexArrayObject object to the buffer.
   * @param vao A IGLVertexArrayObject (VAO) object to bind.
   */
  virtual void bindVertexArray(GL::IGLVertexArrayObject* vao) = 0;

  /**
   * @brief Returns the completeness status of the IGLFramebuffer object.
   * @param target A GLenum specifying the binding point (target).
   * @return A GLenum indicating the completeness status of the framebuffer or 0
   * if an error occurs.
   */
  virtual GLenum checkFramebufferStatus(GLenum target) = 0;

  /**
   * @brief Clears buffers to preset values.
   * @param mask A GLbitfield bitwise OR mask that indicates the buffers to be
   * cleared.
   */
  virtual void clear(GLbitfield mask) = 0;

  /**
   * @brief Clears buffers from the currently bound framebuffer.
   * @param buffer A GLenum specifying the buffer to clear.
   * @param drawbuffer A GLint specifying the draw buffer to clear.
   * @param values Specifying the values to clear to.
   * @param srcOffset optional source offset
   */
  virtual void clearBufferfv(GLenum buffer, GLint drawbuffer, const std::vector<GLfloat>& values,
                             GLint srcOffset = 0)
    = 0;

  /**
   * @brief Clears buffers from the currently bound framebuffer.
   * @param buffer A GLenum specifying the buffer to clear.
   * @param drawbuffer A GLint specifying the draw buffer to clear.
   * @param values Specifying the values to clear to.
   * @param srcOffset optional source offset
   */
  virtual void clearBufferiv(GLenum buffer, GLint drawbuffer, const std::vector<GLint>& values,
                             GLint srcOffset = 0)
    = 0;

  /**
   * @brief Clears buffers from the currently bound framebuffer.
   * @param buffer A GLenum specifying the buffer to clear.
   * @param drawbuffer A GLint specifying the draw buffer to clear.
   * @param values Specifying the values to clear to.
   * @param srcOffset An optional source offset
   */
  virtual void clearBufferuiv(GLenum buffer, GLint drawbuffer, const std::vector<GLuint>& values,
                              GLint srcOffset = 0)
    = 0;

  /**
   * @brief Clears buffers from the currently bound framebuffer.
   * @param buffer A GLenum specifying the buffer to clear.
   * @param drawbuffer A GLint specifying the draw buffer to clear.
   * @param depth A GLfloat specifying the value to clear a depth render buffer
   * to.
   * @param stencil A GLint specifying the value to clear the stencil render
   * buffer to.
   */
  virtual void clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) = 0;

  /**
   * @brief Specifies the color values used when clearing color buffers.
   * @param red A GLclampf specifying the red color value used when the color
   * buffers are cleared.
   * @param green A GLclampf specifying the green color value used when the
   * color buffers are cleared.
   * @param blue A GLclampf specifying the blue color value used when the color
   * buffers are cleared.
   * @param alpha A GLclampf specifying the alpha (transparency) value used when
   * the color buffers are cleared.
   */
  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;

  /**
   * @brief Specifies the clear value for the depth buffer.
   * @param depth A GLclampf specifying the depth value used when the depth
   * buffer is cleared.
   */
  virtual void clearDepth(GLclampf depth) = 0;

  /**
   * @brief Apecifies the clear value for the stencil buffer.
   * @param stencil A GLint specifying the index used when the stencil buffer
   * is cleared.
   */
  virtual void clearStencil(GLint stencil) = 0;

  /**
   * @brief Sets which color components to enable or to disable when drawing or
   * rendering to a IGLFramebuffer.
   * @param red A GLboolean specifying whether or not the red color component
   * can be written into the frame buffer.
   * @param green A GLboolean specifying whether or not the green color
   * component can be written into the frame buffer.
   * @param blue A GLboolean specifying whether or not the blue color component
   * can be written into the frame buffer.
   * @param alpha A GLboolean specifying whether or not the alpha (transparency)
   * component can be written into the frame buffer.
   */
  virtual void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = 0;

  /**
   * @brief Compiles a GLSL shader into binary data so that it can be used by a
   * IGLProgram.
   * @param shader A fragment or vertex IGLShader.
   */
  virtual void compileShader(IGLShader* shader) = 0;

  /**
   * @brief Specifies a two-dimensional texture image in a compressed format.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param internalformat A GLenum specifying the compressed image format.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param border A GLint specifying the width of the border. Must be 0.
   * @param pixels An Uint8Array that be used as a data store for the compressed
   * image data in memory.
   */
  virtual void compressedTexImage2D(GLenum target, GLint level, GLenum internalformat,
                                    GLsizei width, GLsizei height, GLint border,
                                    const Uint8Array& pixels)
    = 0;

  /**
   * @brief Specifies a two-dimensional sub-rectangle for a texture image in a
   * compressed format.
   * @param target A GLenum specifying the binding point (target) of the active
   * compressed texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param xoffset A GLint specifying the horizontal offset within the
   * compressed texture image.
   * @param yoffset A GLint specifying the vertical offset within the compressed
   * texture image.
   * @param width A GLsizei specifying the width of the compressed texture.
   * @param height A GLsizei specifying the height of the compressed texture.
   * @param format A GLenum specifying the compressed image format.
   * @param size A GLsizeiptr setting the size of the buffer object's data
   * store.
   */
  virtual void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                       GLsizei width, GLsizei height, GLenum format,
                                       GLsizeiptr size)
    = 0;

  /**
   * @brief Copies pixels from the current IGLFramebuffer into a 2D texture
   * image.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param internalformat A GLint specifying the color components in the
   * texture.
   * @param x A GLint specifying the x coordinate of the lower left corner where
   * to start copying.
   * @param y A GLint specifying the y coordinate of the lower left corner where
   * to start copying.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param border A GLint specifying the width of the border. Must be 0.
   */
  virtual void copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y,
                              GLsizei width, GLsizei height, GLint border)
    = 0;

  /**
   * @brief Copies pixels from the current IGLFramebuffer into an existing 2D
   * texture sub-image.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param xoffset A GLint specifying the horizontal offset within the texture
   * image.
   * @param yoffset A GLint specifying the vertical offset within the texture
   * image.
   * @param x A GLint specifying the x coordinate of the lower left corner where
   * to start copying.
   * @param y A GLint specifying the y coordinate of the lower left corner where
   * to start copying.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   */
  virtual void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x,
                                 GLint y, GLint width, GLint height)
    = 0;

  /**
   * @brief Creates and initializes a IGLBuffer storing data such as vertices or
   * colors.
   * @return An IGLBuffer storing data such as vertices or colors.
   */
  virtual std::shared_ptr<IGLBuffer> createBuffer() = 0;

  /**
   * @brief Creates and initializes a IGLFramebuffer object.
   * @return An IGLFramebuffer object.
   */
  virtual IGLFramebufferPtr createFramebuffer() = 0;

  /**
   * @brief Creates and initializes a IGLProgram object.
   * @return An IGLProgram object that is a combination of two compiled
   * IGLShaders consisting of a vertex shader and a fragment shader (both
   * written in GLSL). These are then linked into a usable program.
   */
  virtual IGLProgramPtr createProgram() = 0;

  /**
   * @brief Creates and initializes IGLQuery objects, which provide ways to
   * asynchronously query for information.
   * @return An IGLQuery object.
   */
  virtual std::unique_ptr<IGLQuery> createQuery() = 0;

  /**
   * @brief Creates and initializes a IGLRenderbuffer object.
   * @return A IGLRenderbuffer object that stores data such an image, or can be
   * source or target of an rendering operation.
   */
  virtual IGLRenderbufferPtr createRenderbuffer() = 0;

  /**
   * @brief Creates a IGLShader that can then be configured further using
   * IGLRenderingContext.shaderSource() and IGLRenderingContext.compileShader().
   * @param type Either VERTEX_SHADER or FRAGMENT_SHADER.
   * @return An IGLShader object.
   */
  virtual IGLShaderPtr createShader(GLenum type) = 0;

  /**
   * @brief Creates and initializes a IGLTexture object.
   * @return An IGLTexture object to which images can be bound to.
   */
  virtual IGLTexturePtr createTexture() = 0;

  /**
   * @brief Creates and initializes a IGLTransformFeedback object.
   * @return A IGLTransformFeedback object.
   */
  virtual IGLTransformFeedbackPtr createTransformFeedback() = 0;

  /**
   * @brief Creates and initializes a IGLVertexArrayObject object that
   * represents a vertex array object (VAO) pointing to vertex array data and
   * which provides names for different sets of vertex data.
   * @return An IGLVertexArrayObject representing a vertex array object (VAO)
   * which points to vertex array data.
   */
  virtual IGLVertexArrayObjectPtr createVertexArray() = 0;

  /**
   * @brief Specifies whether or not front- and/or back-facing polygons can be
   * culled.
   * @param mode A GLenum specifying whether front- or back-facing polygons are
   * candidates for culling.
   */
  virtual void cullFace(GLenum mode) = 0;

  /**
   * @brief Deletes a given IGLBuffer. This method has no effect if the buffer
   * has already been deleted.
   * @param buffer An IGLBuffer object to delete.
   */
  virtual void deleteBuffer(IGLBuffer* buffer) = 0;

  /**
   * @brief deletes a given IGLFramebuffer object. This method has no effect if
   * the frame buffer has already been deleted.
   * @param framebuffer A IGLFramebuffer object to delete.
   */
  virtual void deleteFramebuffer(IGLFramebuffer* framebuffer) = 0;

  /**
   * @brief Deletes a given IGLProgram object. This method has no effect if the
   * program has already been deleted.
   * @param program An IGLProgram object to delete.
   */
  virtual void deleteProgram(IGLProgram* program) = 0;

  /**
   * @brief Celetes a given IGLQuery object.
   * @param query An IGLQuery object to delete.
   */
  virtual void deleteQuery(IGLQuery* query) = 0;

  /**
   * @brief Deletes a given IGLRenderbuffer object. This method has no effect if
   * the render buffer has already been deleted.
   * @param renderbuffer An IGLRenderbuffer object to delete.
   */
  virtual void deleteRenderbuffer(IGLRenderbuffer* renderbuffer) = 0;

  /**
   * @brief Deletes a given IGLShader object. This method has no effect if the
   * shader has already been deleted.
   * @param shader An IGLShader object to delete.
   */
  virtual void deleteShader(IGLShader* shader) = 0;

  /**
   * @brief Deletes a given IGLTexture object. This method has no effect if the
   * texture has already been deleted.
   * @param texture A IGLTexture object to delete.
   */
  virtual void deleteTexture(IGLTexture* texture) = 0;

  /**
   * @brief Deletes a given IGLTransformFeedback object.
   * @param transformFeedback A IGLTransformFeedback object to delete.
   */
  virtual void deleteTransformFeedback(IGLTransformFeedback* transformFeedback) = 0;

  /**
   * @brief Deletes a given IGLVertexArrayObject object.
   * @param vao An IGLVertexArrayObject (VAO) object to delete.
   */
  virtual void deleteVertexArray(IGLVertexArrayObject* vao) = 0;

  /**
   * @brief Specifies a function that compares incoming pixel depth to the
   * current depth buffer value.
   * @param func A GLenum specifying the depth comparison function, which sets
   * the conditions under which the pixel will be drawn.
   */
  virtual void depthFunc(GLenum func) = 0;

  /**
   * @brief Sets whether writing into the depth buffer is enabled or disabled.
   * @param flag A GLboolean specifying whether or not writing into the depth
   * buffer is enabled.
   */
  virtual void depthMask(GLboolean flag) = 0;

  /**
   * @brief Specifies the depth range mapping from normalized device coordinates
   * to window or viewport coordinates.
   * @param zNear A GLclampf specifying the mapping of the near clipping plane
   * to window or viewport coordinates. Clamped to the range 0 to 1 and must be
   * less than or equal to zFar.
   * @param zFar A GLclampf specifying the mapping of the far clipping plane to
   * window or viewport coordinates. Clamped to the range 0 to 1.
   */
  virtual void depthRange(GLclampf zNear, GLclampf zFar) = 0;

  /**
   * @brief Detaches a previously attached IGLProgram from a IGLShader.
   * @param An IGLProgram to detach.
   * @param shader A fragment or vertex IGLShader.
   */
  virtual void detachShader(IGLProgram* program, IGLShader* shader) = 0;

  /**
   * @brief Disables specific GL capabilities for this context.
   * @param cap A GLenum specifying which GL capability to disable.
   */
  virtual void disable(GLenum cap) = 0;

  /**
   * @brief Turns the generic vertex attribute array off at a given index
   * position.
   * @param index A GLuint specifying the index of the vertex attribute to
   * disable.
   */
  virtual void disableVertexAttribArray(GLuint index) = 0;

  /**
   * @brief Renders primitives from array data.
   * @param mode A GLenum specifying the type primitive to render.
   * @param first A GLint specifying the starting index in the array of vector
   * points.
   * @param count A GLsizei specifying the number of indices to be rendered.
   */
  virtual void drawArrays(GLenum mode, GLint first, GLint count) = 0;

  /**
   * @brief Renders primitives from array data like the gl.drawArrays() method.
   * In addition, it can execute multiple instances of the range of elements.
   * @param mode A GLenum specifying the type primitive to render.
   * @param first A GLint specifying the starting index in the array of vector
   * points.
   * @param count A GLsizei specifying the number of indices to be rendered.
   * @param instanceCount A GLsizei specifying the number of instances of the
   * range of elements to execute.
   */
  virtual void drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
    = 0;

  /**
   * @brief Defines draw buffers to which fragment colors are written into. The
   * draw buffer settings are part of the state of the currently bound
   * framebuffer or the drawingbuffer if no framebuffer is bound.
   * @param buffers An Array of GLenum specifying the buffers into which
   * fragment colors will be written.
   */
  virtual void drawBuffers(const std::vector<GLenum>& buffers) = 0;

  /**
   * @brief Renders primitives from array data.
   * @param mode A GLenum specifying the type primitive to render.
   * @param count A GLsizei specifying the number of elements to be rendered.
   * @param type A GLenum specifying the type of the values in the element array
   * buffer.
   * @param offset A GLintptr specifying an offset in the element array buffer.
   */
  virtual void drawElements(GLenum mode, GLsizei count, GLenum type, GLintptr offset) = 0;

  /**
   * @brief Renders primitives from array data like the drawElements() method.
   * In addition, it can execute multiple instances of a set of elements.
   * @param mode A GLenum specifying the type primitive to render.
   * @param count A GLsizei specifying the number of elements to be rendered.
   * @param type A GLenum specifying the type of the values in the element array
   * buffer.
   * @param offset A GLintptr specifying an offset in the element array buffer.
   * Must be a valid multiple of the size of the given type.
   * @param instanceCount A GLsizei specifying the number of instances of the
   * set of elements to execute.
   */
  virtual void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLintptr offset,
                                     GLsizei instanceCount)
    = 0;

  /**
   * @brief Anables specific GL capabilities for this context.
   * @param cap A GLenum specifying which GL capability to enable.
   */
  virtual void enable(GLenum cap) = 0;

  /**
   * @brief Turns the generic vertex attribute array on at a given index
   * position.
   * @param index A GLuint specifying the index of the vertex attribute to
   * enable.
   */
  virtual void enableVertexAttribArray(GLuint index) = 0;

  /**
   * @brief Marks the end of a given query target.
   * @param target A GLenum specifying the target of the query. Possible values:
   *  - ANY_SAMPLES_PASSED: Specifies an occlusion query: these queries detect
   * whether an object is visible (whether the scoped drawing commands pass the
   * depth test and if so, how many samples pass).
   *  - ANY_SAMPLES_PASSED_CONSERVATIVE: Same as above above, but less accurate
   * and faster version.
   *  - TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN: Number of primitives that are
   * written to transform feedback buffers.
   */
  virtual void endQuery(GLenum target) = 0;

  /**
   * @brief ends a transform feedback operation.
   */
  virtual void endTransformFeedback() = 0;

  /**
   * @brief Blocks execution until all previously called commands are finished.
   */
  virtual void finish() = 0;

  /**
   * @brief Empties different buffer commands, causing all commands to be
   * executed as quickly as possible.
   */
  virtual void flush() = 0;

  /**
   * @brief Attaches an IGLRenderbuffer object to an IGLFramebuffer object.
   * @param target A GLenum specifying the binding point (target) for the
   * framebuffer.
   * @param attachment A GLenum specifying the attachment point for the render
   * buffer.
   * @param renderbuffertarget A GLenum specifying the binding point (target)
   * for the render buffer.
   * @param renderbuffer An IGLRenderbuffer object to attach.
   */
  virtual void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget,
                                       IGLRenderbuffer* renderbuffer)
    = 0;

  /**
   * @brief Attaches a texture to an IGLFramebuffer.
   * @param target A GLenum specifying the binding point (target).
   * @param attachment A GLenum specifying the attachment point for the texture.
   * @param textarget A GLenum specifying the texture target.
   * @param texture An IGLTexture object whose image to attach.
   * @param level A GLint specifying the mipmap level of the texture image to be
   * attached. Must be 0.
   */
  virtual void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget,
                                    IGLTexture* texture, GLint level)
    = 0;

  /**
   * @brief Addresses the inefficiency of sequential multiview rendering by
   * adding a means to render to multiple elements of a 2D texture array
   * simultaneously.  In multiview rendering, draw calls are instanced into each
   * corresponding element of the texture array.  The vertex program uses a new
   * gl_ViewID_OVR variable to compute per-view values, typically the vertex
   * position and view-dependent variables like reflection.
   * @see
   * https://www.khronos.org/registry/OpenGL/extensions/OVR/OVR_multiview.txt
   */
  virtual void framebufferTextureMultiviewOVR(GLenum target, GLenum attachment, IGLTexture* texture,
                                              GLint level, GLint baseViewIndex, GLint numViews)
    = 0;

  /**
   * @brief Specifies whether polygons are front- or back-facing by setting a
   * winding orientation.
   * @param mode Sets the winding orientation.
   */
  virtual void frontFace(GLenum mode) = 0;

  /**
   * @brief Generates a set of mipmaps for an IGLTexture object.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture whose mipmaps will be generated.
   */
  virtual void generateMipmap(GLenum target) = 0;

  /**
   * @brief Returns a list of IGLShader objects attached to a IGLProgram.
   * @param program A IGLProgram object to get attached shaders for.
   * @return An Array of IGLShader objects that are attached to the given
   * IGLProgram.
   */
  virtual std::vector<IGLShader*> getAttachedShaders(IGLProgram* program) = 0;

  /**
   * @brief Returns the location of an attribute variable in a given IGLProgram.
   * @param program A IGLProgram containing the attribute variable.
   * @param name A String specifying the name of the attribute variable whose
   * location to get.
   * @return A GLint number indicating the location of the variable name if
   * found. Returns -1 otherwise.
   */
  virtual GLint getAttribLocation(IGLProgram* program, const std::string& name) = 0;

  /**
   * @brief Enables a GL extension.
   * @param name A String for the name of the GL extension to enable.
   * @return A GL extension object, or null if name does not match (case-insensitive) to one of the
   * strings in WebGLRenderingContext.getSupportedExtensions.
   */
  virtual GL::any getExtension(const std::string& name) = 0;

  /**
   * @brief Returns true if the specified GL extension is supported.
   * @param extension A String specifying the extension to query.
   * @return A GLboolean indicating whether or not the GL extension is
   * supported.
   */
  virtual GLboolean hasExtension(const std::string& extension) = 0;

  virtual std::array<int, 3> getScissorBoxParameter() = 0; // GL::SCISSOR_BOX

  /**
   * @brief Returns a value for the passed parameter name.
   * @param pname A GLenum specifying which parameter value to return. See below
   * for possible values.
   * @return A GLint representing the value for the passed parameter name.
   */
  virtual GLint getParameteri(GLenum pname) = 0;

  /**
   * @brief Returns a value for the passed parameter name.
   * @param pname A GLenum specifying which parameter value to return. See below
   * for possible values.
   * @return A GLenum representing the value for the passed parameter name.
   */
  virtual GLfloat getParameterf(GLenum pname) = 0;

  /**
   * @brief Returns parameter information of a IGLQuery object.
   * @param query A IGLQuery object
   * @param pname A GLenum specifying which information to return, should be
   * QUERY_RESULT_AVAILABLE.
   * @return A GLboolean representing the information of a IGLQuery object.
   */
  virtual GLboolean getQueryParameterb(IGLQuery* query, GLenum pname) = 0;

  /**
   * @brief Returns parameter information of a IGLQuery object.
   * @param query A IGLQuery object
   * @param pname A GLenum specifying which information to return, should be
   * QUERY_RESULT.
   * @return A GLuint representing the information of a IGLQuery object.
   */
  virtual GLuint getQueryParameteri(IGLQuery* query, GLenum pname) = 0;

  virtual std::string getString(GLenum pname) = 0;

  /**
   * @brief Returns information about the given texture.
   * @param pname A Glenum specifying the information to query.
   * @return A GLint representing the requested texture information (as
   * specified with pname).
   */
  virtual GLint getTexParameteri(GLenum pname) = 0;

  /**
   * @brief Returns information about the given texture.
   * @param pname A Glenum specifying the information to query.
   * @return A GLfloat representing the requested texture information (as
   * specified with pname).
   */
  virtual GLfloat getTexParameterf(GLenum pname) = 0;

  /**
   * @brief Returns error information.
   * @return Error information.
   */
  virtual GLenum getError()                      = 0;
  virtual const char* getErrorString(GLenum err) = 0;

  /**
   * @brief Returns information about the given program.
   * @param program A IGLProgram to get parameter information from.
   * @param pname A Glenum specifying the information to query.
   * @return The requested program information (as specified with pname).
   */
  virtual GLint getProgramParameter(IGLProgram* program, GLenum pname) = 0;

  /**
   * @brief Returns the information log for the specified IGLProgram object.
   * It contains errors that occurred during failed linking or validation of
   * IGLProgram objects.
   * @param program An IGLProgram to query.
   * @return A String that contains diagnostic messages, warning messages, and
   * other information about the last linking or validation operation. When an
   * IGLProgram object is initially created, its information log will be a
   * string of length 0.
   */
  virtual std::string getProgramInfoLog(IGLProgram* program) = 0;

  /**
   * @brief Returns information about the renderbuffer.
   * @param target A Glenum specifying the target renderbuffer object.
   * @param pname A Glenum specifying the information to query.
   * @return Depends on the requested information (as specified with pname).
   * Either a GLint or a GLenum.
   */
  virtual GLint getRenderbufferParameter(GLenum target, GLenum pname) = 0;

  /**
   * @brief Returns the information log for the specified IGLShader object. It
   * contains warnings, debugging and compile information.
   * @param shader An IGLShader to query.
   * @return A String that contains diagnostic messages, warning messages, and
   * other information about the last compile operation. When an IGLShader
   * object is initially created, its information log will be a string of length
   * 0.
   */
  virtual std::string getShaderInfoLog(IGLShader* shader) = 0;

  /**
   * @brief Returns information about the given shader.
   * @param shader An IGLShader to get parameter information from.
   * @param pname A Glenum specifying the information to query.
   * @return Returns the requested shader information (as specified with pname).
   */
  virtual GLint getShaderParameter(IGLShader* shader, GLenum pname) = 0;

  /**
   * @brief Returns a new IGLShaderPrecisionFormat object describing the range
   * and precision for the specified shader numeric format.
   * @param shadertype Either a FRAGMENT_SHADER or a VERTEX_SHADER.
   * @param precisiontype A precision type value. Either LOW_FLOAT,
   * MEDIUM_FLOAT, HIGH_FLOAT, LOW_INT, MEDIUM_INT, or HIGH_INT.
   * @return A IGLShaderPrecisionFormat object or nullptr, if an error occurs.
   */
  virtual IGLShaderPrecisionFormat* getShaderPrecisionFormat(GLenum shadertype,
                                                             GLenum precisiontype)
    = 0;

  /**
   * @brief Returns the source code of an IGLShader as a String.
   * @param shader A IGLShader object to get the source code from.
   * @return A String containing the source code of the shader.
   */
  virtual std::string getShaderSource(IGLShader* shader) = 0;

  /**
   * @brief Retrieves the index of a uniform block within a IGLProgram.
   * @param program An IGLProgram containing the uniform block.
   * @param uniformBlockName A String specifying the name of the uniform block
   * to whose index to retrieve.
   * @return A GLuint indicating the uniform block index.
   */
  virtual GLuint getUniformBlockIndex(IGLProgram* program, const std::string& uniformBlockName) = 0;

  virtual std::unique_ptr<IGLUniformLocation> getUniformLocation(IGLProgram* program,
                                                                 const std::string& name)
    = 0;

  /**
   * @brief Specifies hints for certain behaviors. The interpretation of these
   * hints depend on the implementation.
   * @param target Sets which behavior to be controlled.
   * @param mode Sets the behavior.
   */
  virtual void hint(GLenum target, GLenum mode) = 0;

  /**
   * @brief Returns true if the passed IGLBuffer is valid and false otherwise.
   * @param buffer A IGLBuffer to check.
   * @return A GLboolean indicating whether or not the buffer is valid.
   */
  virtual GLboolean isBuffer(IGLBuffer* buffer) = 0;

  /**
   * @brief Tests whether a specific GL capability is enabled or not for this
   * context.
   * @param cap A GLenum specifying which GL capability to test.
   * @return A GLboolean indicating if the capability cap is enabled (true), or
   * not (false).
   */
  virtual GLboolean isEnabled(GLenum cap) = 0;

  /**
   * @brief Returns true if the passed IGLFramebuffer is valid and false
   * otherwise.
   * @param framebuffer A IGLFramebuffer to check.
   * @return A GLboolean indicating whether or not the frame buffer is valid.
   */
  virtual GLboolean isFramebuffer(IGLFramebuffer* framebuffer) = 0;

  /**
   * @brief Returns true if the passed IGLProgram is valid, false otherwise.
   * @param program A IGLProgram to check.
   * @return A GLboolean indicating whether or not the program is valid.
   */
  virtual GLboolean isProgram(IGLProgram* program) = 0;

  /**
   * @brief Returns true if the passed IGLRenderbuffer is valid and false
   * otherwise.
   * @param renderbuffer An IGLRenderbuffer to check.
   * @return A GLboolean indicating whether or not the renderbuffer is valid.
   */
  virtual GLboolean isRenderbuffer(IGLRenderbuffer* renderbuffer) = 0;

  /**
   * @brief Returns true if the passed IGLShader is valid, false otherwise.
   * @param shader An IGLShader to check.
   * @return A GLboolean indicating whether or not the shader is valid.
   */
  virtual GLboolean isShader(IGLShader* shader) = 0;

  /**
   * @brief Returns true if the passed IGLTexture is valid and false otherwise.
   * @param texture An IGLTexture to check.
   * @return A GLboolean indicating whether or not the texture is valid.
   */
  virtual GLboolean isTexture(IGLTexture* texture) = 0;

  /**
   * @brief Sets the line width of rasterized lines.
   * @param width A GLfloat specifying the width of rasterized lines.
   */
  virtual void lineWidth(GLfloat width) = 0;

  /**
   * @brief Links a given IGLProgram to the attached vertex and fragment
   * shaders.
   * @param program An IGLProgram to link.
   * @return Whether or not the linking succeeded.
   */
  virtual bool linkProgram(IGLProgram* program) = 0;

  /**
   * @brief Specifies the pixel storage modes.
   * @param pname A Glenum specifying which parameter to set. See below for
   * possible values.
   * @param param A GLint specifying a value to set the pname parameter to. See
   * below for possible values.
   */
  virtual void pixelStorei(GLenum pname, GLint param) = 0;

  /**
   * @brief Specifies the scale factors and units to calculate depth values.
   * @param factor A GLfloat which sets the scale factor for the variable depth
   * offset for each polygon.
   * @param units A GLfloat which sets the multiplier by which an
   * implementation-specific value is multiplied with to create a constant depth
   * offset.
   */
  virtual void polygonOffset(GLfloat factor, GLfloat units) = 0;

  /**
   * @brief Selects a color buffer as the source for pixels for subsequent calls
   * to copyTexImage2D, copyTexSubImage2D, copyTexSubImage3D or readPixels.
   * @param src A GLenum specifying a color buffer.
   */
  virtual void readBuffer(GLenum src) = 0;

  /**
   * @brief Reads a block of pixels from a specified rectangle of the current
   * color framebuffer into an Uint8Array object.
   * @param x A GLint specifying the first horizontal pixel that is read from
   * the lower left corner of a rectangular block of pixels.
   * @param y A GLint specifying the first vertical pixel that is read from the
   * lower left corner of a rectangular block of pixels.
   * @param width A GLsizei specifying the width of the rectangle.
   * @param height A GLsizei specifying the height of the rectangle.
   * @param format A GLenum specifying the format of the pixel data.
   * @param type A GLenum specifying the data type of the pixel data.
   * @param pixels An Float32Array object to read data into.
   */
  virtual void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                          GLenum type, Float32Array& pixels)
    = 0;

  /**
   * @brief Reads a block of pixels from a specified rectangle of the current
   * color framebuffer into an Uint8Array object.
   * @param x A GLint specifying the first horizontal pixel that is read from
   * the lower left corner of a rectangular block of pixels.
   * @param y A GLint specifying the first vertical pixel that is read from the
   * lower left corner of a rectangular block of pixels.
   * @param width A GLsizei specifying the width of the rectangle.
   * @param height A GLsizei specifying the height of the rectangle.
   * @param format A GLenum specifying the format of the pixel data.
   * @param type A GLenum specifying the data type of the pixel data.
   * @param pixels An Uint8Array object to read data into.
   */
  virtual void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                          GLenum type, Uint8Array& pixels)
    = 0;

  /**
   * @brief Creates and initializes a renderbuffer object's data store.
   * @param target A Glenum specifying the target renderbuffer object.
   * @param internalformat A Glenum specifying the internal format of the
   * renderbuffer.
   * @param width A GLsizei specifying the width of the renderbuffer in pixels.
   * @param height A GLsizei specifying the height of the renderbuffer in
   * pixels.
   */
  virtual void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width,
                                   GLsizei height)
    = 0;

  /**
   * @brief Returns creates and initializes a renderbuffer object's data store
   * and allows specifying a number of samples to be used.
   * @param target A GLenum specifying the target renderbuffer object.
   * @param samples A GLsizei specifying the number of samples to be used for
   * the renderbuffer storage.
   * @param internalFormat A GLenum specifying the internal format of the
   * renderbuffer.
   * @param width A GLsizei specifying the width of the renderbuffer in pixels.
   * @param height A GLsizei specifying the height of the renderbuffer in
   * pixels.
   */
  virtual void renderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalFormat,
                                              GLsizei width, GLsizei height)
    = 0;

  /**
   * @brief Specifies multi-sample coverage parameters for anti-aliasing
   * effects.
   * @param value A GLclampf which sets a single floating-point coverage value
   * clamped to the range [0,1].
   * @param invert A GLboolean which sets whether or not the coverage masks
   * should be inverted.
   */
  virtual void sampleCoverage(GLclampf value, GLboolean invert) = 0;

  /**
   * @brief Sets a scissor box, which limits the drawing to a specified
   * rectangle.
   * @param x A GLint specifying the horizontal coordinate for the lower left
   * corner of the box.
   * @param y A GLint specifying the vertical coordinate for the lower left
   * corner of the box.
   * @param width A non-negative GLsizei specifying the width of the scissor
   * box.
   * @param height A non-negative GLsizei specifying the height of the scissor
   * box.
   */
  virtual void scissor(GLint x, GLint y, GLsizei width, GLsizei height) = 0;

  /**
   * @brief Returns the source code of an IGLShader as a String.
   * @param shader An IGLShader object to get the source code from.
   * @param source A String containing the source code of the shader.
   */
  virtual void shaderSource(IGLShader* shader, const std::string& source) = 0;

  /**
   * @brief Sets the front and back function and reference value for stencil
   * testing.
   * @param func A GLenum specifying the test function.
   * @param ref A GLint specifying the reference value for the stencil test.
   * @param mask A GLuint specifying a bit-wise mask that is used to AND the
   * reference value and the stored stencil value when the test is done.
   */
  virtual void stencilFunc(GLenum func, GLint ref, GLuint mask) = 0;

  /**
   * @brief Sets the front and/or back function and reference value for stencil
   * testing. Stencilling enables and disables drawing on a per-pixel basis. It
   * is typically used in multipass rendering to achieve special effects.
   * @param face A GLenum specifying whether the front and/or back stencil state
   * is updated.
   * @param func A GLenum specifying the test function.
   * @param ref A GLint specifying the reference value for the stencil test.
   * This value is clamped to the range 0 to 2^(n-1) where n is the number of
   * bitplanes in the stencil buffer.
   * @param mask A GLuint specifying a bit-wise mask that is used to AND the
   * reference value and the stored stencil value when the test is done.
   */
  virtual void stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) = 0;

  /**
   * @brief Controls enabling and disabling of both the front and back writing
   * of individual bits in the stencil planes.
   * @param mask A GLuint specifying a bit mask to enable or disable writing of
   * individual bits in the stencil planes.
   */
  virtual void stencilMask(GLuint mask) = 0;

  /**
   * @brief Controls enabling and disabling of front and/or back writing of
   * individual bits in the stencil planes.
   * @param face A GLenum specifying whether the front and/or back stencil
   * writemask is updated.
   * @param mask A GLuint specifying a bit mask to enable or disable writing of
   * individual bits in the stencil planes.
   */
  virtual void stencilMaskSeparate(GLenum face, GLuint mask) = 0;

  /**
   * @brief Sets both the front and back-facing stencil test actions.
   * @param fail A GLenum specifying the function to use when the stencil test
   * fails.
   * @param zfail A GLenum specifying the function to use when the stencil test
   * passes, but the depth test fails.
   * @param zpass A GLenum specifying the function to use when both the stencil
   * test and the depth test pass, or when the stencil test passes and there is
   * no depth buffer or depth testing is disabled.
   */
  virtual void stencilOp(GLenum fail, GLenum zfail, GLenum zpass) = 0;

  /**
   * @brief Sets the front and/or back-facing stencil test actions.
   * @param face A GLenum specifying whether the front and/or back stencil state
   * is updated.
   * @param fail A GLenum specifying the function to use when the stencil test
   * fails.
   * @param zfail A GLenum specifying the function to use when the stencil test
   * passes, but the depth test fails.
   * @param zpass A GLenum specifying the function to use when both the stencil
   * test and the depth test pass, or when the stencil test passes and there is
   * no depth buffer or depth testing is disabled.
   */
  virtual void stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass) = 0;

  /**
   * @brief Specifies a two-dimensional texture image.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param internalformat A GLint specifying the color components in the
   * texture.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param border A GLint specifying the width of the border. Must be 0.
   * @param format A GLenum specifying the format of the texel data.
   * @param type A GLenum specifying the data type of the texel data.
   * @param pixels An Uint8Array pixel source for the texture (can be nullptr)
   */
  virtual void
  texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
             GLint border, GLenum format, GLenum type,
             const Uint8Array* const pixels) // NOLINT ([readability-avoid-const-params-in-decls])
    = 0;

  /**
   * @brief Specifies a three-dimensional texture image.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param internalformat A GLint specifying the color components in the
   * texture.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param depth A GLsizei specifying the depth of the texture.
   * @param border A GLint specifying the width of the border. Must be 0.
   * @param format A GLenum specifying the format of the texel data.
   * @param type A GLenum specifying the data type of the texel data.
   * @param pixels An Uint8Array pixel source for the texture.
   */
  virtual void texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width,
                          GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type,
                          const Uint8Array& pixels)
    = 0;

  /**
   * @brief Sets texture parameters.
   * @param target A GLenum specifying the binding point (target).
   * @param pname A Glenum specifying the texture parameter to set.
   * @param param A GLfloat or GLint specifying the value for the specified
   * parameter pname.
   */
  virtual void texParameterf(GLenum target, GLenum pname, GLfloat param) = 0;

  /**
   * @brief Set texture parameters.
   * @param target A GLenum specifying the binding point (target).
   * @param pname A Glenum specifying the texture parameter to set.
   * @param param A GLfloat or GLint specifying the value for the specified
   * parameter pname.
   */
  virtual void texParameteri(GLenum target, GLenum pname, GLint param) = 0;

  /**
   * @brief Specifies all levels of a three-dimensional texture or
   * two-dimensional array texture.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture. Possible values:
   *    * gl.TEXTURE_3D: A three-dimensional texture.
   *    * gl.TEXTURE_2D_ARRAY: A two-dimensional array texture.
   * @param levels A GLint specifying the number of texture levels.
   * @param internalformat A GLenum specifying the texture store format.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param depth A GLsizei specifying the depth of the texture.
   */
  virtual void texStorage3D(GLenum target, GLint levels, GLenum internalformat, GLsizei width,
                            GLsizei height, GLsizei depth)
    = 0;

  /**
   * @brief Specifies a sub-rectangle of the current texture.
   * @param target A GLenum specifying the binding point (target) of the active
   * texture.
   * @param level A GLint specifying the level of detail. Level 0 is the base
   * image level and level n is the nth mipmap reduction level.
   * @param xoffset A GLint specifying the horizontal offset within the texture
   * image.
   * @param yoffset A GLint specifying the vertical offset within the texture
   * image.
   * @param width A GLsizei specifying the width of the texture.
   * @param height A GLsizei specifying the height of the texture.
   * @param format A GLenum specifying the format of the texel data.
   * @param type A GLenum specifying the data type of the texel data.
   * @param pixels A pixel source for the texture.
   */
  virtual void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                             GLsizei width, GLsizei height, GLenum format, GLenum type, any pixels)
    = 0;

  /**
   * @brief Specifies values to record in WebGLTransformFeedback buffers.
   * @param program A IGLProgram.
   * @param varyings An Array of Strings specifying the the names of the varying
   * variables to use.
   * @param bufferMode A GLenum specifying the mode to use when capturing the
   * varying variables.
   */
  virtual void transformFeedbackVaryings(IGLProgram* program,
                                         const std::vector<std::string>& varyings,
                                         GLenum bufferMode)
    = 0;

  /**
   * @brief Specifies the value of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLfloat to be used for the uniform variable.
   */
  virtual void uniform1f(IGLUniformLocation* location, GLfloat v0) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param array A Float32Array to be used for the uniform variable.
   */
  virtual void uniform1fv(GL::IGLUniformLocation* location, const Float32Array& array) = 0;

  /**
   * @brief Specifies the value of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLint to be used for the uniform variable.
   */
  virtual void uniform1i(IGLUniformLocation* location, GLint v0) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v An Int32Array to be used for the uniform variable.
   */
  virtual void uniform1iv(IGLUniformLocation* location, const Int32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLfloat to be used for the uniform variable.
   * @param v1 A GLfloat to be used for the uniform variable.
   */
  virtual void uniform2f(IGLUniformLocation* location, GLfloat v0, GLfloat v1) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v A Float32Array to be used for the uniform variable.
   */
  virtual void uniform2fv(IGLUniformLocation* location, const Float32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLint to be used for the uniform variable.
   * @param v1 A GLint to be used for the uniform variable.
   */
  virtual void uniform2i(IGLUniformLocation* location, GLint v0, GLint v1) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v An Int32Array to be used for the uniform variable.
   */
  virtual void uniform2iv(IGLUniformLocation* location, const Int32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLfloat to be used for the uniform variable.
   * @param v1 A GLfloat to be used for the uniform variable.
   * @param v2 A GLfloat to be used for the uniform variable.
   */
  virtual void uniform3f(IGLUniformLocation* location, GLfloat v0, GLfloat v1, GLfloat v2) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v A Float32Array to be used for the uniform variable.
   */
  virtual void uniform3fv(IGLUniformLocation* location, const Float32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLint to be used for the uniform variable.
   * @param v1 A GLint to be used for the uniform variable.
   * @param v2 A GLint to be used for the uniform variable.
   */
  virtual void uniform3i(IGLUniformLocation* location, GLint v0, GLint v1, GLint v2) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v An Int32Array to be used for the uniform variable.
   */
  virtual void uniform3iv(IGLUniformLocation* location, const Int32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLfloat to be used for the uniform variable.
   * @param v1 A GLfloat to be used for the uniform variable.
   * @param v2 A GLfloat to be used for the uniform variable.
   * @param v3 A GLfloat to be used for the uniform variable.
   */
  virtual void uniform4f(IGLUniformLocation* location, GLfloat v0, GLfloat v1, GLfloat v2,
                         GLfloat v3)
    = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v A Float32Array to be used for the uniform variable.
   */
  virtual void uniform4fv(IGLUniformLocation* location, const Float32Array& v) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param v0 A GLint to be used for the uniform variable.
   * @param v1 A GLint to be used for the uniform variable.
   * @param v2 A GLint to be used for the uniform variable.
   * @param v3 A GLint to be used for the uniform variable.
   */
  virtual void uniform4i(IGLUniformLocation* location, GLint v0, GLint v1, GLint v2, GLint v3) = 0;

  /**
   * @brief Specifies the values of the uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param An Int32Array to be used for the uniform variable.
   */
  virtual void uniform4iv(IGLUniformLocation* location, const Int32Array& v) = 0;

  /**
   * @brief Assigns binding points for active uniform blocks.
   * @param program An IGLProgram containing the active uniform block whose
   * binding to assign.
   * @param uniformBlockIndex A GLuint specifying the index of the active
   * uniform block within the program.
   * @param uniformBlockBinding A GLuint specifying the binding point to which
   * to bind the uniform block.
   */
  virtual void uniformBlockBinding(IGLProgram* program, GLuint uniformBlockIndex,
                                   GLuint uniformBlockBinding)
    = 0;

  /**
   * @brief Specifies the matrix values for uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param transpose A GLboolean specifying whether to transpose the matrix.
   * Must be false.
   * @param value A Float32Array of float values.
   */
  virtual void uniformMatrix2fv(IGLUniformLocation* location, GLboolean transpose,
                                const Float32Array& value)
    = 0;

  /**
   * @brief Specifies the matrix values for uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param transpose A GLboolean specifying whether to transpose the matrix.
   * Must be false.
   * @param value A Float32Array of float values.
   */
  virtual void uniformMatrix3fv(IGLUniformLocation* location, GLboolean transpose,
                                const Float32Array& value)
    = 0;

  /**
   * @brief Specifies the matrix values for uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param transpose A GLboolean specifying whether to transpose the matrix.
   * Must be false.
   * @param value A Float32Array of float values.
   */
  virtual void uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                                const Float32Array& value)
    = 0;

  /**
   * @brief Specifies the matrix values for uniform variable.
   * @param location An IGLUniformLocation object containing the location of the
   * uniform attribute to modify.
   * @param transpose A GLboolean specifying whether to transpose the matrix.
   * Must be false.
   * @param value A Float32Array of float values.
   */
  virtual void uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                                const std::array<float, 16>& value)
    = 0;

  /**
   * @brief Sets the specified IGLProgram as part the current rendering state.
   * @param An IGLProgram to use.
   */
  virtual void useProgram(IGLProgram* program) = 0;

  /**
   * @brief Validates an IGLProgram. It checks if it is successfully linked and
   * if it can be used in the current GL state.
   * @param program An IGLProgram to validate.
   */
  virtual void validateProgram(IGLProgram* program) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param v0 A floating point Number for the vertex attribute value.
   */
  virtual void vertexAttrib1f(GLuint index, GLfloat v0)           = 0;
  virtual void vertexAttrib1fv(GLuint indx, Float32Array& values) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param v0 A floating point Number for the vertex attribute value.
   * @param v1 A floating point Number for the vertex attribute value.
   */
  virtual void vertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param values A Float32Array for floating point vector vertex attribute
   * values.
   */
  virtual void vertexAttrib2fv(GLuint index, Float32Array& values) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param v0 A floating point Number for the vertex attribute value.
   * @param v1 A floating point Number for the vertex attribute value.
   * @param v2 A floating point Number for the vertex attribute value.
   */
  virtual void vertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param values A Float32Array for floating point vector vertex attribute
   * values.
   */
  virtual void vertexAttrib3fv(GLuint index, Float32Array& values) = 0;

  /**
   * @brief Specifies values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param v0 A floating point Number for the vertex attribute value.
   * @param v1 A floating point Number for the vertex attribute value.
   * @param v2 A floating point Number for the vertex attribute value.
   * @param v3 A floating point Number for the vertex attribute value.
   */
  virtual void vertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = 0;

  /**
   * @brief Specifies the values for generic vertex attributes.
   * @param index A GLuint specifying the position of the vertex attribute to be
   * modified.
   * @param values A Float32Array for floating point vector vertex attribute
   * values.
   */
  virtual void vertexAttrib4fv(GLuint index, Float32Array& values) = 0;

  /**
   * @brief Modifies the rate at which generic vertex attributes advance when
   * rendering multiple instances of primitives with drawArraysInstanced() and
   * drawElementsInstanced().
   * @param index A GLuint specifying the index of the generic vertex
   * attributes.
   * @param divisor A GLuint specifying the number of instances that will pass
   * between updates of the generic attribute.
   */
  virtual void vertexAttribDivisor(GLuint index, GLuint divisor) = 0;

  /**
   * @brief Specifies integer data formats and locations of vertex attributes in
   * a vertex attributes array.
   * @param index A GLuint specifying the index of the vertex attribute that is
   * to be modified.
   * @param size A GLint specifying the number of components per vertex
   * attribute. Must be 1, 2, 3, or 4.
   * @param type A GLenum specifying the data type of each component in the
   * array.
   * @param normalized A GLboolean specifying if the data is normalized
   * @param stride A GLsizei specifying the offset in bytes between the
   * beginning of consecutive vertex attributes.
   * @param offset A GLintptr specifying an offset in bytes of the first
   * component in the vertex attribute array.
   */
  virtual void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
                                   GLint stride, GLintptr offset)
    = 0;

  /**
   * @brief Sets the viewport, which specifies the affine transformation of x
   * and y from normalized device coordinates to window coordinates.
   * @param x A GLint specifying the horizontal coordinate for the lower left
   * corner of the viewport origin.
   * @param y A GLint specifying the vertical coordinate for the lower left
   * corner of the viewport origin.
   * @param width A non-negative GLsizei specifying the width of the viewport.
   * @param height A non-negative GLsizei specifying the height of the viewport.
   */
  virtual void viewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;

}; // end of class GLRenderContext

class BABYLON_SHARED_EXPORT IGLVertexArrayObject {

public:
  IGLVertexArrayObject(GLuint iValue) : value{iValue}
  {
  }

public:
  GLuint value;

}; // end of class IGLVertexArrayObject

} // end of namespace GL
} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H
