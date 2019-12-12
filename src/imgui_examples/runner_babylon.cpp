#include "runner_babylon.h"

#include <glad/glad.h>
//#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

namespace ImGuiUtils
{
namespace ImGuiRunner2
{
void RunGui_WithExit( // type is ImGuiRunnerFunctionType
  GuiFunctionWithExit guiFunction,
  AppWindowParams appWindowParams,
  PostInitFunction postInitFunction
)
{
}
void RunGui(
  GuiFunction guiFunction,
  AppWindowParams appWindowParams,
  PostInitFunction postInitFunction
)
{
  GuiFunctionWithExit guiFunctionWithExit = [&]() -> bool {
    guiFunction();
    return false;
  };
  RunGui_WithExit(guiFunctionWithExit, appWindowParams, postInitFunction);
}

void ResetDockLayout()
{
  //details::SetDockLayout_NotDone();
}

} // namespace ImGuiRunner
} // namespace ImGuiUtils
