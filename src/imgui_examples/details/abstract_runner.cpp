#include "imgui_examples/details/abstract_runner.h"
#include <glad/glad.h>
#include <imgui.h>

namespace ImGui {
namespace ImGuiRunner {

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
void AbstractRunner::LoadFonts()
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

//void AbstractRunner::ShowGui()
//{
//  static bool show_demo_window = true;
//  if (show_demo_window)
//    ImGui::ShowDemoWindow(&show_demo_window);
//}

void AbstractRunner::ImGuiRender()
{
  auto & io = ImGui::GetIO();
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImVec4 clear_color = ClearColor();
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
ImVec4 AbstractRunner::ClearColor()
{
  return ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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

  while(!ExitRequired())
  {
    PollEvents();

    NewFrame_OpenGl();
    NewFrame_Backend();
    ImGui::NewFrame();

    ShowGui();

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
