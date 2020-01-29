#include <babylon/GL/gl_rendering_context.h>

#include <array>

// glad
// GLAD_DEBUG  enables to debug all the OpenGl calls (calls GlGetError at each
// step) in order to use this, you need to replace the folder external/glad/
// content by the content of external/glad.debug, and to define GLAD_DEBUG
// below. #define GLAD_DEBUG
#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
// GLFW
#include <GLFW/glfw3.h>

// Logging
#include <babylon/core/logging.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

namespace BABYLON {
namespace GL {

void MessageCallback(GLenum /*source*/, GLenum type, GLuint /*id*/, GLenum severity,
                     GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

GLRenderingContext::GLRenderingContext()
{
  // Build map of string vs enums
  // Textures
  EnumMap["TEXTURE"]   = TEXTURE;
  EnumMap["TEXTURE0"]  = TEXTURE0;
  EnumMap["TEXTURE1"]  = TEXTURE1;
  EnumMap["TEXTURE2"]  = TEXTURE2;
  EnumMap["TEXTURE3"]  = TEXTURE3;
  EnumMap["TEXTURE4"]  = TEXTURE4;
  EnumMap["TEXTURE5"]  = TEXTURE5;
  EnumMap["TEXTURE6"]  = TEXTURE6;
  EnumMap["TEXTURE7"]  = TEXTURE7;
  EnumMap["TEXTURE8"]  = TEXTURE8;
  EnumMap["TEXTURE9"]  = TEXTURE9;
  EnumMap["TEXTURE10"] = TEXTURE10;
  EnumMap["TEXTURE11"] = TEXTURE11;
  EnumMap["TEXTURE12"] = TEXTURE12;
  EnumMap["TEXTURE13"] = TEXTURE13;
  EnumMap["TEXTURE14"] = TEXTURE14;
  EnumMap["TEXTURE15"] = TEXTURE15;
  EnumMap["TEXTURE16"] = TEXTURE16;
  EnumMap["TEXTURE17"] = TEXTURE17;
  EnumMap["TEXTURE18"] = TEXTURE18;
  EnumMap["TEXTURE19"] = TEXTURE19;
  EnumMap["TEXTURE20"] = TEXTURE20;
  EnumMap["TEXTURE21"] = TEXTURE21;
  EnumMap["TEXTURE22"] = TEXTURE22;
  EnumMap["TEXTURE23"] = TEXTURE23;
  EnumMap["TEXTURE24"] = TEXTURE24;
  EnumMap["TEXTURE25"] = TEXTURE25;
  EnumMap["TEXTURE26"] = TEXTURE26;
  EnumMap["TEXTURE27"] = TEXTURE27;
  EnumMap["TEXTURE28"] = TEXTURE28;
  EnumMap["TEXTURE29"] = TEXTURE29;
  EnumMap["TEXTURE30"] = TEXTURE30;
  EnumMap["TEXTURE31"] = TEXTURE31;
  // Color attachments
  EnumMap["COLOR_ATTACHMENT0"]  = COLOR_ATTACHMENT0;
  EnumMap["COLOR_ATTACHMENT1"]  = COLOR_ATTACHMENT1;
  EnumMap["COLOR_ATTACHMENT2"]  = COLOR_ATTACHMENT2;
  EnumMap["COLOR_ATTACHMENT3"]  = COLOR_ATTACHMENT3;
  EnumMap["COLOR_ATTACHMENT4"]  = COLOR_ATTACHMENT4;
  EnumMap["COLOR_ATTACHMENT5"]  = COLOR_ATTACHMENT5;
  EnumMap["COLOR_ATTACHMENT6"]  = COLOR_ATTACHMENT6;
  EnumMap["COLOR_ATTACHMENT7"]  = COLOR_ATTACHMENT7;
  EnumMap["COLOR_ATTACHMENT8"]  = COLOR_ATTACHMENT8;
  EnumMap["COLOR_ATTACHMENT9"]  = COLOR_ATTACHMENT9;
  EnumMap["COLOR_ATTACHMENT10"] = COLOR_ATTACHMENT10;
  EnumMap["COLOR_ATTACHMENT11"] = COLOR_ATTACHMENT11;
  EnumMap["COLOR_ATTACHMENT12"] = COLOR_ATTACHMENT12;
  EnumMap["COLOR_ATTACHMENT13"] = COLOR_ATTACHMENT13;
  EnumMap["COLOR_ATTACHMENT14"] = COLOR_ATTACHMENT14;
  EnumMap["COLOR_ATTACHMENT15"] = COLOR_ATTACHMENT15;
  EnumMap["COLOR_ATTACHMENT16"] = COLOR_ATTACHMENT16;
  EnumMap["COLOR_ATTACHMENT17"] = COLOR_ATTACHMENT17;
  EnumMap["COLOR_ATTACHMENT18"] = COLOR_ATTACHMENT18;
  EnumMap["COLOR_ATTACHMENT19"] = COLOR_ATTACHMENT19;
  EnumMap["COLOR_ATTACHMENT20"] = COLOR_ATTACHMENT20;
  EnumMap["COLOR_ATTACHMENT21"] = COLOR_ATTACHMENT21;
  EnumMap["COLOR_ATTACHMENT22"] = COLOR_ATTACHMENT22;
  EnumMap["COLOR_ATTACHMENT23"] = COLOR_ATTACHMENT23;
  EnumMap["COLOR_ATTACHMENT24"] = COLOR_ATTACHMENT24;
  EnumMap["COLOR_ATTACHMENT25"] = COLOR_ATTACHMENT25;
  EnumMap["COLOR_ATTACHMENT26"] = COLOR_ATTACHMENT26;
  EnumMap["COLOR_ATTACHMENT27"] = COLOR_ATTACHMENT27;
  EnumMap["COLOR_ATTACHMENT28"] = COLOR_ATTACHMENT28;
  EnumMap["COLOR_ATTACHMENT29"] = COLOR_ATTACHMENT29;
  EnumMap["COLOR_ATTACHMENT30"] = COLOR_ATTACHMENT30;
  EnumMap["COLOR_ATTACHMENT31"] = COLOR_ATTACHMENT31;
}

GLRenderingContext::~GLRenderingContext() = default;

std::string GlErrorCodeStr(GLenum error_code)
{
  std::string error;
  switch (error_code) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
  }
  return error;
}

void glad_post_call_callback(const char* name, void* /*funcptr*/, int /*len_args*/, ...)
{
  GLenum error_code;
  error_code = glGetError();

  std::stringstream msg_str;
  msg_str << "ERROR " << GlErrorCodeStr(error_code) << "(" << error_code << ") in " << name << "\n";
  if (error_code != GL_NO_ERROR) {
    fprintf(stderr, "%s", msg_str.str().c_str());
#ifdef _MSC_VER
    OutputDebugString(msg_str.str().c_str());
#endif
  }
}

void glad_pre_call_callback(const char* name, void* /*funcptr*/, int /*len_args*/, ...)
{
  std::stringstream msg_str;
  msg_str << "glad_pre_call_callback " << name << "\n";
  fprintf(stderr, "%s", msg_str.str().c_str());
#ifdef _MSC_VER
  OutputDebugString(msg_str.str().c_str());
#if defined(_WIN32) && !defined(_WIN64)
#define WIN_32BITS
#endif
#endif
}

bool GLRenderingContext::initialize(bool enableGLDebugging)
{
  // HUM : glad already loaded by imgui ?
  // Initialize glad
  /*if (!gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    fprintf(stderr, "gladLoadGLLoader: Failed to initialize OpenGL ES context\n");
    return false;
  }
  if (!GLAD_GL_ES_VERSION_3_0) {
    fprintf(stderr, "GLAD could not initialize OpenGl ES 3.0\n");
  }*/
#ifdef GLAD_DEBUG
  glad_set_pre_callback(glad_pre_call_callback);
  glad_set_post_callback(glad_post_call_callback);
#endif

  // Log the GL version
  BABYLON_LOGF_INFO("GLRenderingContext", "Using GL version: %s", glGetString(GL_VERSION))

  // Setup OpenGL options
  // glEnable(GL_MULTISAMPLE);

  // Enable debug output
#if !defined(__EMSCRIPTEN__) && !defined(WIN_32BITS)
  if (enableGLDebugging) {
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(MessageCallback, nullptr);
  }
#endif //__EMSCRIPTEN__
  return true;
}

GLenum GLRenderingContext::operator[](const std::string& name)
{
  return EnumMap[name];
}

void GLRenderingContext::activeTexture(GLenum texture)
{
  glActiveTexture(texture);
}

void GLRenderingContext::attachShader(IGLProgram* program, IGLShader* shader)
{
  glAttachShader(program->value, shader ? shader->value : 0);
}

void GLRenderingContext::beginQuery(GLenum target, IGLQuery* query)
{
  glBeginQuery(target, query ? query->value : 0);
}

void GLRenderingContext::beginTransformFeedback(GLenum primitiveMode)
{
  glBeginTransformFeedback(primitiveMode);
}

void GLRenderingContext::bindAttribLocation(IGLProgram* program, GLuint index,
                                            const std::string& name)
{
  glBindAttribLocation(program->value, index, name.c_str());
}

void GLRenderingContext::bindBuffer(GLenum target, IGLBuffer* buffer)
{
  glBindBuffer(target, buffer ? buffer->value : 0);
}

void GLRenderingContext::bindFramebuffer(GLenum target, IGLFramebuffer* framebuffer)
{
  glBindFramebuffer(target, framebuffer ? framebuffer->value : 0);
}

void GLRenderingContext::bindBufferBase(GLenum target, GLuint index, IGLBuffer* buffer)
{
  glBindBufferBase(target, index, buffer ? buffer->value : 0);
}

void GLRenderingContext::bindRenderbuffer(GLenum target, IGLRenderbuffer* renderbuffer)
{
  glBindRenderbuffer(target, renderbuffer ? renderbuffer->value : 0);
}

void GLRenderingContext::bindTexture(GLenum target, IGLTexture* texture)
{
  if (texture) {
    glBindTexture(target, texture->value);
  }
  else {
    glDisable(target);
  }
}

void GLRenderingContext::bindTransformFeedback(GLenum target,
                                               IGLTransformFeedback* transformFeedback)
{
  glBindTransformFeedback(target, transformFeedback->value);
}

void GLRenderingContext::blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  glBlendColor(red, green, blue, alpha);
}

void GLRenderingContext::blendEquation(GLenum mode)
{
  glBlendEquation(mode);
}

void GLRenderingContext::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
  glBlendEquationSeparate(modeRGB, modeAlpha);
}

