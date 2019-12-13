#ifdef IMGUI_RUNNER_USE_SDL

#include <imgui_examples/details/runner_sdl.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>

#include <sstream>
#include <SDL.h>

namespace ImGui {
namespace ImGuiRunner {

void RunnerSdl::InitBackend()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    std::stringstream msg;
    msg << "RunnerSdl::InitBackend error " << SDL_GetError();
    throw std::runtime_error(msg.str().c_str());
  }
}

void RunnerSdl::Select_Gl_Version()
{
  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 Core + GLSL 150
  // const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  // const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
}

std::string RunnerSdl::GlslVersion()
{
#if __APPLE__
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
#else
  // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
#endif
    return glsl_version;
}

void RunnerSdl::CreateWindowAndContext()
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  mWindow = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  mGlContext = SDL_GL_CreateContext(mWindow);
  SDL_GL_MakeCurrent(mWindow, mGlContext);
  SDL_GL_SetSwapInterval(1); // Enable vsync
}

void RunnerSdl::InitGlLoader()
{
//  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    throw std::runtime_error("gladLoadGLLoader: Failed");
//  if (!GLAD_GL_VERSION_3_3)
//    throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));

  // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
  bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
  bool err = gladLoadGL() == 0;
#else
  bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
  if (err)
  {
    throw std::runtime_error("Failed to initialize OpenGL loader!");
  }

}
void RunnerSdl::SetupPlatformRendererBindings()
{
  ImGui_ImplSDL2_InitForOpenGL(mWindow, mGlContext);
  ImGui_ImplOpenGL3_Init(GlslVersion().c_str());
}

void RunnerSdl::PollEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT)
      mExitRequired = true;
    if (   event.type == SDL_WINDOWEVENT
        && event.window.event == SDL_WINDOWEVENT_CLOSE
        && event.window.windowID == SDL_GetWindowID(mWindow))
    {
      mExitRequired = true;
    }
  }
}

void RunnerSdl::NewFrame_OpenGl()
{
  ImGui_ImplOpenGL3_NewFrame();
}

void RunnerSdl::NewFrame_Backend()
{
  ImGui_ImplSDL2_NewFrame(mWindow);
}

void RunnerSdl::RenderDrawData_To_OpenGl()
{
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RunnerSdl::UpdateAndRenderAdditionalPlatformWindows()
{
  SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
  SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
}

void RunnerSdl::Cleanup()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(mGlContext);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

void RunnerSdl::SwapBuffers()
{
  SDL_GL_SwapWindow(mWindow);
}

} // namespace ImGuiRunner
} // namespace ImGui

#endif // #ifdef IMGUI_RUNNER_USE_SDL

