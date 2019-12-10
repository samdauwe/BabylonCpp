#ifdef BABYLON_IMGUI__WITH_SDL

#include <sstream>
#include <imgui.h>
#include <glad/glad.h>
#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>
#include <imgui_utils/app_runner/details/imgui_runner_utils.h>
#include <SDL.h>
#include <SDL_opengles2.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace ImGuiUtils
{
namespace ImGuiRunner
{

//#ifndef __EMSCRIPTEN__
#if 1
void Glad_Init()
{
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    throw std::runtime_error("gladLoadGLLoader: Failed");
  if (!GLAD_GL_VERSION_3_3)
    throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));
}
#else
void Glad_Init() {}
#endif

struct MySdlContext
{
  SDL_Window*     window = nullptr;
  SDL_GLContext   gLContext = nullptr;
};


MySdlContext SetupSdl(
  const std::string & windowTitle,
  int width,
  int height
  )
{
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    throw std::runtime_error("Error in SetupSdl");
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  MySdlContext result;
  result.window = SDL_CreateWindow(windowTitle.c_str() , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
  result.gLContext = SDL_GL_CreateContext(result.window);
  if (!result.gLContext)
  {
    fprintf(stderr, "Failed to initialize WebGL context!\n");
    throw std::runtime_error("SetupSdl: Failed to initialize WebGL context");
  }
  SDL_GL_SetSwapInterval(1); // Enable vsync

  return result;
}

void CleanupSdl(MySdlContext& context)
{
  SDL_DestroyWindow(context.window);
  SDL_Quit();
}

void ImGui_Init(MySdlContext& context)
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  std::string glslVersion = details::SelectGlslVersion();
  ImGui_ImplSDL2_InitForOpenGL(context.window , context.gLContext);
  ImGui_ImplOpenGL3_Init(glslVersion.c_str());

  ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
}

void ImGui_Cleanup()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void ImGui_PrepareNewFrame(MySdlContext& context)
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(context.window);
  ImGui::NewFrame();
}

void ImGui_ImplRender(const ImVec4 &clear_color)
{
  // Rendering
  ImGui::Render();
  glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool LoopProc(
  MySdlContext& mySdlContext,
  PostInitFunction postInitFunction,
  GuiFunctionWithExit guiFunction,
  const AppWindowParams& appWindowParams
  )
{
  bool shouldExit_CloseWindow = false;

  ImGuiIO& io = ImGui::GetIO();
  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
  // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (event.type == SDL_QUIT)
      shouldExit_CloseWindow = true;
    if (   event.type == SDL_WINDOWEVENT
        && event.window.event == SDL_WINDOWEVENT_CLOSE
        && event.window.windowID == SDL_GetWindowID(mySdlContext.window))
    {
      shouldExit_CloseWindow = true;
    }
    // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
  }

  ImGui_PrepareNewFrame(mySdlContext);

  static bool postInited = false;
  if (!postInited)
  {
    postInitFunction();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = appWindowParams.ConfigWindowsMoveFromTitleBarOnly;
    postInited = true;
  }

  if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenWindow)
    details::ImplProvideFullScreenWindow(appWindowParams);

  if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
  {
    if (!details::WasDockLayoutDone())
    {
      if (appWindowParams.InitialDockLayoutFunction)
        appWindowParams.InitialDockLayoutFunction(details::MainDockSpaceId());
      details::SetDockLayout_Done();
    }
    details::ImplProvideFullScreenDockSpace(appWindowParams);
  }

  bool shouldExit_Button = guiFunction();

  if (appWindowParams.DefaultWindowType != DefaultWindowTypeOption::NoDefaultWindow)
    ImGui::End();

  SDL_GL_MakeCurrent(mySdlContext.window, mySdlContext.gLContext);
  ImGui_ImplRender(appWindowParams.ClearColor);
  SDL_GL_SwapWindow(mySdlContext.window);

  return shouldExit_Button || shouldExit_CloseWindow;
}

void RunGui(
  GuiFunctionWithExit guiFunction,
  AppWindowParams appWindowParams,
  PostInitFunction postInitFunction
)
{
  if (appWindowParams.WindowedFullScreen)
  {
    auto mainScreenSize = details::MainScreenResolution();
    mainScreenSize.second -= appWindowParams.WindowedFullScreen_HeightReduce;
    appWindowParams.Width = mainScreenSize.first;
    appWindowParams.Height = mainScreenSize.second;
  }

  MySdlContext mySdlContext = SetupSdl(appWindowParams.Title, appWindowParams.Width, appWindowParams.Height);

#ifndef __EMSCRIPTEN__
  Glad_Init();
#endif

  ImGui_Init(mySdlContext);

  if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;

  if (appWindowParams.LoadFontAwesome)
    details::LoadFontAwesome();

  bool shouldExit = false;
  while (!shouldExit)
  {
    shouldExit = LoopProc(mySdlContext, postInitFunction, guiFunction, appWindowParams);
  }

  ImGui_Cleanup();
  CleanupSdl(mySdlContext);
}


void RunGui(
  GuiFunction guiFunction,
  AppWindowParams windowParams,
  PostInitFunction postInitFunction)
{
  GuiFunctionWithExit guiFunctionWithExit = [&]() -> bool {
    guiFunction();
    return false;
  };
  RunGui(guiFunctionWithExit, windowParams, postInitFunction);
}

void ResetDockLayout()
{
  details::SetDockLayout_NotDone();
}



} // namespace ImGuiRunner
} // namespace ImGuiUtils


#endif // #ifdef BABYLON_IMGUI__WITH_SDL