void GLRenderingContext::blendFunc(GLenum sfactor, GLenum dfactor)
{
  glBlendFunc(sfactor, dfactor);
}

void GLRenderingContext::blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                           GLenum dstAlpha)
{
  glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void GLRenderingContext::blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                         GLbitfield mask, GLenum filter)
{
  glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void GLRenderingContext::bufferData(GLenum target, GLsizeiptr sizeiptr, GLenum usage)
{
  glBufferData(target, sizeiptr >> 32, reinterpret_cast<any>(sizeiptr & 0x0000ffff), usage);
}

void GLRenderingContext::bufferData(GLenum target, const Float32Array& data, GLenum usage)
{
  glBufferData(target, static_cast<GLint>(data.size() * sizeof(GLfloat)), data.data(), usage);
}

void GLRenderingContext::bufferData(GLenum target, const Int32Array& data, GLenum usage)
{
  glBufferData(target, static_cast<GLint>(data.size() * sizeof(int32_t)), data.data(), usage);
}

void GLRenderingContext::bufferData(GLenum target, const Uint16Array& data, GLenum usage)
{
  glBufferData(target, static_cast<GLint>(data.size() * sizeof(uint16_t)), data.data(), usage);
}

void GLRenderingContext::bufferData(GLenum target, const Uint32Array& data, GLenum usage)
{
  glBufferData(target, static_cast<GLint>(data.size() * sizeof(uint32_t)), data.data(), usage);
}

void GLRenderingContext::bufferSubData(GLenum target, GLintptr offset, const Uint8Array& data)
{
  glBufferSubData(target, offset, static_cast<GLint>(data.size() * sizeof(GLbyte)), data.data());
}

void GLRenderingContext::bufferSubData(GLenum target, GLintptr offset, const Float32Array& data)
{
  glBufferSubData(target, offset, static_cast<GLint>(data.size() * sizeof(GLfloat)), data.data());
}

void GLRenderingContext::bufferSubData(GLenum target, GLintptr offset, Int32Array& data)
{
  glBufferSubData(target, offset, static_cast<GLint>(data.size() * sizeof(int32_t)), data.data());
}

void GLRenderingContext::bindVertexArray(GL::IGLVertexArrayObject* vao)
{
  glBindVertexArray(vao ? vao->value : 0);
}

GLenum GLRenderingContext::checkFramebufferStatus(GLenum target)
{
  return glCheckFramebufferStatus(target);
}

void GLRenderingContext::clear(GLbitfield mask)
{
  glClear(mask);
}

void GLRenderingContext::clearBufferfv(GLenum buffer, GLint drawbuffer,
                                       const std::vector<GLfloat>& values, GLint /*srcOffset*/)
{
  glClearBufferfv(buffer, drawbuffer, values.data());
}

void GLRenderingContext::clearBufferiv(GLenum buffer, GLint drawbuffer,
                                       const std::vector<GLint>& values, GLint /*srcOffset*/)
{
  glClearBufferiv(buffer, drawbuffer, values.data());
}

void GLRenderingContext::clearBufferuiv(GLenum buffer, GLint drawbuffer,
                                        const std::vector<GLuint>& values, GLint /*srcOffset*/)
{
  glClearBufferuiv(buffer, drawbuffer, values.data());
}

void GLRenderingContext::clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth,
                                       GLint stencil)
{
  glClearBufferfi(buffer, drawbuffer, depth, stencil);
}

