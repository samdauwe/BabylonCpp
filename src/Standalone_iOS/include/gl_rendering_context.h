#ifndef BABYLON_IMPL_GL_RENDERING_CONTEXT_H
#define BABYLON_IMPL_GL_RENDERING_CONTEXT_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <unordered_map>

namespace BABYLON {
namespace GL {

class GLRenderingContext : public BABYLON::GL::IGLRenderingContext {

public:
  GLRenderingContext();
  ~GLRenderingContext() override; // = default

  bool initialize(bool enableGLDebugging = false) override;
  GLenum operator[](const std::string& name) override;
  void activeTexture(GLenum texture) override;
  void attachShader(IGLProgram* program, IGLShader* shader) override;
  void beginQuery(GLenum target, IGLQuery* query) override;
  void beginTransformFeedback(GLenum primitiveMode) override;
  void bindAttribLocation(IGLProgram* program, GLuint index, const std::string& name) override;
  void bindBuffer(GLenum target, IGLBuffer* buffer) override;
  void bindFramebuffer(GLenum target, IGLFramebuffer* framebuffer) override;
  void bindBufferBase(GLenum target, GLuint index, IGLBuffer* buffer) override;
  void bindRenderbuffer(GLenum target, IGLRenderbuffer* renderbuffer) override;
  void bindTexture(GLenum target, IGLTexture* texture) override;
  void bindTransformFeedback(GLenum target, IGLTransformFeedback* transformFeedback) override;
  void blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override;
  void blendEquation(GLenum mode) override;
  void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) override;
  void blendFunc(GLenum sfactor, GLenum dfactor) override;
  void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) override;
  void blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0,
                       GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) override;
  void bufferData(GLenum target, GLsizeiptr size, GLenum usage) override;
  void bufferData(GLenum target, const Float32Array& data, GLenum usage) override;
  void bufferData(GLenum target, const Int32Array& data, GLenum usage) override;
  void bufferData(GLenum target, const Uint16Array& data, GLenum usage) override;
  void bufferData(GLenum target, const Uint32Array& data, GLenum usage) override;
  void bufferSubData(GLenum target, GLintptr offset, const Uint8Array& data) override;
  void bufferSubData(GLenum target, GLintptr offset, const Float32Array& data) override;
  void bufferSubData(GLenum target, GLintptr offset, Int32Array& data) override;
  void bindVertexArray(GL::IGLVertexArrayObject* vao) override;
  GLenum checkFramebufferStatus(GLenum target) override;
  void clear(GLbitfield mask) override;
  void clearBufferfv(GLenum buffer, GLint drawbuffer, const std::vector<GLfloat>& values,
                     GLint srcOffset = 0) override;
  void clearBufferiv(GLenum buffer, GLint drawbuffer, const std::vector<GLint>& values,
                     GLint srcOffset = 0) override;
  void clearBufferuiv(GLenum buffer, GLint drawbuffer, const std::vector<GLuint>& values,
                      GLint srcOffset = 0) override;
  void clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) override;
  void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override;
  void clearDepth(GLclampf depth) override;
  void clearStencil(GLint stencil) override;
  void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) override;
  void compileShader(IGLShader* shader) override;
  void compressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                            GLsizei height, GLint border, const Uint8Array& pixels) override;
  void compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                               GLsizei width, GLsizei height, GLenum format,
                               GLsizeiptr size) override;
  void copyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y,
                      GLsizei width, GLsizei height, GLint border) override;
  void copyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y,
                         GLint width, GLint height) override;
  std::shared_ptr<IGLBuffer> createBuffer() override;
  IGLFramebufferPtr createFramebuffer() override;
  IGLProgramPtr createProgram() override;
  std::unique_ptr<IGLQuery> createQuery() override;
  IGLRenderbufferPtr createRenderbuffer() override;
  IGLShaderPtr createShader(GLenum type) override;
  IGLTexturePtr createTexture() override;
  IGLTransformFeedbackPtr createTransformFeedback() override;
  IGLVertexArrayObjectPtr createVertexArray() override;
  void cullFace(GLenum mode) override;
  void deleteBuffer(IGLBuffer* buffer) override;
  void deleteFramebuffer(IGLFramebuffer* framebuffer) override;
  void deleteProgram(IGLProgram* program) override;
  void deleteQuery(IGLQuery* query) override;
  void deleteRenderbuffer(IGLRenderbuffer* renderbuffer) override;
  void deleteShader(IGLShader* shader) override;
  void deleteTexture(IGLTexture* texture) override;
  void deleteTransformFeedback(IGLTransformFeedback* transformFeedback) override;
  void deleteVertexArray(IGLVertexArrayObject* vao) override;
  void depthFunc(GLenum func) override;
  void depthMask(GLboolean flag) override;
  void depthRange(GLclampf zNear, GLclampf zFar) override;
  void detachShader(IGLProgram* program, IGLShader* shader) override;
  void disable(GLenum cap) override;
  void disableVertexAttribArray(GLuint index) override;
  void drawArrays(GLenum mode, GLint first, GLint count) override;
  void drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount) override;
  void drawBuffers(const std::vector<GLenum>& buffers) override;
  void drawElements(GLenum mode, GLsizei count, GLenum type, GLintptr offset) override;
  void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLintptr offset,
                             GLsizei instanceCount) override;
  void enable(GLenum cap) override;
  void enableVertexAttribArray(GLuint index) override;
  void endQuery(GLenum target) override;
  void endTransformFeedback() override;
  void finish() override;
  void flush() override;
  void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget,
                               IGLRenderbuffer* renderbuffer) override;
  void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, IGLTexture* texture,
                            GLint level) override;
  void framebufferTextureLayer(GLenum target, GLenum attachment, IGLTexture* texture, GLint level,
                               GLint layer) override;
  void framebufferTextureMultiviewOVR(GLenum target, GLenum attachment, IGLTexture* texture,
                                      GLint level, GLint baseViewIndex, GLint numViews) override;
  void frontFace(GLenum mode) override;
  void generateMipmap(GLenum target) override;
  std::vector<IGLShader*> getAttachedShaders(IGLProgram* program) override;
  GLint getAttribLocation(IGLProgram* program, const std::string& name) override;
  GL::any getExtension(const std::string& name) override;
  GLboolean hasExtension(const std::string& extension) override;
  std::array<int, 3> getScissorBoxParameter() override; // GL::SCISSOR_BOX
  GLint getParameteri(GLenum pname) override;
  GLfloat getParameterf(GLenum pname) override;
  GLboolean getQueryParameterb(IGLQuery* query, GLenum pname) override;
  GLuint getQueryParameteri(IGLQuery* query, GLenum pname) override;
  std::string getString(GLenum pname) override;
  GLint getTexParameteri(GLenum pname) override;
  GLfloat getTexParameterf(GLenum pname) override;
  GLenum getError() override;
  const char* getErrorString(GLenum err) override;
  GLint getProgramParameter(IGLProgram* program, GLenum pname) override;
  std::string getProgramInfoLog(IGLProgram* program) override;
  GLint getRenderbufferParameter(GLenum target, GLenum pname) override;
  std::string getShaderInfoLog(IGLShader* shader) override;
  GLint getShaderParameter(IGLShader* shader, GLenum pname) override;
  IGLShaderPrecisionFormat* getShaderPrecisionFormat(GLenum shadertype,
                                                     GLenum precisiontype) override;
  std::string getShaderSource(IGLShader* shader) override;
  GLuint getUniformBlockIndex(IGLProgram* program, const std::string& uniformBlockName) override;
  std::unique_ptr<IGLUniformLocation> getUniformLocation(IGLProgram* program,
                                                         const std::string& name) override;
  void hint(GLenum target, GLenum mode) override;
  GLboolean isBuffer(IGLBuffer* buffer) override;
  GLboolean isEnabled(GLenum cap) override;
  GLboolean isFramebuffer(IGLFramebuffer* framebuffer) override;
  GLboolean isProgram(IGLProgram* program) override;
  GLboolean isRenderbuffer(IGLRenderbuffer* renderbuffer) override;
  GLboolean isShader(IGLShader* shader) override;
  GLboolean isTexture(IGLTexture* texture) override;
  void lineWidth(GLfloat width) override;
  bool linkProgram(IGLProgram* program) override;
  void pixelStorei(GLenum pname, GLint param) override;
  void polygonOffset(GLfloat factor, GLfloat units) override;
  void readBuffer(GLenum src) override;
  void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                  Float32Array& pixels) override;
  void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                  Uint8Array& pixels) override;
  void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width,
                           GLsizei height) override;
  void renderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalFormat,
                                      GLsizei width, GLsizei height) override;
  void sampleCoverage(GLclampf value, GLboolean invert) override;
  void scissor(GLint x, GLint y, GLsizei width, GLsizei height) override;
  void shaderSource(IGLShader* shader, const std::string& source) override;
  void stencilFunc(GLenum func, GLint ref, GLuint mask) override;
  void stencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) override;
  void stencilMask(GLuint mask) override;
  void stencilMaskSeparate(GLenum face, GLuint mask) override;
  void stencilOp(GLenum fail, GLenum zfail, GLenum zpass) override;
  void stencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass) override;
  void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                  GLint border, GLenum format, GLenum type,
                  const Uint8Array* const pixels) override;
  void texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                  GLsizei depth, GLint border, GLenum format, GLenum type,
                  const Uint8Array& pixels) override;
  void texParameterf(GLenum target, GLenum pname, GLfloat param) override;
  void texParameteri(GLenum target, GLenum pname, GLint param) override;
  void texStorage3D(GLenum target, GLint levels, GLenum internalformat, GLsizei width,
                    GLsizei height, GLsizei depth) override;
  void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                     GLsizei height, GLenum format, GLenum type, const Uint8Array& pixels) override;
  void transformFeedbackVaryings(IGLProgram* program, const std::vector<std::string>& varyings,
                                 GLenum bufferMode) override;
  void uniform1f(IGLUniformLocation* location, GLfloat v0) override;
  void uniform1fv(GL::IGLUniformLocation* location, const Float32Array& array) override;
  void uniform1i(IGLUniformLocation* location, GLint v0) override;
  void uniform1iv(IGLUniformLocation* location, const Int32Array& v) override;
  void uniform2f(IGLUniformLocation* location, GLfloat v0, GLfloat v1) override;
  void uniform2fv(IGLUniformLocation* location, const Float32Array& v) override;
  void uniform2i(IGLUniformLocation* location, GLint v0, GLint v1) override;
  void uniform2iv(IGLUniformLocation* location, const Int32Array& v) override;
  void uniform3f(IGLUniformLocation* location, GLfloat v0, GLfloat v1, GLfloat v2) override;
  void uniform3fv(IGLUniformLocation* location, const Float32Array& v) override;
  void uniform3i(IGLUniformLocation* location, GLint v0, GLint v1, GLint v2) override;
  void uniform3iv(IGLUniformLocation* location, const Int32Array& v) override;
  void uniform4f(IGLUniformLocation* location, GLfloat v0, GLfloat v1, GLfloat v2,
                 GLfloat v3) override;
  void uniform4fv(IGLUniformLocation* location, const Float32Array& v) override;
  void uniform4i(IGLUniformLocation* location, GLint v0, GLint v1, GLint v2, GLint v3) override;
  void uniform4iv(IGLUniformLocation* location, const Int32Array& v) override;
  void uniformBlockBinding(IGLProgram* program, GLuint uniformBlockIndex,
                           GLuint uniformBlockBinding) override;
  void uniformMatrix2fv(IGLUniformLocation* location, GLboolean transpose,
                        const Float32Array& value) override;
  void uniformMatrix3fv(IGLUniformLocation* location, GLboolean transpose,
                        const Float32Array& value) override;
  void uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                        const Float32Array& value) override;
  void uniformMatrix4fv(IGLUniformLocation* location, GLboolean transpose,
                        const std::array<float, 16>& value) override;
  void useProgram(IGLProgram* program) override;
  void validateProgram(IGLProgram* program) override;
  void vertexAttrib1f(GLuint index, GLfloat v0) override;
  void vertexAttrib1fv(GLuint indx, Float32Array& values) override;
  void vertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1) override;
  void vertexAttrib2fv(GLuint index, Float32Array& values) override;
  void vertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) override;
  void vertexAttrib3fv(GLuint index, Float32Array& values) override;
  void vertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) override;
  void vertexAttrib4fv(GLuint index, Float32Array& values) override;
  void vertexAttribDivisor(GLuint index, GLuint divisor) override;
  void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
                           GLint stride, GLintptr offset) override;
  void viewport(GLint x, GLint y, GLsizei width, GLsizei height) override;

private:
  std::unordered_map<std::string, GLenum> EnumMap;

}; // end of class GLRenderingContext

} // end of namespace GL
} // end of namespace BABYLON

#endif // end of BABYLON_IMPL_GL_RENDERING_CONTEXT_H
