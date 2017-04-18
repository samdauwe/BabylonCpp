#ifndef BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H
#define BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GL {

using GLenum     = unsigned int;
using GLboolean  = bool;
using GLbitfield = unsigned long;
using GLbyte     = char;
using GLshort    = short;
using GLint      = int;
using GLsizei    = long;
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
  /* BlendEquationSeparate */
  FUNC_ADD       = 0x8006,
  BLEND_EQUATION = 0x8009,
  /* same as BLEND_EQUATION*/
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
  CURRENT_VERTEX_ATTRIB        = 0x8626,
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
  NO_ERROR          = 0x0000,
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
  /* SCISSOR_TEST*/
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
  STENCIL_INDEX                                = 0x1901,
  STENCIL_INDEX8                               = 0x8D48,
  DEPTH_STENCIL                                = 0x84F9,
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
  FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE           = 0x8CD0,
  FRAMEBUFFER_ATTACHMENT_OBJECT_NAME           = 0x8CD1,
  FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL         = 0x8CD2,
  FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3,
  COLOR_ATTACHMENT0                            = 0x8CE0,
  DEPTH_ATTACHMENT                             = 0x8D00,
  STENCIL_ATTACHMENT                           = 0x8D20,
  DEPTH_STENCIL_ATTACHMENT                     = 0x821A,
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
  MAX_RENDERBUFFER_SIZE                        = 0x84E8,
  INVALID_FRAMEBUFFER_OPERATION                = 0x0506,
  /* WebGL-specific enums */
  UNPACK_FLIP_Y_WEBGL                = 0x9240,
  UNPACK_PREMULTIPLY_ALPHA_WEBGL     = 0x9241,
  CONTEXT_LOST_WEBGL                 = 0x9242,
  UNPACK_COLORSPACE_CONVERSION_WEBGL = 0x9243,
  BROWSER_DEFAULT_WEBGL              = 0x9244,
  // IGL_EXT_texture_filter_anisotropic
  TEXTURE_MAX_ANISOTROPY_EXT     = 0x84FE,
  MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF
}; // end of enum GLEnums

class BABYLON_SHARED_EXPORT IGLBuffer {

public:
  IGLBuffer(GLuint _value)
      : references{1}, is32Bits{true}, capacity{1}, value{_value}
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

class BABYLON_SHARED_EXPORT IGLProgram {

public:
  IGLProgram(GLuint _value) : value{_value}
  {
  }

public:
  GLuint value;

}; // end of class IGLProgram

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
      , type{0}
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
  unsigned int type;
  unsigned int _cachedWrapU;
  unsigned int _cachedWrapV;
  unsigned int _cachedCoordinatesMode;
  bool _generateDepthBuffer;
  bool _generateStencilBuffer;
  std::string url;
  std::unique_ptr<IGLFramebuffer> _framebuffer;
  std::unique_ptr<IGLFramebuffer> _MSAAFramebuffer;
  std::unique_ptr<IGLRenderbuffer> _depthBuffer;
  std::unique_ptr<IGLRenderbuffer> _depthStencilBuffer;
  std::unique_ptr<IGLRenderbuffer> _MSAARenderBuffer;
  std::vector<std::function<void()>> onLoadedCallbacks;
}; // end of class IGLTexture

class BABYLON_SHARED_EXPORT IGLUniformLocation {

public:
  IGLUniformLocation(GLint _value) : value{_value}
  {
  }

public:
  GLint value;

}; // end of class IGLUniformLocation

