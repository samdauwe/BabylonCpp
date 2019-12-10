#include <glad/glad.h>
#include <map>
#include <imgui.h>

// Use stb_image.h to load a PNG from disk and turn it into raw RGBA pixel data:
#ifndef __EMSCRIPTEN__
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image/stb_image.h>
#include <babylon/core/filesystem.h>


namespace ImGuiUtils
{

  struct ImageFileTexture
  {
    int _width = 0, _height = 0;
    GLuint _textureId = 0;
    bool found = true;

    ImageFileTexture() = default;
    ImageFileTexture(const std::string &filename)
    {
      if (!BABYLON::Filesystem::exists(filename))
      {
        found = false;
        return;
      }

      unsigned char* my_image_data = stbi_load(filename.c_str(), &_width, &_height, nullptr, 4);

      // Turn the RGBA pixel data into an OpenGL texture:
      glGenTextures(1, &_textureId);
      glBindTexture(GL_TEXTURE_2D, _textureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, my_image_data);
      stbi_image_free(my_image_data);
    }
    
    ~ImageFileTexture() = default;
  };

  using Filename = std::string;

  void ImageFromFile(const Filename &filename, ImVec2 size /*= ImVec2(0.f, 0.f)*/)
  {
    static std::map<Filename, ImageFileTexture> cacheImageFileTexture;
    if (cacheImageFileTexture.find(filename) == cacheImageFileTexture.end())
      cacheImageFileTexture[filename] = std::move(ImageFileTexture(filename));

    const ImageFileTexture & texture = cacheImageFileTexture.at(filename);
    if (texture.found)
    {
      if ((size.x == 0.f) && (size.y == 0.f))
        size = ImVec2((float)texture._width, (float)texture._height);
      else if (size.x == 0.f)
        size = ImVec2((float)texture._width / (float)texture._height * size.y, size.y);
      else if (size.y == 0.f)
        size = ImVec2(size.x, (float)texture._height / (float)texture._width * size.x);

      ImGui::Image((void*)(intptr_t)texture._textureId, size);
    }
    else
    {
      ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 0.7f), "Image not found");
    }
  }

  } // namespace ImGuiUtils