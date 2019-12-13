#include "imgui_examples/details/abstract_runner.h"
#include <glad/glad.h>
#include <imgui.h>

namespace ImGui {
namespace ImGuiRunner {

#if defined(_WIN32)
#undef APIENTRY
  #include <windows.h>
  ImVec2 MainScreenResolution()
  {
    return {
      (int)GetSystemMetrics(SM_CXSCREEN),
      (int)GetSystemMetrics(SM_CYSCREEN)
    };
  }
#elif defined(__linux__)
  #include <X11/Xlib.h>
  ImVec2 MainScreenResolution()
  {
    Display* d = XOpenDisplay(NULL);
    Screen* s  = DefaultScreenOfDisplay(d);
    return { s->width, s->height };
  }
#else
ImVec2 MainScreenResolution()
{
  return { 1280.f, 720.f };
}
#endif

bool ExampleGui()
{
  // Our state
  static bool show_demo_window = true;
  static bool show_another_window = false;
  //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  bool exitRequired = false;

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//    if (ImGui::ColorEdit3("clear color", (float*)&clear_color)) // Edit 3 floats representing a color
//      SetClearColor(clear_color);

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::Button("Quit"))
      exitRequired = true;

    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window)
  {
    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
      show_another_window = false;
    ImGui::End();
  }

  return exitRequired;
}

void AbstractRunner::SetupImgGuiContext()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

}
void AbstractRunner::SetupImGuiStyle()
{
  auto & io = ImGui::GetIO();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

}

void LoadNoAdditionalFont()
{
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

void AbstractRunner::ImGuiRender()
{
  auto & io = ImGui::GetIO();
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImVec4 clear_color = mClearColor;
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AbstractRunner::RunIt()
{
  InitBackend();
  Select_Gl_Version();
  CreateWindowAndContext();
  InitGlLoader();
  SetupImgGuiContext();
  SetupImGuiStyle();
  SetupPlatformRendererBindings();
  LoadFonts();

  while(!mExitRequired)
  {
    PollEvents();

    NewFrame_OpenGl();
    NewFrame_Backend();
    ImGui::NewFrame();

    bool exitRequired = ShowGui();
    if (exitRequired)
      mExitRequired = true;

    ImGuiRender();
    RenderDrawData_To_OpenGl();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      UpdateAndRenderAdditionalPlatformWindows();

    SwapBuffers();
  }

  Cleanup();
}

} // namespace ImGuiRunner
} // namespace ImGui
