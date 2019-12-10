#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ImGuiUtils
{
namespace GlfwRunner
{

  void glfw_error_callback(int error, const char* description);

  void InitGlfw_Hints();

  struct GLFWWindowParams
  {
    bool FullScreen = false;
    int Width = 1280;
    int Height = 720;
    std::string Title = "My Window";
    GLFWwindow *ParentWindow = nullptr;
  };

  GLFWwindow* GLFW_CreateWindow(const GLFWWindowParams & params);

  void GLFW_Cleanup(GLFWwindow* window);

  void GLFW_Init();

} // namespace GlfwRunner
} // namespace ImGuiUtils