void GLRenderingContext::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  glClearColor(red, green, blue, alpha);
}

void GLRenderingContext::clearDepth(GLclampf depth)
{
  glClearDepthf(depth);
}

void GLRenderingContext::clearStencil(GLint stencil)
{
  glClearStencil(stencil);
}

void GLRenderingContext::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
  glColorMask(red, green, blue, alpha);
}

void GLRenderingContext::compileShader(IGLShader* shader)
{
  glCompileShader(shader->value);

  GLint compileStatus;
  glGetShaderiv(shader->value, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE)
  {
    char shaderInfoLog[5000];
    GLsizei shaderInfoLogLength;
    glGetShaderInfoLog(shader->value, 5000, &shaderInfoLogLength, shaderInfoLog);
    BABYLON_LOG_ERROR("compileShader", "Error!", shaderInfoLog);
    BABYLON_LOG_ERROR("compileShader", "Related shader source", getShaderSource(shader).c_str());
  }

}

void GLRenderingContext::compressedTexImage2D(GLenum target, GLint level, GLenum internalformat,
                                              GLint width, GLint height, GLint border,
                                              const Uint8Array& pixels)
{
  glCompressedTexImage2D(target, level, internalformat, width, height, border,
                         static_cast<GLint>(pixels.size() * sizeof(GLbyte)), &pixels[0]);
}

