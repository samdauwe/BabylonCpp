#ifdef BABYLON_IMGUI__WITH_GLFW
#include <imgui.h>
#include <glad/glad.h>
#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

#include <imgui_impl_glfw.h>
#include <imgui_utils/app_runner/details/glfw_runner.h>

#include "imgui_impl_opengl3.h"
#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>

#include <utility>

namespace ImGuiUtils
{
namespace ImGuiRunner
{

void Glad_Init()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("gladLoadGLLoader: Failed");
  if (!GLAD_GL_VERSION_3_3)
    throw(std::runtime_error("GLAD could not initialize OpenGl 3.3"));
}


  void ImGui_Init(GLFWwindow* window)
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(details::SelectGlslVersion().c_str());
    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
  }

  void ImGui_Cleanup()
  {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGui_PrepareNewFrame()
  {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGui_ImplRender(GLFWwindow* window, const ImVec4 &clear_color)
  {
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void RunGui(
    GuiFunctionWithExit guiFunction,
    AppWindowParams appWindowParams,
    PostInitFunction postInitFunction
  )
  {
    GlfwRunner::GLFW_Init();

    // Create GLFWwindow
    GlfwRunner::GLFWWindowParams glfwWindowParams;
    {
      glfwWindowParams.FullScreen = appWindowParams.FullScreen;
      glfwWindowParams.Height = appWindowParams.Height;
      glfwWindowParams.Width = appWindowParams.Width;
      glfwWindowParams.ParentWindow = appWindowParams.ParentWindow;
      glfwWindowParams.Title = appWindowParams.Title;
    }

    if (appWindowParams.WindowedFullScreen)
    {
      auto mainScreenSize = details::MainScreenResolution();
      mainScreenSize.second -= appWindowParams.WindowedFullScreen_HeightReduce;
      glfwWindowParams.Width = mainScreenSize.first;
      glfwWindowParams.Height = mainScreenSize.second;
    }


    GLFWwindow* window = GlfwRunner::GLFW_CreateWindow(glfwWindowParams);
    if (appWindowParams.WindowedFullScreen)
    {
      glfwSetWindowPos(window, 0, appWindowParams.WindowedFullScreen_HeightReduce / 2);
    }

#ifndef __EMSCRIPTEN__
    Glad_Init();
#endif

    ImGui_Init(window);

    if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
      ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;

    if (appWindowParams.LoadFontAwesome)
      details::LoadFontAwesome();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
      // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
      glfwPollEvents();

      ImGui_PrepareNewFrame();

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

      bool shouldExit = guiFunction();

      if (appWindowParams.DefaultWindowType != DefaultWindowTypeOption::NoDefaultWindow)
        ImGui::End();

      ImGui_ImplRender(window, appWindowParams.ClearColor);

      glfwSwapBuffers(window);

      if (shouldExit)
        break;
    }

    ImGui_Cleanup();
    GlfwRunner::GLFW_Cleanup(window);
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

#endif // #ifdef BABYLON_IMGUI__WITH_GLFW
