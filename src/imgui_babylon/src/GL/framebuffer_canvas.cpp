#include <babylon/GL/framebuffer_canvas.h>
#include <babylon/GL/gl_rendering_context.h>
#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <babylon/utils/stb_image_write.h>

namespace BABYLON {
namespace impl {

FramebufferCanvas::FramebufferCanvas() : 
  ICanvas{}
{
  _renderingContext          = std::make_unique<GL::GLRenderingContext>();
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
}

void FramebufferCanvas::initializeFrameBuffer()
{
  _renderingContext->drawingBufferWidth = clientWidth;
  _renderingContext->drawingBufferHeight = clientHeight;
  
  //glGenFramebuffers(1, &mFrameBuffer_Id);
  mFrameBuffer = std::move(_renderingContext->createFramebuffer());
  //glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer_Id);
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());

  // create a color attachment texture
  mTextureColorBuffer = std::move(_renderingContext->createTexture());
  _renderingContext->bindTexture(GL_TEXTURE_2D, mTextureColorBuffer.get());
  _renderingContext->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth, clientHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  _renderingContext->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer.get(), 0);

  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  //unsigned int rbo;
  //glGenRenderbuffers(1, &rbo);
  auto rbo = _renderingContext->createRenderbuffer();
  _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, rbo);
  _renderingContext->renderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, clientWidth, clientHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
  // now actually attach it
  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
  //
  // OUCH : rbo may be dead here !!!
  //
  _renderingContext->framebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 

  if ( _renderingContext->checkFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, 0);
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

bool FramebufferCanvas::onlyRenderBoundingClientRect() const
{
  return false;
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
  //return mFrameBuffer->value;
}


Uint8Array reverse_pixels_rows(
  const Uint8Array & pixels,
  int width, 
  int height, 
  int nbChannels)
{
  Uint8Array pixels_reverse_rows;
  pixels_reverse_rows.resize(width * height * 3, 0);
  for (int row = 0; row < height; row++)
  {
    const uint8_t * src = &(pixels[(height - 1 - row) * nbChannels * width]);
    uint8_t * dst = &(pixels_reverse_rows[row * nbChannels * width]);
    for (int col = 0; col < width; col++)
    {
      for (int channel = 0; channel < nbChannels; channel++)
      {
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
  _renderingContext->readPixels(0, clientHeight, clientWidth, clientHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  // rows are inversed in the framebuffer: invert them now
  auto pixels_reverse_rows = reverse_pixels_rows(pixels, clientWidth, clientHeight, 3);
  return pixels_reverse_rows;
}

void FramebufferCanvas::saveScreenshotJpg(const char * filename, int quality)
{
  bind();
  auto pixels = readPixelsRgb();
  unbind();
  int nbChannels = 3;
  stbi_write_jpg(filename, clientWidth, clientHeight, nbChannels, pixels.data(), quality);
}

void FramebufferCanvas::saveScreenshotPng(const char * filename)
{
  bind();
  auto pixels = readPixelsRgb();
  unbind();
  int nbChannels = 3;
  int stride_in_bytes = clientWidth * nbChannels;
  stbi_write_png(filename, clientWidth, clientHeight, nbChannels, pixels.data(), stride_in_bytes);
}

} // end of namespace impl
} // end of namespace BABYLON