void GLRenderingContext::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset,
                                                 GLint yoffset, GLsizei width, GLsizei height,
                                                 GLenum format, GLsizeiptr size)
{
  glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format,
                            static_cast<GLint>(size >> 32),
                            reinterpret_cast<const GLint*>(size & 0x0000ffff));
}

void GLRenderingContext::copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x,
                                        GLint y, GLint width, GLint height, GLint border)
{
  glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

void GLRenderingContext::copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                           GLint x, GLint y, GLint width, GLint height)
{
  glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

std::shared_ptr<IGLBuffer> GLRenderingContext::createBuffer()
{
  GLuint buffer = 0;
  glGenBuffers(1, &buffer);
  return std::make_shared<IGLBuffer>(buffer);
}

IGLFramebufferPtr GLRenderingContext::createFramebuffer()
{
  GLuint buffer = 0;
  glGenFramebuffers(1, &buffer);
  return std::make_shared<IGLFramebuffer>(buffer);
}

IGLProgramPtr GLRenderingContext::createProgram()
{
  return std::make_shared<IGLProgram>(glCreateProgram());
}

std::unique_ptr<IGLQuery> GLRenderingContext::createQuery()
{
  return std::make_unique<IGLQuery>(0);
}

IGLRenderbufferPtr GLRenderingContext::createRenderbuffer()
{
  GLuint buffer;
  glGenRenderbuffers(1, &buffer);
  return std::make_shared<IGLRenderbuffer>(buffer);
}

IGLShaderPtr GLRenderingContext::createShader(GLenum type)
{
  return std::make_shared<IGLShader>(glCreateShader(type));
}

IGLTexturePtr GLRenderingContext::createTexture()
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  return std::make_shared<IGLTexture>(texture);
}

IGLTransformFeedbackPtr GLRenderingContext::createTransformFeedback()
{
  GLuint transformFeedbackArray = 0;
  glGenTransformFeedbacks(1, &transformFeedbackArray);
  return std::make_shared<IGLTransformFeedback>(transformFeedbackArray);
}

IGLVertexArrayObjectPtr GLRenderingContext::createVertexArray()
{
  GLuint vertexArray = 0;
  glGenVertexArrays(1, &vertexArray);
  return std::make_shared<IGLVertexArrayObject>(vertexArray);
}

void GLRenderingContext::cullFace(GLenum mode)
{
  glCullFace(mode);
}

void GLRenderingContext::deleteBuffer(IGLBuffer* buffer)
{
  glDeleteBuffers(1, &buffer->value);
  buffer->value = 0;
}

void GLRenderingContext::deleteFramebuffer(IGLFramebuffer* framebuffer)
{
  glDeleteFramebuffers(1, &framebuffer->value);
  framebuffer->value = 0;
}

void GLRenderingContext::deleteProgram(IGLProgram* program)
{
  glDeleteProgram(program->value);
}

void GLRenderingContext::deleteQuery(IGLQuery* /*query*/)
{
}

void GLRenderingContext::deleteRenderbuffer(IGLRenderbuffer* renderbuffer)
{
  glDeleteRenderbuffers(1, &renderbuffer->value);
  renderbuffer->value = 0;
}

void GLRenderingContext::deleteShader(IGLShader* shader)
{
  glDeleteShader(shader ? shader->value : 0);
}

void GLRenderingContext::deleteTexture(IGLTexture* texture)
{
  glDeleteTextures(1, &texture->value);
  texture->value = 0;
}

void GLRenderingContext::deleteTransformFeedback(IGLTransformFeedback* transformFeedback)
{
  glDeleteTransformFeedbacks(1, &transformFeedback->value);
  transformFeedback->value = 0;
}

void GLRenderingContext::deleteVertexArray(IGLVertexArrayObject* vao)
{
  glDeleteVertexArrays(1, &vao->value);
  vao->value = 0;
}

void GLRenderingContext::depthFunc(GLenum func)
{
  glDepthFunc(func);
}

void GLRenderingContext::depthMask(GLboolean flag)
{
  glDepthMask(flag);
}

void GLRenderingContext::depthRange(GLclampf zNear, GLclampf zFar)
{
  glDepthRangef(zNear, zFar);
}

void GLRenderingContext::detachShader(IGLProgram* program, IGLShader* shader)
{
  glDetachShader(program->value, shader->value);
}