/**
* @brief
*/
class BABYLON_SHARED_EXPORT IGLRenderingContext {

public:
  virtual bool initialize()     = 0;
  virtual void backupGLState()  = 0;
  virtual void restoreGLState() = 0;
  // virtual ICanvas* getCanvas() = 0;
  // virtual GLsizei getDrawingBufferWidth() = 0;
  // virtual GLsizei getDrawingBufferHeight() = 0;
  // virtual GLContextAttributes getContextAttributes() = 0;
  // virtual bool isContextLost() = 0;
  // virtual std::vector<std::string> getSupportedExtensions() = 0;
  // virtual any getExtension(const std::string& name) = 0;
  virtual GLenum operator[](const std::string& name) = 0;
  virtual void activeTexture(GLenum texture)         = 0;
  virtual void attachShader(const std::unique_ptr<IGLProgram>& program,
                            const std::unique_ptr<IGLShader>& shader)
    = 0;
  virtual void bindAttribLocation(IGLProgram* program, GLuint index,
                                  const std::string& name)
    = 0;
  virtual void bindBuffer(GLenum target, IGLBuffer* buffer)                = 0;
  virtual void bindFramebuffer(GLenum target, IGLFramebuffer* framebuffer) = 0;
  virtual void
  bindRenderbuffer(GLenum target,
                   const std::unique_ptr<IGLRenderbuffer>& renderbuffer)
    = 0;
  virtual void bindTexture(GLenum target, IGLTexture* texture) = 0;
  virtual void blendColor(GLclampf red, GLclampf green, GLclampf blue,
                          GLclampf alpha)
    = 0;
  virtual void blendEquation(GLenum mode) = 0;
  virtual void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) = 0;
  virtual void blendFunc(GLenum sfactor, GLenum dfactor)               = 0;
  virtual void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                 GLenum dstAlpha)
    = 0;
  virtual void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1,
                               GLint srcY1, GLint dstX0, GLint dstY0,
                               GLint dstX1, GLint dstY1, GLbitfield mask,
                               GLenum filter)
    = 0;
  virtual void bufferData(GLenum target, GLsizeiptr size, GLenum usage) = 0;
  virtual void bufferData(GLenum target, const Float32Array& data, GLenum usage)
    = 0;
  virtual void bufferData(GLenum target, const Int32Array& data, GLenum usage)
    = 0;
  virtual void bufferData(GLenum target, const Uint16Array& data, GLenum usage)
    = 0;
  virtual void bufferData(GLenum target, const Uint32Array& data, GLenum usage)
    = 0;
  virtual void bufferSubData(GLenum target, GLintptr offset,
                             const Float32Array& data)
    = 0;
  virtual void bufferSubData(GLenum target, GLintptr offset, Int32Array& data)
    = 0;
  virtual void bindVertexArray(GL::IGLVertexArrayObject* vao) = 0;
  virtual GLenum checkFramebufferStatus(GLenum target)        = 0;
  virtual void clear(GLuint mask)                             = 0;
  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue,
                          GLclampf alpha)
    = 0;
  virtual void clearDepth(GLclampf depth)  = 0;
  virtual void clearStencil(GLint stencil) = 0;
  virtual void colorMask(GLboolean red, GLboolean green, GLboolean blue,
                         GLboolean alpha)
    = 0;
  virtual void compileShader(const std::unique_ptr<IGLShader>& shader) = 0;
  virtual void compressedTexImage2D(GLenum target, GLint level,
                                    GLenum internalformat, GLint width,
                                    GLint height, GLint border,
                                    const Uint8Array& pixels)
    = 0;
  virtual void compressedTexSubImage2D(GLenum target, GLint level,
                                       GLint xoffset, GLint yoffset,
                                       GLint width, GLint height, GLenum format,
                                       GLsizeiptr size)
    = 0;
  virtual void copyTexImage2D(GLenum target, GLint level, GLenum internalformat,
                              GLint x, GLint y, GLint width, GLint height,
                              GLint border)
    = 0;
  virtual void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset,
                                 GLint yoffset, GLint x, GLint y, GLint width,
                                 GLint height)
    = 0;
  virtual std::unique_ptr<IGLBuffer> createBuffer()                 = 0;
  virtual std::unique_ptr<IGLFramebuffer> createFramebuffer()       = 0;
  virtual std::unique_ptr<IGLProgram> createProgram()               = 0;
  virtual std::unique_ptr<IGLRenderbuffer> createRenderbuffer()     = 0;
  virtual std::unique_ptr<IGLShader> createShader(GLenum type)      = 0;
  virtual std::unique_ptr<IGLTexture> createTexture()               = 0;
  virtual std::unique_ptr<IGLVertexArrayObject> createVertexArray() = 0;
  virtual void cullFace(GLenum mode)                                = 0;
  virtual void deleteBuffer(IGLBuffer* buffer)                      = 0;
  virtual void
  deleteFramebuffer(const std::unique_ptr<IGLFramebuffer>& framebuffer)
    = 0;
  virtual void deleteProgram(IGLProgram* program) = 0;
  virtual void
  deleteRenderbuffer(const std::unique_ptr<IGLRenderbuffer>& renderbuffer)
    = 0;
  virtual void deleteShader(const std::unique_ptr<IGLShader>& shader) = 0;
  virtual void deleteTexture(IGLTexture* texture)                     = 0;
  virtual void deleteVertexArray(IGLVertexArrayObject* vao)           = 0;
  virtual void depthFunc(GLenum func)                                 = 0;
  virtual void depthMask(GLboolean flag)                              = 0;
  virtual void depthRange(GLclampf zNear, GLclampf zFar)            = 0;
  virtual void detachShader(IGLProgram* program, IGLShader* shader) = 0;
  virtual void disable(GLenum cap)                    = 0;
  virtual void disableVertexAttribArray(GLuint index) = 0;
  virtual void drawArrays(GLenum mode, GLint first, GLint count) = 0;
  virtual void drawArraysInstanced(GLenum mode, GLint first, GLsizei count,
                                   GLsizei instanceCount)
    = 0;
  virtual void drawElements(GLenum mode, GLint count, GLenum type,
                            GLintptr offset)
    = 0;
  virtual void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type,
                                     GLintptr offset, GLsizei instanceCount)
    = 0;
  virtual void enable(GLenum cap)                    = 0;
  virtual void enableVertexAttribArray(GLuint index) = 0;
  virtual void finish()                              = 0;
  virtual void flush()                               = 0;
  virtual void
  framebufferRenderbuffer(GLenum target, GLenum attachment,
                          GLenum renderbuffertarget,
                          const std::unique_ptr<IGLRenderbuffer>& renderbuffer)
    = 0;
  virtual void framebufferTexture2D(GLenum target, GLenum attachment,
                                    GLenum textarget, IGLTexture* texture,
                                    GLint level)
    = 0;
  virtual void frontFace(GLenum mode)                                     = 0;
  virtual void generateMipmap(GLenum target)                              = 0;
  virtual std::vector<IGLShader*> getAttachedShaders(IGLProgram* program) = 0;
  virtual GLint getAttribLocation(IGLProgram* program, const std::string& name)
    = 0;
  virtual bool hasExtension(const std::string& extension) = 0;
  virtual std::array<int, 3> getScissorBoxParameter() = 0; // GL::SCISSOR_BOX
  virtual GLint getParameteri(GLenum pname)      = 0;
  virtual GLfloat getParameterf(GLenum pname)    = 0;
  virtual std::string getString(GLenum pname)    = 0;
  virtual GLint getTexParameteri(GLenum pname)   = 0;
  virtual GLfloat getTexParameterf(GLenum pname) = 0;
  virtual GLenum getError()                      = 0;
  virtual const char* getErrorString(GLenum err) = 0;
  virtual GLint getProgramParameter(IGLProgram* program, GLenum pname) = 0;
  virtual std::string
  getProgramInfoLog(const std::unique_ptr<IGLProgram>& program)
    = 0;
  virtual any getRenderbufferParameter(GLenum target, GLenum pname) = 0;
  virtual GLint getShaderParameter(const std::unique_ptr<IGLShader>& shader,
                                   GLenum pname)
    = 0;
  virtual IGLShaderPrecisionFormat*
  getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype)
    = 0;
  virtual std::string getShaderInfoLog(const std::unique_ptr<IGLShader>& shader)
    = 0;
  virtual std::string getShaderSource(IGLShader* shader) = 0;
  virtual std::unique_ptr<IGLUniformLocation>
  getUniformLocation(IGLProgram* program, const std::string& name) = 0;
  virtual void hint(GLenum target, GLenum mode)                    = 0;
  virtual GLboolean isBuffer(IGLBuffer* buffer)                           = 0;
  virtual GLboolean isEnabled(GLenum cap)                                 = 0;
  virtual GLboolean isFramebuffer(IGLFramebuffer* framebuffer)            = 0;
  virtual GLboolean isProgram(const std::unique_ptr<IGLProgram>& program) = 0;
  virtual GLboolean isRenderbuffer(IGLRenderbuffer* renderbuffer)         = 0;
  virtual GLboolean isShader(IGLShader* shader)                           = 0;
  virtual GLboolean isTexture(IGLTexture* texture)                        = 0;
  virtual void lineWidth(GLfloat width)                                   = 0;
  virtual bool linkProgram(const std::unique_ptr<IGLProgram>& program)    = 0;
  virtual void pixelStorei(GLenum pname, GLint param)       = 0;
  virtual void polygonOffset(GLfloat factor, GLfloat units) = 0;
  virtual void readPixels(GLint x, GLint y, GLint width, GLint height,
                          GLenum format, GLenum type, Uint8Array& pixels)
    = 0;

  virtual void renderbufferStorage(GLenum target, GLenum internalformat,
                                   GLint width, GLint height)
    = 0;
  virtual void renderbufferStorageMultisample(GLenum target, GLsizei samples,
                                              GLenum internalFormat,
                                              GLsizei width, GLsizei height)
    = 0;
  virtual void sampleCoverage(GLclampf value, GLboolean invert) = 0;
  virtual void scissor(GLint x, GLint y, GLint width, GLint height) = 0;
  virtual void shaderSource(const std::unique_ptr<IGLShader>& shader,
                            const std::string& source)
    = 0;
  virtual void stencilFunc(GLenum func, GLint ref, GLuint mask) = 0;
  virtual void stencilFuncSeparate(GLenum face, GLenum func, GLint ref,
                                   GLuint mask)
    = 0;
  virtual void stencilMask(GLuint mask) = 0;
  virtual void stencilMaskSeparate(GLenum face, GLuint mask) = 0;
  virtual void stencilOp(GLenum fail, GLenum zfail, GLenum zpass) = 0;
  virtual void stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail,
                                 GLenum zpass)
    = 0;
  virtual void texImage2D(GLenum target, GLint level, GLint internalformat,
                          GLint width, GLint height, GLint border,
                          GLenum format, GLenum type, const Uint8Array& pixels)
    = 0;
  virtual void texParameterf(GLenum target, GLenum pname, GLfloat param) = 0;
  virtual void texParameteri(GLenum target, GLenum pname, GLint param)   = 0;
  virtual void texSubImage2D(GLenum target, GLint level, GLint xoffset,
                             GLint yoffset, GLint width, GLint height,
                             GLenum format, GLenum type, any pixels)
    = 0;
  virtual void uniform1f(IGLUniformLocation* location, GLfloat x) = 0;
  virtual void uniform1fv(GL::IGLUniformLocation* uniform,
                          const Float32Array& array)
    = 0;
  virtual void uniform1i(IGLUniformLocation* location, GLint x) = 0;
  virtual void uniform1iv(IGLUniformLocation* location, const Int32Array& v)
    = 0;
  virtual void uniform2f(IGLUniformLocation* location, GLfloat x, GLfloat y)
    = 0;
  virtual void uniform2fv(IGLUniformLocation* location, const Float32Array& v)
    = 0;
  virtual void uniform2i(IGLUniformLocation* location, GLint x, GLint y) = 0;
  virtual void uniform2iv(IGLUniformLocation* location, const Int32Array& v)
    = 0;
  virtual void uniform3f(IGLUniformLocation* location, GLfloat x, GLfloat y,
                         GLfloat z)
    = 0;
  virtual void uniform3fv(IGLUniformLocation* location, const Float32Array& v)
    = 0;
  virtual void uniform3i(IGLUniformLocation* location, GLint x, GLint y,
                         GLint z)
    = 0;
  virtual void uniform3iv(IGLUniformLocation* location, const Int32Array& v)
    = 0;
  virtual void uniform4f(IGLUniformLocation* location, GLfloat x, GLfloat y,
                         GLfloat z, GLfloat w)
    = 0;
  virtual void uniform4fv(IGLUniformLocation* location, const Float32Array& v)
    = 0;
  virtual void uniform4i(IGLUniformLocation* location, GLint x, GLint y,
                         GLint z, GLint w)
    = 0;
  virtual void uniform4iv(IGLUniformLocation* location, const Int32Array& v)
    = 0;
  virtual void uniformMatrix2fv(IGLUniformLocation* location,
                                GLboolean transpose, const Float32Array& value)
    = 0;
  virtual void uniformMatrix3fv(IGLUniformLocation* location,
                                GLboolean transpose, const Float32Array& value)
    = 0;
  virtual void uniformMatrix4fv(IGLUniformLocation* location,
                                GLboolean transpose, const Float32Array& value)
    = 0;
  virtual void uniformMatrix4fv(IGLUniformLocation* location,
                                GLboolean transpose,
                                const std::array<float, 16>& value)
    = 0;
  virtual void useProgram(IGLProgram* program)      = 0;
  virtual void validateProgram(IGLProgram* program) = 0;
  virtual void vertexAttrib1f(GLuint indx, GLfloat x)             = 0;
  virtual void vertexAttrib1fv(GLuint indx, Float32Array& values) = 0;
  virtual void vertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) = 0;
  virtual void vertexAttrib2fv(GLuint indx, Float32Array& values) = 0;
  virtual void vertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z) = 0;
  virtual void vertexAttrib3fv(GLuint indx, Float32Array& values) = 0;
  virtual void vertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                              GLfloat w)
    = 0;
  virtual void vertexAttrib4fv(GLuint indx, Float32Array& values) = 0;
  virtual void vertexAttribDivisor(GLuint index, GLuint divisor)  = 0;
  virtual void vertexAttribPointer(GLuint indx, GLint size, GLenum type,
                                   GLboolean normalized, GLint stride,
                                   GLintptr offset)
    = 0;
  virtual void viewport(GLint x, GLint y, GLint width, GLint height) = 0;

protected:
  GLuint last_program;
  GLint last_texture;
  GLint last_active_texture;
  GLint last_array_buffer;
  GLint last_element_array_buffer;
  GLint last_vertex_array;
  GLint last_blend_src;
  GLint last_blend_dst;
  GLint last_blend_equation_rgb;
  GLint last_blend_equation_alpha;
  GLint last_viewport[4];
  GLboolean last_enable_blend;
  GLboolean last_enable_cull_face;
  GLboolean last_enable_depth_test;
  GLboolean last_enable_scissor_test;

}; // end of class GLRenderContext

class BABYLON_SHARED_EXPORT IGLVertexArrayObject {

public:
  IGLVertexArrayObject();

}; // end of class IGLVertexArrayObject

} // end of namespace GL
} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IGL_RENDERING_CONTEXT_H
