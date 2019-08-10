#pragma once

namespace gl_ivs_utils
{
  class FrameBuffer
  {
  public:
    FrameBuffer(int width, int height);

    void Bind();
    void UnBind();
    unsigned int TextureId();

  private:
    unsigned int mFrameBuffer_Id;
    unsigned int mTextureColorBufferId;

  };
} // namespace gl_ivs_utils