void GLRenderingContext::disable(GLenum cap)
{
  glDisable(cap);
}

void GLRenderingContext::disableVertexAttribArray(GLuint index)
{
  glDisableVertexAttribArray(index);
}

void GLRenderingContext::drawArrays(GLenum mode, GLint first, GLint count)
{
  glDrawArrays(mode, first, count);
}

void GLRenderingContext::drawArraysInstanced(GLenum mode, GLint first, GLsizei count,
                                             GLsizei instanceCount)
{
  glDrawArraysInstanced(mode, first, count, instanceCount);
}

void GLRenderingContext::drawBuffers(const std::vector<GLenum>& buffers)
{
  auto n = static_cast<GLsizei>(buffers.size());
  glDrawBuffers(n, &buffers[0]);
}

void GLRenderingContext::drawElements(GLenum mode, GLint count, GLenum type, GLintptr offset)
{
  glDrawElements(mode, count, type, reinterpret_cast<any>(offset));
}

void GLRenderingContext::drawElementsInstanced(GLenum mode, GLsizei count, GLenum type,
                                               GLintptr offset, GLsizei instanceCount)
{
  glDrawElementsInstanced(mode, count, type, reinterpret_cast<any>(offset), instanceCount);
}

void GLRenderingContext::enable(GLenum cap)
{
  glEnable(cap);
}

void GLRenderingContext::enableVertexAttribArray(GLuint index)
{
  glEnableVertexAttribArray(index);
}

void GLRenderingContext::endQuery(GLenum target)
{
  glEndQuery(target);
}

void GLRenderingContext::endTransformFeedback()
{
  glEndTransformFeedback();
}

void GLRenderingContext::finish()
{
  glFinish();
}

void GLRenderingContext::flush()
{
  glFlush();
}

void GLRenderingContext::framebufferRenderbuffer(GLenum target, GLenum attachment,
                                                 GLenum renderbuffertarget,
                                                 IGLRenderbuffer* renderbuffer)
{
  glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer->value);
}

void GLRenderingContext::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget,
                                              IGLTexture* texture, GLint level)
{
  glFramebufferTexture2D(target, attachment, textarget, texture->value, level);
}

void GLRenderingContext::framebufferTextureMultiviewOVR(GLenum /*target*/, GLenum /*attachment*/,
                                                        IGLTexture* /*texture*/, GLint /*level*/,
                                                        GLint /*baseViewIndex*/, GLint /*numViews*/)
{
}

void GLRenderingContext::frontFace(GLenum mode)
{
  glFrontFace(mode);
}

void GLRenderingContext::generateMipmap(GLenum target)
{
  glGenerateMipmap(target);
}

std::vector<IGLShader*> GLRenderingContext::getAttachedShaders(IGLProgram* /*program*/)
{
  // Not supported
  return std::vector<IGLShader*>();
}

GLint GLRenderingContext::getAttribLocation(IGLProgram* program, const std::string& name)
{
  return glGetAttribLocation(program->value, name.c_str());
}

GL::any GLRenderingContext::getExtension(const std::string& /*name*/)
{
  return nullptr;
}

bool GLRenderingContext::hasExtension(const std::string& /*extension*/)
{
  return true;
}

std::array<int, 3> GLRenderingContext::getScissorBoxParameter()
{
  return {{0, 0, 0}};
}

GLint GLRenderingContext::getParameteri(GLenum pname)
{
  GLint parameter = 0;
  glGetIntegerv(pname, &parameter);
  return parameter;
}

GLfloat GLRenderingContext::getParameterf(GLenum pname)
{
  GLfloat parameter = 0;
  glGetFloatv(pname, &parameter);
  return parameter;
}

GLboolean GLRenderingContext::getQueryParameterb(IGLQuery* query, GLenum pname)
{
  GLuint parameter = 0;
  glGetQueryObjectuiv(query->value, pname, &parameter);
  return parameter == GL_TRUE;
}

GLuint GLRenderingContext::getQueryParameteri(IGLQuery* query, GLenum pname)
{
  unsigned int parameter = 0;
  glGetQueryObjectuiv(query->value, pname, &parameter);
  return parameter;
}

std::string GLRenderingContext::getString(GLenum pname)
{
  const char* str = reinterpret_cast<const char*>(glGetString(pname));
  return str ? std::string(str) : "";
}

GLint GLRenderingContext::getTexParameteri(GLenum pname)
{
  GLint parameter = 0;
  glGetTexParameteriv(GL_TEXTURE_2D, pname, &parameter);
  return parameter;
}

GLfloat GLRenderingContext::getTexParameterf(GLenum pname)
{
  GLfloat parameter = 0;
  glGetTexParameterfv(GL_TEXTURE_2D, pname, &parameter);
  return parameter;
}

GLenum GLRenderingContext::getError()
{
  return glGetError();
}

