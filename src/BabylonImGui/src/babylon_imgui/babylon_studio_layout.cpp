#include "babylon/babylon_imgui/babylon_studio_layout.h"

#include "imgui_internal.h"
#include "imgui_utils/app_runner/imgui_runner_old.h"
#include "imgui_utils/icons_font_awesome_5.h"

namespace BABYLON {

BabylonStudioLayout::BabylonStudioLayout()
{
  _dockableWindowsInfos = {
    {DockableWindowId::Scene3d, {ICON_FA_CUBE " 3D Scene", true}},
    {DockableWindowId::SampleBrowser, {ICON_FA_PALETTE " Browse samples", true}},
    {DockableWindowId::SamplesCodeViewer, {ICON_FA_EDIT " Samples Code Viewer", true}},
#ifdef BABYLON_BUILD_PLAYGROUND
    {DockableWindowId::PlaygroundEditor, {ICON_FA_FLASK " Playground", true}},
#endif
    {DockableWindowId::Logs, {ICON_FA_STREAM " Logs", true}},
    {DockableWindowId::Inspector, {ICON_FA_EDIT " Inspector", true}},
  };
}

std::vector<BABYLON::DockableWindowId> BabylonStudioLayout::dockableWindowsList()
{
  std::vector<DockableWindowId> r;
  for (const auto& kv : _dockableWindowsInfos) {
    r.push_back(kv.first);
  }
  return r;
}

void BabylonStudioLayout::registerGuiRenderFunction(DockableWindowId id, GuiRenderFunction fn)
{
  _dockableWindowsInfos.at(id).RenderFunction = fn;
}

void BabylonStudioLayout::renderGui()
{
  for (const auto& kv : _dockableWindowsInfos) {
    const DockableWindowInfo& dockableWindowInfo = kv.second;
    if (dockableWindowInfo.RenderFunction && dockableWindowInfo.IsVisible) {
      bool visible = true;
      ImGui::Begin(dockableWindowInfo.Label.c_str(), &visible);
      dockableWindowInfo.RenderFunction();
      ImGui::End();
      setVisible(kv.first, visible);
    }
  }
}

void BabylonStudioLayout::renderMenu()
{
  if (ImGui::BeginMenu("Layout")) {
    static std::map<LayoutMode, std::string> layoutModeLabels{
      {LayoutMode::Scene, ICON_FA_CUBE " Scene"},
      {LayoutMode::SceneAndBrowser, ICON_FA_CUBES " Scene and Samples Browser"},
      {LayoutMode::Dev, ICON_FA_FILE_CODE " Dev"},
    };
    for (const auto& kv : layoutModeLabels) {
      if (ImGui::MenuItem(kv.second.c_str(), "", _layoutMode == kv.first)) {
        _layoutMode = kv.first;
        ApplyLayoutMode(_layoutMode);
      }
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Windows")) {
    for (const auto& kv : _dockableWindowsInfos) {
      const DockableWindowInfo& dockableWindowInfo = kv.second;
      auto id                                      = kv.first;
      if (dockableWindowInfo.RenderFunction) {
        if (ImGui::MenuItem(dockableWindowInfo.Label.c_str(), "", isVisible(id)))
          setVisible(id, !isVisible(id));
      }
    }
    ImGui::EndMenu();
  }
}

bool BabylonStudioLayout::isVisible(DockableWindowId id)
{
  return _dockableWindowsInfos.at(id).IsVisible;
}

void BabylonStudioLayout::setVisible(DockableWindowId id, bool v)
{
  _dockableWindowsInfos.at(id).IsVisible = v;
}

std::string BabylonStudioLayout::Label(DockableWindowId id)
{
  return _dockableWindowsInfos.at(id).Label;
}

const char* BabylonStudioLayout::Label_c(DockableWindowId id)
{
  return _dockableWindowsInfos.at(id).Label.c_str();
}

void BabylonStudioLayout::DoDock(DockableWindowId id, ImGuiID dockId)
{
  ImGui::DockBuilderDockWindow(Label_c(id), dockId);
}


ImGuiID BabylonStudioLayout::DoSplit(ImGuiID* io_dockToSplit, ImGuiDir_ splitDirection, float ratio)
{
  ImGuiID new_dock
    = ImGui::DockBuilderSplitNode(*io_dockToSplit, splitDirection, ratio, nullptr, io_dockToSplit);
  return new_dock;
}

bool BabylonStudioLayout::IsShowingSceneOnly()
{
  if (!isVisible(DockableWindowId::Scene3d))
    return false;
  for (auto id : dockableWindowsList())
    if (isVisible(id) && id != DockableWindowId::Scene3d)
      return false;
  return true;
}

void BabylonStudioLayout::PrepareLayout(ImGuiID fullDockSpaceId)
{
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::DockBuilderRemoveNode(fullDockSpaceId); // Clear out existing layout
  ImGui::DockBuilderAddNode(fullDockSpaceId);    // Add empty node
  ImGui::DockBuilderSetNodeSize(fullDockSpaceId, viewport->Size);

  ImGuiID dock_main_id = fullDockSpaceId;
  ImGuiID dock_id_bottom = DoSplit(&dock_main_id, ImGuiDir_Down, 0.25f);
  ImGuiID dock_id_left = DoSplit(&dock_main_id, ImGuiDir_Left, 0.30f);
  ImGuiID dock_id_right = DoSplit(&dock_main_id, ImGuiDir_Right, 0.45f);

#ifdef BABYLON_BUILD_PLAYGROUND
  DoDock(DockableWindowId::PlaygroundEditor, dock_main_id);
#endif
  DoDock(DockableWindowId::SamplesCodeViewer, dock_main_id);

  if (_layoutMode == LayoutMode::Dev) {
    DoDock(DockableWindowId::Inspector, dock_main_id);
    DoDock(DockableWindowId::SampleBrowser, dock_main_id);
  }
  else {
    DoDock(DockableWindowId::Inspector, dock_id_left);
    DoDock(DockableWindowId::SampleBrowser, dock_id_left);
  }

  DoDock(DockableWindowId::Scene3d, dock_id_right);
  DoDock(DockableWindowId::Logs, dock_id_bottom);

  ImGui::DockBuilderFinish(fullDockSpaceId);
}

void BabylonStudioLayout::FocusWindow(DockableWindowId id)
{
  ImGuiWindow* window = ImGui::FindWindowByName(Label_c(id));
  ImGui::FocusWindow(window);
}

void BabylonStudioLayout::ApplyLayoutMode(LayoutMode mode)
{
  _layoutMode = mode;
  switch (_layoutMode) {

    case LayoutMode::Scene:
      for (auto id : dockableWindowsList())
        setVisible(id, false);
      setVisible(DockableWindowId::Scene3d, true);
      break;

    case LayoutMode::SceneAndBrowser:
      setVisible(DockableWindowId::Scene3d, true);
      setVisible(DockableWindowId::Inspector, false);
      setVisible(DockableWindowId::SampleBrowser, true);
      setVisible(DockableWindowId::SamplesCodeViewer, false);
#ifdef BABYLON_BUILD_PLAYGROUND
      setVisible(DockableWindowId::PlaygroundEditor, false);
#endif
      setVisible(DockableWindowId::Logs, false);
      break;

    case LayoutMode::Dev:
      for (auto id : dockableWindowsList())
        setVisible(id, true);
      break;
    default:
      break;
  }
  ImGuiUtils::ImGuiRunner::ResetDockLayout();
}

} // namespace BABYLON
