#include "imgui_sdl_emscripten_runner.h"

// Adapted from external/imgui/examples/example_emscripten/main.cpp
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

#include <stdexcept>

namespace ImGuiUtils
{
namespace ImGuiRunner
{

  struct MySdlContext
  {
    SDL_Window* window          = nullptr;
    SDL_GLContext glContext     = nullptr;
  };

  struct MyAppContext
  {
    MySdlContext sdlContext;
    GuiFunctionWithExit guiFunction;
    AppWindowParams appWindowParams;
    PostInitFunction postInitFunction;
  };

  MyAppContext gAppContext;

  void main_loop(void*);

  void run_infinite_loop_inside_emscripten(MyAppContext & appContext)
  {
    auto & sdlContext = appContext.sdlContext;

    //gGuiFunction = f;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
      printf("Error: %s\n", SDL_GetError());
      throw std::runtime_error("run_app");
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
    sdlContext.window = SDL_CreateWindow("Dear ImGui Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    sdlContext.glContext = SDL_GL_CreateContext(sdlContext.window);
    if (!sdlContext.glContext)
    {
      fprintf(stderr, "Failed to initialize WebGL context!\n");
      throw std::runtime_error("Failed to initialize WebGL context!");
    }
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    const char* glsl_version = "#version 100";
    //const char* glsl_version = "#version 300 es";

    ImGui_ImplSDL2_InitForOpenGL(sdlContext.window, sdlContext.glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Babylon specific
    const auto & appWindowParams = appContext.appWindowParams;
    if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
      ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;
    if (appWindowParams.LoadFontAwesome)
      details::LoadFontAwesome();

    emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
  }


  void gui_loop_babylon_specific(MyAppContext & appContext)
  {
    MySdlContext & sdlContext = appContext.sdlContext;
    const auto & appWindowParams = appContext.appWindowParams;
    static bool postInited = false;
    if (!postInited)
    {
      appContext.postInitFunction();
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
  }

  void main_loop(void* arg)
  {
    (void)arg;

    // Babylon specific
    gui_loop_babylon_specific(gAppContext);
    MyAppContext & appContext = gAppContext;
    auto & sdlContext = gAppContext.sdlContext;

    ImGuiIO& io = ImGui::GetIO();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdlContext.window);
    ImGui::NewFrame();

    static bool show_demo_window = true;
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    bool shouldExit = gAppContext.guiFunction();
    if (appContext.appWindowParams.DefaultWindowType != DefaultWindowTypeOption::NoDefaultWindow)
      ImGui::End();
    ImVec4 clear_color = appContext.appWindowParams.ClearColor;

    // Rendering
    ImGui::Render();

    SDL_GL_MakeCurrent( sdlContext.window, sdlContext.glContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdlContext.window);
  }

  void RunGui_Emscripten(
    GuiFunctionWithExit guiFunction,
    AppWindowParams appWindowParams,
    PostInitFunction postInitFunction
  )
  {
    MyAppContext & appContext = gAppContext;
    appContext.guiFunction = guiFunction;
    appContext.appWindowParams = appWindowParams;
    appContext.postInitFunction = postInitFunction;
    run_infinite_loop_inside_emscripten(appContext);
  }

} // namespace ImGuiRunner
} // namespace ImGuiUtils