const char* GLRenderingContext::getErrorString(GLenum err)
{
  switch (err) {
    case GL_INVALID_ENUM:
      return "Invalid enum";
    case GL_INVALID_OPERATION:
      return "Invalid operation";
    case GL_INVALID_VALUE:
      return "Invalid value";
    case GL_OUT_OF_MEMORY:
      return "Out of memory";
    default:
      return "Unknown error";
  }
}

GLint GLRenderingContext::getProgramParameter(IGLProgram* program, GLenum pname)
{
  GLint parameter = 0;
  glGetProgramiv(program->value, pname, &parameter);
  return parameter;
}

std::string GLRenderingContext::getProgramInfoLog(IGLProgram* program)
{
  GLint k = -1;
  glGetProgramiv(program->value, GL_INFO_LOG_LENGTH, &k);
  if (k <= 0) {
    return "";
  }

  std::string result;
  result.reserve(static_cast<size_t>(k + 1));
  glGetProgramInfoLog(program->value, k, &k, const_cast<char*>(result.c_str()));
  return result;
}

GLint GLRenderingContext::getRenderbufferParameter(GLenum target, GLenum pname)
{
  GLint params;
  glGetRenderbufferParameteriv(target, pname, &params);
  return params;
}

GLint GLRenderingContext::getShaderParameter(IGLShader* shader, GLenum pname)
{
  GLint parameter = 0;
  glGetShaderiv(shader->value, pname, &parameter);
  return parameter;
}

IGLShaderPrecisionFormat* GLRenderingContext::getShaderPrecisionFormat(GLenum /*shadertype*/,
                                                                       GLenum /*precisiontype*/)
{
  return nullptr;
}

std::string GLRenderingContext::getShaderInfoLog(IGLShader* shader)
{
  GLint logSize = -1;
  glGetShaderiv(shader->value, GL_INFO_LOG_LENGTH, &logSize);
  if (logSize <= 0) {
    return "";
  }

  // The maxLength includes the NULL character
  auto logSizei = static_cast<GLbitfield>(logSize);
  std::vector<GLchar> infoLog(logSizei);
  glGetShaderInfoLog(shader->value, logSize, &logSize, &infoLog[0]);
  return std::string(infoLog.begin(), infoLog.end());
}

std::string GLRenderingContext::getShaderSource(IGLShader* shader)
{
  GLchar buffer[5000];
  GLsizei size;
  glGetShaderSource(shader->value, 5000, &size, &buffer[0]);

  return std::string(buffer, static_cast<std::size_t>(size) + 1);
}

GLuint GLRenderingContext::getUniformBlockIndex(IGLProgram* program,
                                                const std::string& uniformBlockName)
{
  return glGetUniformBlockIndex(program->value, uniformBlockName.data());
}

std::unique_ptr<IGLUniformLocation> GLRenderingContext::getUniformLocation(IGLProgram* program,
                                                                           const std::string& name)
{
  GLint value = glGetUniformLocation(program->value, name.c_str());
  if (value != -1) {
    return std::make_unique<IGLUniformLocation>(value);
  }

  return nullptr;
}

void GLRenderingContext::hint(GLenum target, GLenum mode)
{
  glHint(target, mode);
}

GLboolean GLRenderingContext::isBuffer(IGLBuffer* buffer)
{
  return glIsBuffer(buffer->value) == GL_TRUE;
}

GLboolean GLRenderingContext::isEnabled(GLenum cap)
{
  return glIsEnabled(cap) == GL_TRUE;
}

GLboolean GLRenderingContext::isFramebuffer(IGLFramebuffer* framebuffer)
{
  return glIsFramebuffer(framebuffer->value) == GL_TRUE;
}

GLboolean GLRenderingContext::isProgram(IGLProgram* program)
{
  return glIsProgram(program->value) == GL_TRUE;
}

GLboolean GLRenderingContext::isRenderbuffer(IGLRenderbuffer* renderbuffer)
{
  return glIsRenderbuffer(renderbuffer->value) == GL_TRUE;
}

GLboolean GLRenderingContext::isShader(IGLShader* shader)
{
  return glIsShader(shader->value) == GL_TRUE;
}

GLboolean GLRenderingContext::isTexture(IGLTexture* texture)
{
  return glIsTexture(texture->value) == GL_TRUE;
}

void GLRenderingContext::lineWidth(GLfloat width)
{
  glLineWidth(width);
}

bool GLRenderingContext::linkProgram(IGLProgram* program)
{
  glLinkProgram(program->value);

  // Test linker result.
  GLint linkSucceed = GL_FALSE;
  glGetProgramiv(program->value, GL_LINK_STATUS, &linkSucceed);

  return linkSucceed != GL_FALSE;
}

void GLRenderingContext::pixelStorei(GLenum pname, GLint param)
{
  if (pname != UNPACK_FLIP_Y_WEBGL) {
    glPixelStorei(pname, param);
  }
}

void GLRenderingContext::polygonOffset(GLfloat factor, GLfloat units)
{
  glPolygonOffset(factor, units);
}

void GLRenderingContext::readBuffer(GLenum src)
{
  glReadBuffer(src);
}

