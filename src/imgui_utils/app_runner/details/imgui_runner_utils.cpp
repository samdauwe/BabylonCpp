//
// Created by Pascal Thomet on 10/12/2019.
//

#include <imgui.h>
#include <imgui_utils/imgui_utils.h>
#include "imgui_runner_utils.h"
#include "imgui_utils/icons_font_awesome_5.h"

namespace ImGuiUtils
{
namespace ImGuiRunner
{
namespace details
{

#if defined(__APPLE__) || defined(__EMSCRIPTEN__)

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


  std::string SelectGlslVersion()
  {
  #if __APPLE__
    const char* glsl_version = "#version 150"; // GL 3.2 + GLSL 150
  #elif defined(__EMSCRIPTEN__)
    // const char* glsl_version = "#version 100"; // ?!?
    const char* glsl_version = "#version 300 es";
  #else
    const char* glsl_version = "#version 130"; // GL 3.0 + GLSL 130
  #endif
    return std::string(glsl_version);
  }

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

  bool gWasDockLayoutDone = false;

  bool WasDockLayoutDone()
  {
    return gWasDockLayoutDone;
  }
  void SetDockLayout_Done()
  {
    gWasDockLayoutDone = true;
  }
  void SetDockLayout_NotDone()
  {
    remove("imgui.ini");
    gWasDockLayoutDone = false;
  }

} // namespace details
} // namespace ImGuiRunner
} // namespace ImGuiUtils
