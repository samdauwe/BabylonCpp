#include "glad/glad.h"
#include "SamplesRunner/impl/frame_buffer.h"
#include <stdexcept>

namespace gl_ivs_utils
{
    FrameBuffer::FrameBuffer(int width, int height)
    {
      glGenFramebuffers(1, &mFrameBuffer_Id);
      glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer_Id);
      // create a color attachment texture
      //unsigned int textureColorbuffer;
      glGenTextures(1, &mTextureColorBufferId);
      glBindTexture(GL_TEXTURE_2D, mTextureColorBufferId);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBufferId, 0);
      // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
      unsigned int rbo;
      glGenRenderbuffers(1, &rbo);
      glBindRenderbuffer(GL_RENDERBUFFER, rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
                                                                                                    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Bind()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer_Id);
    }

    void FrameBuffer::UnBind()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int FrameBuffer::TextureId()
    {
      return mTextureColorBufferId;
    }

} // namespace gl_ivs_utils