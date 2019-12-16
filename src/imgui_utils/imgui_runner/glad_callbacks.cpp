#include "imgui_utils/imgui_runner/glad_callbacks.h"
#include <cstdio>
#ifdef _WIN32
#include <windows.h>
#endif
#include <glad/glad.h>
#include <string>
#include <sstream>

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
#ifndef __EMSCRIPTEN__
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW";
      break;
#endif
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
  }
  return error;
}


void glad_post_call_callback(const char* name, void* /*funcptr*/,
                             int /*len_args*/, ...)
{
  GLenum error_code;
  error_code = glGetError();

  std::stringstream msg_str;
  msg_str << "ERROR " << GlErrorCodeStr(error_code) << "(" << error_code
          << ") in " << name << "\n";
  if (error_code != GL_NO_ERROR) {
    fprintf(stderr, "%s", msg_str.str().c_str());
#ifdef _MSC_VER
    OutputDebugString(msg_str.str().c_str());
#endif
  }
}

void glad_pre_call_callback(const char* name, void* /*funcptr*/,
                            int /*len_args*/, ...)
{
  std::stringstream msg_str;
  msg_str << "glad_pre_call_callback " << name << "\n";
  fprintf(stderr, "%s", msg_str.str().c_str());
#ifdef _MSC_VER
  OutputDebugString(msg_str.str().c_str());
#endif
}
