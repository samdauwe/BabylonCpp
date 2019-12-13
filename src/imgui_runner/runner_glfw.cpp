#ifdef IMGUI_RUNNER_USE_GLFW
#include "runner_glfw.h"

#include "imgui_runner/runner_glfw.h"
#include <GLFW/glfw3.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <stdexcept>

// SDL + OpenGL3 + glad

namespace ImGui {
namespace ImGuiRunner {


static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void RunnerGlfw::InitBackend()
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    throw std::runtime_error("RunnerGlfw::InitBackend failed");
}

void RunnerGlfw::Select_Gl_Version()
{
  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
    //const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  //const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
}

std::string RunnerGlfw::GlslVersion()
{
#if __APPLE__
  // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
#else
  const char* glsl_version = "#version 130";
#endif
  return glsl_version;
}

void RunnerGlfw::CreateWindowAndContext()
{
  // Check if full screen mode is requested
  if (mBackendFullScreen)
  {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    mWindow = glfwCreateWindow(mode->width, mode->height, mBackendWindowTitle.c_str(), monitor, nullptr);
  }
  else
  {
    mWindow = glfwCreateWindow((int)mBackendWindowSize.x, (int)mBackendWindowSize.y, mBackendWindowTitle.c_str(), NULL, NULL);
  }
  if ( (mBackendWindowPosition.x >= 0.f ) && (mBackendWindowPosition.y >= 0.f))
    glfwSetWindowPos(mWindow, (int)mBackendWindowPosition.x , (int)mBackendWindowPosition.y);

  if (mWindow == NULL)
  {
    glfwTerminate();
    throw std::runtime_error("RunnerGlfw::CreateWindowAndContext failed");
  }
  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(1); // Enable vsync
}

void RunnerGlfw::InitGlLoader()
{
#ifndef __EMSCRIPTEN__
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("gladLoadGLLoader: Failed");
  if (!GLAD_GL_VERSION_3_3)
    throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));

//  // Initialize OpenGL loader
//#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
//  bool err = gl3wInit() != 0;
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
//  bool err = glewInit() != GLEW_OK;
//#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
//  bool err = gladLoadGL() == 0;
//#else
//  bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
//#endif
//  if (err)
//  {
//    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
//    return 1;
//  }
#endif // #ifndef __EMSCRIPTEN__
}

void RunnerGlfw::SetupPlatformRendererBindings()
{
  ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
  ImGui_ImplOpenGL3_Init(GlslVersion().c_str());
}

void RunnerGlfw::PollEvents()
{
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  glfwPollEvents();
  mExitRequired = glfwWindowShouldClose(mWindow);
}

void RunnerGlfw::NewFrame_OpenGl()
{
  ImGui_ImplOpenGL3_NewFrame();
}

void RunnerGlfw::NewFrame_Backend()
{
  ImGui_ImplGlfw_NewFrame();
}

void RunnerGlfw::RenderDrawData_To_OpenGl()
{
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RunnerGlfw::UpdateAndRenderAdditionalPlatformWindows()
{
  GLFWwindow* backup_current_context = glfwGetCurrentContext();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  glfwMakeContextCurrent(backup_current_context);
}

void RunnerGlfw::Cleanup()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

void RunnerGlfw::SwapBuffers()
{
  glfwSwapBuffers(mWindow);
}

} // namespace ImGuiRunner
} // namespace ImGui
#endif // #ifdef IMGUI_RUNNER_USE_GLFW
