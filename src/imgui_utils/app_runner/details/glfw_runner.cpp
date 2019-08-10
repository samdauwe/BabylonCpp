#include <imgui_utils/app_runner/details/glfw_runner.h>

namespace imgui_utils
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

    std::string SelectGlslVersion()
    {
#if __APPLE__    
      const char* glsl_version = "#version 150"; // GL 3.2 + GLSL 150
#else    
      const char* glsl_version = "#version 130"; // GL 3.0 + GLSL 130
#endif
      return std::string(glsl_version);
    }

    GLFWwindow* GLFW_CreateWindow(const GLFWWindowParams & params)
    {
      GLFWmonitor* monitor = nullptr; // will be set only if full screen is required

      // Check if full screen mode is requested
      if (params.FullScreen)
      {
        monitor = glfwGetPrimaryMonitor();
        const auto videoMode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
        glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
        glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
        glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
        glfwWindowHint(GLFW_ALPHA_BITS, videoMode->redBits);
        glfwWindowHint(GLFW_DEPTH_BITS, videoMode->redBits * 4);
      }

      GLFWwindow* glfwWindow = glfwCreateWindow(params.Width, params.Height, params.Title.c_str(), nullptr, params.ParentWindow);

      // Confirm that GLFW window was created successfully
      if (!glfwWindow) {
        glfwTerminate();
        throw std::runtime_error("CreateGLFWWindow Failed");
      }

      // Make the window's context current
      glfwMakeContextCurrent(glfwWindow);

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

      return glfwWindow;
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

    void Glad_Init()
    {
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("gladLoadGLLoader: Failed");
      if (!GLAD_GL_VERSION_3_3)
        throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));
    }

  } // namespace GlfwRunner
} // namespace imgui_utils