void GLRenderingContext::readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                                    GLenum type, Float32Array& pixels)
{
  glReadPixels(x, y, width, height, format, type, &pixels[0]);
}

void GLRenderingContext::readPixels(GLint x, GLint y, GLint width, GLint height, GLenum format,
                                    GLenum type, Uint8Array& pixels)
{
  glReadPixels(x, y, width, height, format, type, &pixels[0]);
}

void GLRenderingContext::renderbufferStorage(GLenum target, GLenum internalformat, GLint width,
                                             GLint height)
{
  glRenderbufferStorage(target, internalformat, width, height);
}

void GLRenderingContext::renderbufferStorageMultisample(GLenum target, GLsizei samples,
                                                        GLenum internalFormat, GLsizei width,
                                                        GLsizei height)
{
  glRenderbufferStorageMultisample(target, samples, internalFormat, width, height);
}

void GLRenderingContext::sampleCoverage(GLclampf value, GLboolean invert)
{
  glSampleCoverage(value, invert);
}

void GLRenderingContext::scissor(GLint x, GLint y, GLint width, GLint height)
{
  glScissor(x, y, width, height);
}

void GLRenderingContext::shaderSource(IGLShader* shader, const std::string& source)
{
  auto length        = static_cast<int>(source.length());
  const GLchar* line = source.c_str();
  glShaderSource(shader->value, 1, &line, &length);
}

void GLRenderingContext::stencilFunc(GLenum func, GLint ref, GLuint mask)
{
  glStencilFunc(func, ref, mask);
}

void GLRenderingContext::stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
  glStencilFuncSeparate(face, func, ref, mask);
}

void GLRenderingContext::stencilMask(GLuint mask)
{
  glStencilMask(mask);
}

void GLRenderingContext::stencilMaskSeparate(GLenum face, GLuint mask)
{
  glStencilMaskSeparate(face, mask);
}

void GLRenderingContext::stencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
  glStencilOp(fail, zfail, zpass);
}

void GLRenderingContext::stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
  glStencilOpSeparate(face, fail, zfail, zpass);
}

void GLRenderingContext::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width,
                                    GLsizei height, GLint border, GLenum format, GLenum type,
                                    const Uint8Array* const pixels)
{
  if (pixels == nullptr) {
    glTexImage2D(target, level, internalformat, width, height, border, format, type, nullptr);
  }
  else {
    glTexImage2D(target, level, internalformat, width, height, border, format, type,
                 pixels->data());
  }
}

void GLRenderingContext::texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width,
                                    GLsizei height, GLsizei depth, GLint border, GLenum format,
                                    GLenum type, const Uint8Array& pixels)
{
  glTexImage3D(target, level, internalformat, width, height, depth, border, format, type,
               &pixels[0]);
}

void GLRenderingContext::texParameterf(GLenum target, GLenum pname, GLfloat param)
{
  glTexParameterf(target, pname, param);
}

void GLRenderingContext::texParameteri(GLenum target, GLenum pname, GLint param)
{
  glTexParameteri(target, pname, param);
}

void GLRenderingContext::texStorage3D(GLenum target, GLint levels, GLenum internalformat,
                                      GLsizei width, GLsizei height, GLsizei depth)
{
  glTexStorage3D(target, levels, internalformat, width, height, depth);
}

