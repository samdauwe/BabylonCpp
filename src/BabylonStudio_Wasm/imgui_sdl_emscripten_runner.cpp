#include "imgui_sdl_emscripten_runner.h"

// Adapted from external/imgui/examples/example_emscripten/main.cpp
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>

#include <stdexcept>

// Emscripten requires to have full control over the main loop. We're going to store our SDL book-keeping variables globally.
// Having a single function that acts as a loop prevents us to store state in the stack of said function. So we need some location for this.
SDL_Window*     g_Window = NULL;
SDL_GLContext   g_GLContext = NULL;

GuiFunction gGuiFunction;

void main_loop(void*);

void run_app(GuiFunction f)
{
  gGuiFunction = f;

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
  g_Window = SDL_CreateWindow("Dear ImGui Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
  g_GLContext = SDL_GL_CreateContext(g_Window);
  if (!g_GLContext)
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

  ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
  ImGui_ImplOpenGL3_Init(glsl_version);

  emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
}


void main_loop(void* arg)
{
  ImGuiIO& io = ImGui::GetIO();
  IM_UNUSED(arg); // We can pass this argument as the second parameter of emscripten_set_main_loop_arg(), but we don't use that.

  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);
    // Capture events here, based on io.WantCaptureMouse and io.WantCaptureKeyboard
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(g_Window);
  ImGui::NewFrame();

  static bool show_demo_window = true;
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  gGuiFunction();

  // Rendering
  ImGui::Render();

  SDL_GL_MakeCurrent(g_Window, g_GLContext);
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(g_Window);
}
