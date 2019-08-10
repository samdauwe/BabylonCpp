// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_utils/app_runner/details/glfw_runner.h>
#include <imgui_utils/app_runner/imgui_runner.h>

namespace imgui_utils
{
  namespace ImGuiRunner
  {

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

    void ImGui_ImplRender(GLFWwindow* window, ImVec4 &clear_color)
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

    void RunGui(GuiFunction guiFunction)
    {
      GlfwRunner::GLFW_Init();
      // Create GLFWwindow
      GlfwRunner::GLFWWindowParams glfwWindowParams;
      GLFWwindow* window = GlfwRunner::GLFW_CreateWindow(glfwWindowParams);
      GlfwRunner::Glad_Init();
      ImGui_Init(window);

      ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        guiFunction();

        ImGui_ImplRender(window, clear_color);

        glfwSwapBuffers(window);
      }

      ImGui_Cleanup();
      GlfwRunner::GLFW_Cleanup(window);
    }


  } // namespace ImGuiRunner
} // namespace imgui_utils