void GLRenderingContext::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                       GLint width, GLint height, GLenum format, GLenum type,
                                       any pixels)
{
  glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void GLRenderingContext::transformFeedbackVaryings(IGLProgram* program,
                                                   const std::vector<std::string>& varyings,
                                                   GLenum bufferMode)
{
  std::vector<const char*> _varyings;
  for (auto varying : varyings) {
    _varyings.emplace_back(varying.c_str());
  }

  glTransformFeedbackVaryings(program->value, static_cast<int>(varyings.size()), &_varyings[0],
                              bufferMode);
}

void GLRenderingContext::uniform1f(IGLUniformLocation* location, GLfloat x)
{
  glUniform1f(location->value, x);
}

void GLRenderingContext::GLRenderingContext::uniform1fv(GL::IGLUniformLocation* uniform,
                                                        const Float32Array& array)
{
  glUniform1fv(uniform->value, static_cast<int>(array.size() * sizeof(GLfloat)), array.data());
}

void GLRenderingContext::uniform1i(IGLUniformLocation* location, GLint x)
{
  glUniform1i(location->value, x);
}

void GLRenderingContext::uniform1iv(IGLUniformLocation* location, const Int32Array& v)
{
  glUniform1iv(location->value, static_cast<GLint>(v.size() * sizeof(int32_t)), v.data());
}

void GLRenderingContext::uniform2f(IGLUniformLocation* location, GLfloat x, GLfloat y)
{
  glUniform2f(location->value, x, y);
}

void GLRenderingContext::uniform2fv(IGLUniformLocation* location, const Float32Array& v)
{
  glUniform2fv(location->value, static_cast<GLint>(v.size() * sizeof(GLfloat)), v.data());
}

void GLRenderingContext::uniform2i(IGLUniformLocation* location, GLint x, GLint y)
{
  glUniform2i(location->value, x, y);
}

void GLRenderingContext::uniform2iv(IGLUniformLocation* location, const Int32Array& v)
{
  glUniform2iv(location->value, static_cast<GLint>(v.size() * sizeof(int32_t)), v.data());
}

void GLRenderingContext::uniform3f(IGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z)
{
  glUniform3f(location->value, x, y, z);
}

void GLRenderingContext::uniform3fv(IGLUniformLocation* location, const Float32Array& v)
{
  glUniform3fv(location->value, static_cast<GLint>(v.size() * sizeof(GLfloat)), v.data());
}

void GLRenderingContext::uniform3i(IGLUniformLocation* location, GLint x, GLint y, GLint z)
{
  glUniform3i(location->value, x, y, z);
}

void GLRenderingContext::uniform3iv(IGLUniformLocation* location, const Int32Array& v)
{
  glUniform3iv(location->value, static_cast<GLint>(v.size() * sizeof(int32_t)), v.data());
}

void GLRenderingContext::uniform4f(IGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z,
                                   GLfloat w)
{
  glUniform4f(location->value, x, y, z, w);
}

void GLRenderingContext::uniform4fv(IGLUniformLocation* location, const Float32Array& v)
{
  glUniform4fv(location->value, static_cast<GLint>(v.size() * sizeof(GLfloat)), v.data());
}

void GLRenderingContext::uniform4i(IGLUniformLocation* location, GLint x, GLint y, GLint z, GLint w)
{
  glUniform4i(location->value, x, y, z, w);
}

void GLRenderingContext::uniform4iv(IGLUniformLocation* location, const Int32Array& v)
{
  glUniform4iv(location->value, static_cast<GLint>(v.size() * sizeof(int32_t)), v.data());
}

void GLRenderingContext::uniformBlockBinding(IGLProgram* program, GLuint uniformBlockIndex,
                                             GLuint uniformBlockBinding)
{
  glUniformBlockBinding(program->value, uniformBlockIndex, uniformBlockBinding);
}

void GLRenderingContext::uniformMatrix2fv(IGLUniformLocation* location, GLboolean transpose,
                                          const Float32Array& value)
{
  glUniformMatrix2fv(location->value, static_cast<GLint>(value.size() / 16), transpose,
                     value.data());
}

void GLRenderingContext::uniformMatrix3fv(IGLUniformLocation* location, GLboolean transpose,
                                          const Float32Array& value)
{
  glUniformMatrix3fv(location->value, static_cast<GLint>(value.size() / 16), transpose,
                     value.data());
}

void GLRenderingContext::uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                                          const Float32Array& value)
{
  glUniformMatrix4fv(location->value, static_cast<GLint>(value.size() / 16), transpose,
                     value.data());
}

void GLRenderingContext::uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                                          const std::array<float, 16>& value)
{
  glUniformMatrix4fv(location->value, static_cast<GLint>(value.size() / 16), transpose,
                     value.data());
}

void GLRenderingContext::useProgram(IGLProgram* program)
{
  glUseProgram(program->value);
}

void GLRenderingContext::validateProgram(IGLProgram* program)
{
  glValidateProgram(program->value);
}

void GLRenderingContext::vertexAttrib1f(GLuint indx, GLfloat x)
{
  glVertexAttrib1f(indx, x);
}

void GLRenderingContext::vertexAttrib1fv(GLuint indx, Float32Array& values)
{
  glVertexAttrib1fv(indx, values.data());
}

void GLRenderingContext::vertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
  glVertexAttrib2f(indx, x, y);
}

void GLRenderingContext::vertexAttrib2fv(GLuint indx, Float32Array& values)
{
  glVertexAttrib2fv(indx, values.data());
}

void GLRenderingContext::vertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
  glVertexAttrib3f(indx, x, y, z);
}

void GLRenderingContext::vertexAttrib3fv(GLuint indx, Float32Array& values)
{
  glVertexAttrib3fv(indx, values.data());
}

void GLRenderingContext::vertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  glVertexAttrib4f(indx, x, y, z, w);
}

void GLRenderingContext::vertexAttrib4fv(GLuint indx, Float32Array& values)
{
  glVertexAttrib4fv(indx, values.data());
}

void GLRenderingContext::vertexAttribDivisor(GLuint index, GLuint divisor)
{
  glVertexAttribDivisor(index, divisor);
}

void GLRenderingContext::vertexAttribPointer(GLuint indx, GLint size, GLenum type,
                                             GLboolean normalized, GLint stride, GLintptr offset)
{
  glVertexAttribPointer(indx, size, type, normalized, stride, reinterpret_cast<any>(offset));
}

void GLRenderingContext::viewport(GLint x, GLint y, GLint width, GLint height)
{
  glViewport(x, y, width, height);
}

} // end of namespace GL
} // end of namespace BABYLON
