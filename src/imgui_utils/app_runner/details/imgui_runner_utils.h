#ifndef BABYLONCPP_IMGUI_RUNNER_UTILS_H
#define BABYLONCPP_IMGUI_RUNNER_UTILS_H

#include <glad/glad.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <string>
namespace ImGuiUtils
{
namespace ImGuiRunner
{
namespace details
{

  std::string SelectGlslVersion();
  void ResetDockLayout();
  std::pair<int, int> MainScreenResolution();

  void LoadFontAwesome();
  void pushFontAwesome();
  void popFontAwesome();
  void ImplProvideFullScreenWindow(const AppWindowParams& appWindowParams);
  ImGuiID MainDockSpaceId();
  void ImplProvideFullScreenDockSpace(const AppWindowParams& appWindowParams);

  bool WasDockLayoutDone();
  void SetDockLayout_Done();
  void SetDockLayout_NotDone();

} // namespace details
} // namespace ImGuiRunner
} // namespace ImGuiUtils


#endif // BABYLONCPP_IMGUI_RUNNER_UTILS_H
