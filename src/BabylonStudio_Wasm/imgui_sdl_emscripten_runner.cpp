#include "imgui_sdl_emscripten_runner.h"

// Adapted from external/imgui/examples/example_emscripten/main.cpp
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
//#include <SDL_opengles2.h>
#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <stdexcept>

void Glad_Sdl_Init()
{
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    throw std::runtime_error("gladLoadGLLoader: Failed");
  if (!GLAD_GL_VERSION_3_3)
    throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));
}


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

  void main_loop_emscripten(void*);
  bool main_loop();


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

    // Decide GL+GLSL versions
#ifdef __EMSCRIPTEN__
    //const char* glsl_version = "#version 100";
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#elif __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif


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

#ifndef __EMSCRIPTEN__
    printf("Calling Glad_Sdl_Init()\n");
    Glad_Sdl_Init();
    printf("After Calling Glad_Sdl_Init()\n");
    auto str = reinterpret_cast<const char*>(glGetString(7938));
#endif

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


    ImGui_ImplSDL2_InitForOpenGL(sdlContext.window, sdlContext.glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
    const char* str22 = reinterpret_cast<const char*>(glGetString(7938));

    // Babylon specific
    const auto & appWindowParams = appContext.appWindowParams;
    if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
      ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;
    if (appWindowParams.LoadFontAwesome)
      details::LoadFontAwesome();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
#else
    bool done = false;
    while (!done)
      done = main_loop();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(sdlContext.glContext);
    SDL_DestroyWindow(sdlContext.window);
    SDL_Quit();
#endif
  }


  void gui_loop_babylon_specific(MyAppContext & appContext)
  {
    auto str = glGetString(7938);
    //MySdlContext & sdlContext = appContext.sdlContext;
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


  bool main_loop()
  {

    // Babylon specific
    gui_loop_babylon_specific(gAppContext);
    MyAppContext & appContext = gAppContext;
    auto & sdlContext = gAppContext.sdlContext;

    ImGuiIO& io = ImGui::GetIO();

    SDL_Event event;
    bool shouldExit = false;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
      if (event.type == SDL_QUIT)
        shouldExit = true;
      if (event.type == SDL_WINDOWEVENT
          && event.window.event == SDL_WINDOWEVENT_CLOSE
          && event.window.windowID == SDL_GetWindowID(sdlContext.window))
        shouldExit = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdlContext.window);
    ImGui::NewFrame();

    static bool show_demo_window = true;
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    shouldExit = shouldExit || gAppContext.guiFunction();
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

    return shouldExit;
  }

  void main_loop_emscripten(void*)
  {
    bool shouldExit = main_loop();
    (void)shouldExit;
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
