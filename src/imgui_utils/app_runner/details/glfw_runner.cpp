#ifdef BABYLON_IMGUI__WITH_GLFW

#include <stdexcept>
#include <imgui_utils/app_runner/details/glfw_runner.h>

namespace ImGuiUtils
{
namespace GlfwRunner
{

  void glfw_error_callback(int error, const char* description)
  {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
  }

  void InitGlfw_Hints()
  {
    // Draw smooth line with antialias
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    // Initialize OpenGL context
#if __APPLE__
// GL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
// GL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
#endif
  }

  GLFWwindow* GLFW_CreateWindow(const GLFWWindowParams & params)
  {
    GLFWwindow* window = nullptr;

    // Check if full screen mode is requested
    if (params.FullScreen)
    {
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();

      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwWindowHint(GLFW_RED_BITS, mode->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
      glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
      window = glfwCreateWindow(mode->width, mode->height, "My Title", monitor, params.ParentWindow);
    }
    else
      window = glfwCreateWindow(params.Width, params.Height, params.Title.c_str(), nullptr, params.ParentWindow);

    // Confirm that GLFW window was created successfully
    if (!window) {
      glfwTerminate();
      throw std::runtime_error("CreateGLFWWindow Failed");
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Enable vsync
    glfwSwapInterval(1);

    // -- Setup callbacks --
    // Key input events
    //glfwSetKeyCallback(window.glfwWindow, GLFWKeyCallback);
    //// Cursor position
    //glfwSetCursorPosCallback(window.glfwWindow, GLFWCursorPositionCallback);
    //// Mouse button input
    //glfwSetMouseButtonCallback(window.glfwWindow, GLFWMouseButtonCallback);
    //// Scroll input
    //glfwSetScrollCallback(window.glfwWindow, GLFWScrollCallback);
    //// Window resize events
    //glfwSetWindowSizeCallback(window.glfwWindow, GLFWWindowSizeCallback);

    return window;
  }

  void GLFW_Cleanup(GLFWwindow* window)
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void GLFW_Init()
  {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
      throw std::runtime_error("InitGlfw failed");

    InitGlfw_Hints();
  }

} // namespace GlfwRunner
} // namespace ImGuiUtils

#endif // #ifndef BABYLON_IMGUI__WITH_GLFW
