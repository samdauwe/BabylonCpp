// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_utils/app_runner/details/glfw_runner.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>

#include <utility>

#if defined(__APPLE__)

std::pair<int, int> MainScreenResolution()
{
  return { 1280, 720 };
}

#elif defined(_WIN32)

#undef APIENTRY
#include <windows.h>
std::pair<int, int> MainScreenResolution()
{
  return {
    (int)GetSystemMetrics(SM_CXSCREEN),
    (int)GetSystemMetrics(SM_CYSCREEN)
  };
}
#elif defined(__linux__)

#include <X11/Xlib.h>

std::pair<int, int> MainScreenResolution()
{
  Display* d = XOpenDisplay(NULL);
  Screen* s  = DefaultScreenOfDisplay(d);
  return { s->width, s->height };
}
#endif


namespace ImGuiUtils
{
  namespace ImGuiRunner
  {
    static ImFont* _fontRegular = nullptr;
    static ImFont* _fontSolid = nullptr;

    void LoadFontAwesome()
    {
      // Loads fonts
      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->AddFontDefault();
      static ImWchar ranges[] = { 0xf000, 0xf82f, 0 };
      ImFontConfig config;
      config.MergeMode = true;
      auto fontRegularPath
        = std::string("../assets/fonts/") + FONT_ICON_FILE_NAME_FAR;
      _fontRegular = io.Fonts->AddFontFromFileTTF(fontRegularPath.c_str(),
        ImGui::IconSize, &config, ranges);
      auto fontSolidPath
        = std::string("../assets/fonts/") + FONT_ICON_FILE_NAME_FAS;
      _fontSolid = io.Fonts->AddFontFromFileTTF(fontSolidPath.c_str(),
        ImGui::IconSize, &config, ranges);
    }

    void pushFontAwesome()
    {
      ImGui::PushFont(_fontRegular);
      ImGui::PushFont(_fontSolid);
    }

    void popFontAwesome()
    {
      ImGui::PopFont();
      ImGui::PopFont();
    }

    void ImplProvideFullScreenWindow(const AppWindowParams& appWindowParams)
    {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImVec2 winSize = ImGui::GetIO().DisplaySize;
      // winSize.y -= 10.f;
      ImGui::SetNextWindowSize(winSize);
      ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
                                     | ImGuiWindowFlags_NoScrollWithMouse
                                     | ImGuiWindowFlags_NoBringToFrontOnFocus;
      if (appWindowParams.ShowMenuBar)
        windowFlags |= ImGuiWindowFlags_MenuBar;
      ImGui::Begin("Main window (title bar invisible)", nullptr, windowFlags);
    }

    ImGuiID MainDockSpaceId()
    {
      static ImGuiID id = ImGui::GetID("MainDockSpace");
      return id;
    }

    void ImplProvideFullScreenDockSpace(const AppWindowParams& appWindowParams)
    {
      ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::SetNextWindowBgAlpha(0.0f);

      ImGuiWindowFlags window_flags
        = ImGuiWindowFlags_NoDocking;
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                      | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      window_flags
        |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
      if (appWindowParams.ShowMenuBar)
        window_flags |= ImGuiWindowFlags_MenuBar;

      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      static bool p_open = true;
      ImGui::Begin(appWindowParams.Title.c_str(), &p_open, window_flags);
      ImGui::PopStyleVar(3);

      ImGuiID dockspace_id = MainDockSpaceId();
      ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode; // ImGuiDockNodeFlags_PassthruDockspace;
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
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
      ImGui_ImplOpenGL3_Init(GlfwRunner::SelectGlslVersion().c_str());

      // Load Fonts
      // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
      // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
      // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
      // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
      // - Read 'misc/fonts/README.txt' for more instructions and details.
      // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
      //io.Fonts->AddFontDefault();
      //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
      //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
      //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
      //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
      //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
      //IM_ASSERT(font != NULL);

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


    bool gInitialDockLayoutFunction_WasCalled = false;

    void ResetDockLayout()
    {
      gInitialDockLayoutFunction_WasCalled = false;
      remove("imgui.ini");
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
        auto mainScreenSize = MainScreenResolution();
        mainScreenSize.second -= appWindowParams.WindowedFullScreen_HeightReduce;
        glfwWindowParams.Width = mainScreenSize.first;
        glfwWindowParams.Height = mainScreenSize.second;
      }


      GLFWwindow* window = GlfwRunner::GLFW_CreateWindow(glfwWindowParams);
      if (appWindowParams.WindowedFullScreen)
      {
        glfwSetWindowPos(window, 0, appWindowParams.WindowedFullScreen_HeightReduce / 2);
      }

      GlfwRunner::Glad_Init();
      ImGui_Init(window);

      if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
        ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;

      if (appWindowParams.LoadFontAwesome)
        LoadFontAwesome();


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
          ImplProvideFullScreenWindow(appWindowParams);

        if (appWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
        {
          if (!gInitialDockLayoutFunction_WasCalled)
          {
            if (appWindowParams.InitialDockLayoutFunction)
              appWindowParams.InitialDockLayoutFunction(MainDockSpaceId());
            gInitialDockLayoutFunction_WasCalled = true;
          }
          ImplProvideFullScreenDockSpace(appWindowParams);
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


    } // namespace ImGuiRunner
    } // namespace ImGuiUtils
