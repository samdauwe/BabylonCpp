#include <babylon/GL/framebuffer_canvas.h>
#include <babylon/GL/gl_rendering_context.h>
#include <glad/glad.h>

#include <stdexcept>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image/stb_image_resize.h>

namespace BABYLON {
namespace GL {

FramebufferCanvas::FramebufferCanvas()
    : mFrameBuffer{nullptr}, mTextureColorBuffer{nullptr}, mRenderbuffer{nullptr}
{
  _renderingContext = std::make_unique<GL::GLRenderingContext>();
  _renderingContext->initialize();
  _boundingClientRect.bottom = clientHeight;
  _boundingClientRect.height = clientHeight;
  _boundingClientRect.left   = 0;
  _boundingClientRect.right  = clientWidth;
  _boundingClientRect.top    = 0;
  _boundingClientRect.width  = clientWidth;
}

FramebufferCanvas::~FramebufferCanvas()
{
  _renderingContext->deleteFramebuffer(mFrameBuffer.get());
  _renderingContext->deleteTexture(mTextureColorBuffer.get());
  _renderingContext->deleteRenderbuffer(mRenderbuffer.get());
}

void FramebufferCanvas::initializeFrameBuffer()
{
  _renderingContext->drawingBufferWidth  = clientWidth;
  _renderingContext->drawingBufferHeight = clientHeight;

  // Create a frame buffer
  mFrameBuffer = _renderingContext->createFramebuffer();
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());

  // Create a color attachment texture
  mTextureColorBuffer = _renderingContext->createTexture();
  _renderingContext->bindTexture(GL_TEXTURE_2D, mTextureColorBuffer.get());
  _renderingContext->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth, clientHeight, 0, GL_RGB,
                                GL_UNSIGNED_BYTE, nullptr);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Create a renderbuffer object for depth and stencil attachment
  mRenderbuffer = _renderingContext->createRenderbuffer();
  _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer.get());
  _renderingContext->renderbufferStorage(GL_RENDERBUFFER, DEPTH24_STENCIL8, clientWidth,
                                         clientHeight);
  _renderingContext->framebufferRenderbuffer(GL_FRAMEBUFFER, DEPTH_STENCIL_ATTACHMENT,
                                             GL_RENDERBUFFER, mRenderbuffer.get());

  // Set "renderedTexture" as our colour attachement #0
  _renderingContext->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                          mTextureColorBuffer.get(), 0);

  // Set the list of draw buffers.
  std::vector<GLenum> drawBuffers{GL_COLOR_ATTACHMENT0};
  _renderingContext->drawBuffers(drawBuffers); // "1" is the size of DrawBuffers

  if (_renderingContext->checkFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
  }

  _renderingContext->bindTexture(GL_TEXTURE_2D, nullptr);
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
}

void FramebufferCanvas::resize(int iWidth, int iHeight)
{
  if (clientWidth != iWidth || clientHeight != iHeight) {
    clientWidth                            = iWidth;
    clientHeight                           = iHeight;
    _renderingContext->drawingBufferWidth  = iWidth;
    _renderingContext->drawingBufferHeight = iHeight;

    // Resize texture
    _renderingContext->activeTexture(GL_TEXTURE0);
    _renderingContext->bindTexture(GL_TEXTURE_2D, mTextureColorBuffer.get());
    _renderingContext->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth, clientHeight, 0, GL_RGB,
                                  GL_UNSIGNED_BYTE, nullptr);

    // Bind the frame buffer
    _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());

    // Resize renderbuffer
    _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer.get());
    _renderingContext->renderbufferStorage(GL_RENDERBUFFER, DEPTH24_STENCIL8, clientWidth,
                                           clientHeight);

    // Set "renderedTexture" as our colour attachement #0
    _renderingContext->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                            mTextureColorBuffer.get(), 0);

    // Unbind the buffers and texture
    _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
    _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, nullptr);
    _renderingContext->bindTexture(GL_TEXTURE_2D, nullptr);
  }
}

bool FramebufferCanvas::initializeContext3d()
{
  if (!_initialized) {
    _initialized = _renderingContext->initialize();
  }

  return _initialized;
}

ClientRect& FramebufferCanvas::getBoundingClientRect()
{
  return _boundingClientRect;
}

ICanvasRenderingContext2D* FramebufferCanvas::getContext2d()
{
  return nullptr;
}

GL::IGLRenderingContext* FramebufferCanvas::getContext3d(const EngineOptions& /*options*/)
{
  return _renderingContext.get();
}

void FramebufferCanvas::bind()
{
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());
}

void FramebufferCanvas::unbind()
{
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
}

unsigned int FramebufferCanvas::textureId()
{
  return mTextureColorBuffer->value;
  // return mFrameBuffer->value;
}

Uint8Array reverse_pixels_rows(const Uint8Array& pixels, int width, int height, int nbChannels)
{
  Uint8Array pixels_reverse_rows;
  pixels_reverse_rows.resize(width * height * 3, 0);
  for (int row = 0; row < height; row++) {
    const uint8_t* src = &(pixels[(height - 1 - row) * nbChannels * width]);
    uint8_t* dst       = &(pixels_reverse_rows[row * nbChannels * width]);
    for (int col = 0; col < width; col++) {
      for (int channel = 0; channel < nbChannels; channel++) {
        *dst++ = *src++;
      }
    }
  }
  return pixels_reverse_rows;
}

Uint8Array FramebufferCanvas::readPixelsRgb()
{
  Uint8Array pixels;
  pixels.resize(clientWidth * clientHeight * 3, 0);
  _renderingContext->readPixels(0, 0, clientWidth, clientHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  // rows are inversed in the framebuffer: invert them now
  auto pixels_reverse_rows = reverse_pixels_rows(pixels, clientWidth, clientHeight, 3);
  return pixels_reverse_rows;
}

void FramebufferCanvas::saveScreenshotJpg(const char* filename, int quality, int imageWidth)
{
  bind();
  auto pixels = readPixelsRgb();
  unbind();
  int nbChannels = 3;

  if (imageWidth == -1)
    stbi_write_jpg(filename, clientWidth, clientHeight, nbChannels, pixels.data(), quality);
  else {
    Uint8Array pixels_resized;
    int imageHeight = (int)((double)clientHeight * (double)imageWidth / (double)clientWidth + 0.5);
    pixels_resized.resize(imageWidth * imageHeight * 3);
    stbir_resize_uint8(pixels.data(), clientWidth, clientHeight, 0, pixels_resized.data(),
                       imageWidth, imageHeight, 0, 3);
    stbi_write_jpg(filename, imageWidth, imageHeight, nbChannels, pixels_resized.data(), quality);
  }
}

void FramebufferCanvas::saveScreenshotPng(const char* filename)
{
  bind();
  auto pixels = readPixelsRgb();
  unbind();
  int nbChannels      = 3;
  int stride_in_bytes = clientWidth * nbChannels;
  stbi_write_png(filename, clientWidth, clientHeight, nbChannels, pixels.data(), stride_in_bytes);
}

} // end of namespace GL
} // end of namespace BABYLON
